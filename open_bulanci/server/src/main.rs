use std::collections::HashMap;
use std::net::SocketAddr;
use std::sync::Arc;
use std::time::Duration;
use anyhow::Result;
use tokio::net::{TcpListener, UdpSocket};
use tokio::sync::{mpsc, Mutex};
use futures_util::{StreamExt, SinkExt};
use tokio_tungstenite::accept_async;
use tokio_tungstenite::tungstenite::Message as WsMessage;

use bulanci_core::engine::EngineSimulation;
use bulanci_core::{ClientMessage, ServerMessage, FrameInputs};
use bulanci_core::{EngineClock, Scheduler};

// ============================================================================
// Scheduler slots — server uses the same Accumulative Fixed-Timestep
// Catch-up architecture as the client (and the original engine — see
// ghidra_analysis/engine/tick_system.md). The 17ms cadence matches the
// client's SLOT_GAME_TICK so prediction & authority stay in lock-step.
// ============================================================================
const SLOT_GAME_TICK: u32 = 0x40;
const GAME_TICK_MS: u64 = 17;
const SLOT_TIMEOUT_SWEEP: u32 = 0x41;
const TIMEOUT_SWEEP_MS: u64 = 1_000; // purge stale clients every 1s

#[derive(Clone)]
enum ClientTransport {
    Udp { addr: SocketAddr },
    WebSocket { sender: mpsc::UnboundedSender<Vec<u8>> },
}

struct ClientInfo {
    _id: u8,
    name: String,
    transport: ClientTransport,
    inputs: HashMap<u64, FrameInputs>, // tick -> input
    last_seen_tick: u64,
    last_active_time: std::time::Instant,
}

struct ServerState {
    simulation: EngineSimulation,
    clients: HashMap<u8, ClientInfo>,
    next_player_id: u8,
}

impl ServerState {
    fn new() -> Self {
        ServerState {
            simulation: EngineSimulation::new(),
            clients: HashMap::new(),
            next_player_id: 1,
        }
    }
}

#[tokio::main]
async fn main() -> Result<()> {
    println!("Starting Server-Authoritative Bulanci Gateway Server...");

    let state = Arc::new(Mutex::new(ServerState::new()));

    // 1. Bind UDP Gateways for Native Clients
    let udp_addr = "0.0.0.0:34568";
    let udp_socket = Arc::new(UdpSocket::bind(udp_addr).await?);
    println!("UDP Gateway listening on: {}", udp_addr);

    // 2. Bind TCP/WebSocket Gateways for WASM Clients
    let ws_addr = "0.0.0.0:34569";
    let tcp_listener = TcpListener::bind(ws_addr).await?;
    println!("WebSocket Gateway listening on: {}", ws_addr);

    // Spawn UDP receiver task
    let udp_socket_recv = Arc::clone(&udp_socket);
    let state_udp = Arc::clone(&state);
    tokio::spawn(async move {
        let mut buf = vec![0u8; 4096];
        loop {
            match udp_socket_recv.recv_from(&mut buf).await {
                Ok((size, addr)) => {
                    let packet = &buf[..size];
                    if let Ok(msg) = postcard::from_bytes::<ClientMessage>(packet) {
                        if let Err(e) = handle_udp_message(&state_udp, &udp_socket_recv, addr, msg).await {
                            eprintln!("Error handling UDP message: {:?}", e);
                        }
                    }
                }
                Err(e) => {
                    eprintln!("UDP Receive Error: {:?}", e);
                }
            }
        }
    });

    // Spawn WebSocket listener task
    let state_ws = Arc::clone(&state);
    tokio::spawn(async move {
        while let Ok((stream, _addr)) = tcp_listener.accept().await {
            let state_ws_clone = Arc::clone(&state_ws);
            tokio::spawn(async move {
                if let Err(e) = handle_websocket_connection(state_ws_clone, stream).await {
                    eprintln!("WebSocket client error: {:?}", e);
                }
            });
        }
    });

    // 3. Central Game Loop — Accumulative Fixed-Timestep Catch-up.
    //
    // Direct port of the original engine's CDSApp_PumpTick + PulseTasks
    // shape: refresh monotonic clock at the top of every loop iter,
    // dispatch all due scheduler events (with drift-free catch-up), then
    // sleep for 1ms before the next iter. SLOT_GAME_TICK fires at strict
    // 17ms (~58.8Hz) cadence; if a long lock-contention or GC pause
    // stalls the server, the scheduler will fire it back-to-back-to-back
    // in the same iter so the simulation clock catches up to wall clock.
    let state_ticker = Arc::clone(&state);
    let udp_socket_ticker = Arc::clone(&udp_socket);
    tokio::spawn(async move {
        let mut clock = EngineClock::new();
        let mut scheduler = Scheduler::new();
        clock.tick();
        scheduler.register(SLOT_GAME_TICK,     GAME_TICK_MS,    true, clock.elapsed_ms);
        scheduler.register(SLOT_TIMEOUT_SWEEP, TIMEOUT_SWEEP_MS, true, clock.elapsed_ms);

        loop {
            clock.tick();
            let now_ms = clock.elapsed_ms;
            let fired = scheduler.dispatch_due_events(now_ms);

            if !fired.is_empty() {
                let mut sim_ticks_this_iter = 0u32;
                let mut sweep_due = false;
                for slot in &fired {
                    match *slot {
                        SLOT_GAME_TICK     => sim_ticks_this_iter += 1,
                        SLOT_TIMEOUT_SWEEP => sweep_due = true,
                        _ => {}
                    }
                }

                if sim_ticks_this_iter > 0 || sweep_due {
                    let mut server = state_ticker.lock().await;

                    // ---- Periodic dead-client sweep (every 1s). ----
                    if sweep_due {
                        let timeout_duration = Duration::from_secs(5);
                        let mut disconnected_players = Vec::new();
                        server.clients.retain(|id, client| {
                            if client.last_active_time.elapsed() > timeout_duration {
                                disconnected_players.push(*id);
                                println!("Client timed out (inactive): {} (Player {})", client.name, id);
                                false
                            } else {
                                true
                            }
                        });
                        for dp_id in &disconnected_players {
                            let leave_msg = ServerMessage::PlayerLeft { id: *dp_id };
                            if let Ok(serialized) = postcard::to_allocvec(&leave_msg) {
                                broadcast_to_all(&server.clients, &udp_socket_ticker, &serialized).await;
                            }
                        }
                    }

                    // ---- Catch-up game-tick burst. Each step is one full
                    //      simulation tick, advanced deterministically using
                    //      the freshest available input from each client. ----
                    for _ in 0..sim_ticks_this_iter {
                        let current_tick = server.simulation.state.tick;
                        let mut inputs = vec![None; 32];
                        for (player_id, client) in &mut server.clients {
                            let id_idx = *player_id as usize;
                            if id_idx < inputs.len() {
                                let input_to_use = client.inputs.get(&current_tick).cloned()
                                    .or_else(|| client.inputs.get(&client.last_seen_tick).cloned())
                                    .unwrap_or_default();
                                inputs[id_idx] = Some(input_to_use);
                            }
                        }
                        let _expired_timers = server.simulation.tick(&inputs);
                    }

                    // ---- Broadcast ONE state update per loop iter (the
                    //      latest after all catch-up ticks). Clients use the
                    //      `tick` field to drop stale packets. ----
                    if sim_ticks_this_iter > 0 {
                        let update_msg = ServerMessage::StateUpdate {
                            state: server.simulation.state.clone(),
                        };
                        if let Ok(serialized) = postcard::to_allocvec(&update_msg) {
                            broadcast_to_all(&server.clients, &udp_socket_ticker, &serialized).await;
                        }
                    }
                }
            }

            tokio::time::sleep(Duration::from_millis(1)).await;
        }
    });

    // Block main thread indefinitely
    std::future::pending::<()>().await;
    Ok(())
}

async fn handle_udp_message(
    state: &Arc<Mutex<ServerState>>,
    socket: &Arc<UdpSocket>,
    addr: SocketAddr,
    msg: ClientMessage,
) -> Result<()> {
    let mut server = state.lock().await;

    match msg {
        ClientMessage::Connect { name } => {
            // Check if client is already connected
            let already_connected = server.clients.values().any(|c| match &c.transport {
                ClientTransport::Udp { addr: a } => *a == addr,
                _ => false,
            });

            if !already_connected {
                let id = server.next_player_id;
                server.next_player_id += 1;

                println!("UDP Client Connected: {} (assigned Player ID {})", name, id);

                // Add player to simulation state
                // Spawns them at a default position
                server.simulation.add_player(id, 0, 100 + (id as i32 * 50), 200);

                let new_client = ClientInfo {
                    _id: id,
                    name: name.clone(),
                    transport: ClientTransport::Udp { addr },
                    inputs: HashMap::new(),
                    last_seen_tick: 0,
                    last_active_time: std::time::Instant::now(),
                };

                // Notify all about the new player
                let join_msg = ServerMessage::PlayerJoined { id, name: name.clone() };
                if let Ok(serialized) = postcard::to_allocvec(&join_msg) {
                    broadcast_to_all(&server.clients, socket, &serialized).await;
                }

                // Welcome message containing the initial world state
                let welcome = ServerMessage::Welcome {
                    assigned_id: id,
                    initial_state: server.simulation.state.clone(),
                };

                if let Ok(serialized) = postcard::to_allocvec(&welcome) {
                    let _ = socket.send_to(&serialized, addr).await;
                }

                server.clients.insert(id, new_client);
            }
        }
        ClientMessage::Input { tick, inputs } => {
            let current_sim_tick = server.simulation.state.tick;
            // Find player
            if let Some(client) = server.clients.values_mut().find(|c| match &c.transport {
                ClientTransport::Udp { addr: a } => *a == addr,
                _ => false,
            }) {
                client.inputs.insert(tick, inputs);
                client.last_seen_tick = tick;
                client.last_active_time = std::time::Instant::now();

                // Clean old inputs buffer to save memory
                client.inputs.retain(|&t, _| t >= current_sim_tick.saturating_sub(120));
            }
        }
    }

    Ok(())
}

async fn handle_websocket_connection(
    state: Arc<Mutex<ServerState>>,
    stream: tokio::net::TcpStream,
) -> Result<()> {
    let ws_stream = accept_async(stream).await?;
    let (mut ws_writer, mut ws_reader) = ws_stream.split();

    // MPSC queue to handle outgoing network frames to this client
    let (tx, mut rx) = mpsc::unbounded_channel::<Vec<u8>>();

    // Spawn writing task
    tokio::spawn(async move {
        while let Some(bytes) = rx.recv().await {
            if ws_writer.send(WsMessage::Binary(bytes)).await.is_err() {
                break;
            }
        }
    });

    let mut player_id = None;

    // Reading loop
    while let Some(result) = ws_reader.next().await {
        if let Ok(ws_msg) = result {
            match ws_msg {
                WsMessage::Binary(bytes) => {
                    if let Ok(client_msg) = postcard::from_bytes::<ClientMessage>(&bytes) {
                        let mut server = state.lock().await;
                        match client_msg {
                            ClientMessage::Connect { name } => {
                                let id = server.next_player_id;
                                server.next_player_id += 1;
                                player_id = Some(id);

                                println!("WebSocket Client Connected: {} (assigned Player ID {})", name, id);

                                // Add to simulation
                                server.simulation.add_player(id, 0, 100 + (id as i32 * 50), 200);

                                let new_client = ClientInfo {
                                    _id: id,
                                    name: name.clone(),
                                    transport: ClientTransport::WebSocket { sender: tx.clone() },
                                    inputs: HashMap::new(),
                                    last_seen_tick: 0,
                                    last_active_time: std::time::Instant::now(),
                                };

                                // Notify all about join
                                let join_msg = ServerMessage::PlayerJoined { id, name };
                                if let Ok(serialized) = postcard::to_allocvec(&join_msg) {
                                    // Use a temporary dummy socket for broadcast helper (WebSocket clients will receive through sender channel)
                                    let dummy_socket = UdpSocket::bind("0.0.0.0:0").await?;
                                    broadcast_to_all(&server.clients, &Arc::new(dummy_socket), &serialized).await;
                                }

                                // Welcome message
                                let welcome = ServerMessage::Welcome {
                                    assigned_id: id,
                                    initial_state: server.simulation.state.clone(),
                                };
                                if let Ok(serialized) = postcard::to_allocvec(&welcome) {
                                    let _ = tx.send(serialized);
                                }

                                server.clients.insert(id, new_client);
                            }
                            ClientMessage::Input { tick, inputs } => {
                                if let Some(id) = player_id {
                                    let current_sim_tick = server.simulation.state.tick;
                                    if let Some(client) = server.clients.get_mut(&id) {
                                        client.inputs.insert(tick, inputs);
                                        client.last_seen_tick = tick;
                                        client.last_active_time = std::time::Instant::now();

                                        client.inputs.retain(|&t, _| t >= current_sim_tick.saturating_sub(120));
                                    }
                                }
                            }
                        }
                    }
                }
                WsMessage::Close(_) => break,
                _ => {}
            }
        } else {
            break;
        }
    }

    // Handle disconnection
    if let Some(id) = player_id {
        let mut server = state.lock().await;
        if server.clients.remove(&id).is_some() {
            println!("WebSocket Client Disconnected: Player ID {}", id);
            let leave_msg = ServerMessage::PlayerLeft { id };
            if let Ok(serialized) = postcard::to_allocvec(&leave_msg) {
                let dummy_socket = UdpSocket::bind("0.0.0.0:0").await?;
                broadcast_to_all(&server.clients, &Arc::new(dummy_socket), &serialized).await;
            }
        }
    }

    Ok(())
}

async fn broadcast_to_all(
    clients: &HashMap<u8, ClientInfo>,
    udp_socket: &Arc<UdpSocket>,
    serialized_msg: &[u8],
) {
    for client in clients.values() {
        match &client.transport {
            ClientTransport::Udp { addr } => {
                let _ = udp_socket.send_to(serialized_msg, *addr).await;
            }
            ClientTransport::WebSocket { sender } => {
                let _ = sender.send(serialized_msg.to_vec());
            }
        }
    }
}
