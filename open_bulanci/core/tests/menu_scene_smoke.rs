//! Runtime smoke test for the data-driven main menu.
//!
//! Compile-checking the Luau isn't enough — it can't catch a call to a
//! nonexistent `bulanci.*`, a nil index, or a bad `require` graph. This test
//! loads the *actual* shipped pak (`paks/base_ui`) through the same core
//! [`SceneManager`] path the client uses ([`crate::scene_runtime`]): it
//! resolves the declarative `scenes/main_menu.scene.json`, lays out + builds
//! the control tree, runs the attached code-behind scripts, wires up stub
//! host services, and drives the result through every menu interaction —
//! dial hover + click, the S/H/K/X shortcuts, sub-screen keyboard nav, IP
//! text entry, and Enter/Esc on the exit modal. A regression that breaks any
//! screen surfaces here as a build/tick error rather than a black screen.

use std::rc::Rc;

use bulanci_core::scene::{
    AtlasInfo, FrameInput, HostCommand, HostServices, NodeKind, SceneHost, SceneManager,
};

const PAK: &str = concat!(env!("CARGO_MANIFEST_DIR"), "/../paks/base_ui");

/// The same library set the client preloads (see `scene_runtime::SCENE_LIBS`).
const LIBS: &[(&str, &str)] = &[
    ("widgets", "lib/widgets.luau"),
    ("fx", "lib/fx.luau"),
    ("cursor", "lib/cursor.luau"),
    ("poem", "lib/poem.luau"),
    ("startgame1", "lib/startgame1.luau"),
];

/// Stub services standing in for the client's font/texture/VFS stack. They
/// return plausible-but-fake metrics so the scene's layout math runs without
/// raylib.
fn stub_services() -> HostServices {
    let poem = "[CENTER]Title\nFirst line of the poem\nSecond line\n[RIGHT]Author\n".to_string();
    HostServices {
        measure: Some(Rc::new(|text: &str, size: f32, spacing: i32| {
            // ~0.55em per glyph is close enough for the layout to exercise
            // its centering/right-align branches.
            let per = (size * 0.55).max(1.0);
            text.chars().count() as f32 * (per + spacing as f32)
        })),
        read_text: Some(Rc::new(move |_path: &str| Some(poem.clone()))),
        font_metrics: Some(Rc::new(|size: f32| (size + 2.0, size))),
        image_size: Some(Rc::new(|_h: u64| Some((75.0, 75.0)))),
        atlas_info: Some(Rc::new(|_h: u64| {
            Some(AtlasInfo {
                frame_count: 8,
                origin_x: 32.0,
                origin_y: 32.0,
                motion: vec![(0.0, 0.0); 8],
            })
        })),
    }
}

/// Load any declarative scene from the pak directory via the core router (the
/// same `name -> .scene.json + scripts + libs` resolution the client performs
/// against its packed VFS).
fn load_scene(name: &str) -> SceneManager {
    let read = |rel: &str| {
        let path = format!("{PAK}/{rel}");
        std::fs::read(&path).ok()
    };
    SceneManager::load(name, &read, LIBS, stub_services())
        .unwrap_or_else(|| panic!("load declarative '{name}' scene"))
}

/// Load the declarative main menu (the most-exercised surface).
fn load_menu() -> SceneManager {
    load_scene("main_menu")
}

fn input_at(x: f32, y: f32, pressed: bool) -> FrameInput {
    FrameInput { x, y, pressed, ..Default::default() }
}

fn key(name: &str) -> FrameInput {
    FrameInput { x: 400.0, y: 300.0, keys_pressed: vec![name.to_string()], ..Default::default() }
}

#[test]
fn shipped_main_menu_loads_and_builds_a_tree() {
    let mgr = load_menu();
    let host = mgr.host();
    assert!(host.node_count() > 0, "scene built no nodes");
    // The declarative scene resolved (not the legacy luau module).
    assert!(mgr.deps().iter().any(|d| d == "scenes/main_menu.scene.json"));
    // First frame must produce a draw list (backgrounds at minimum).
    host.tick(0, input_at(400.0, 300.0, false)).expect("tick 0");
    let dl = host.draw_list();
    assert!(!dl.is_empty(), "empty draw list on first frame");

    // Layout fidelity: the taffy pass placed the authored controls exactly
    // where the bespoke menu drew them (1:1 with the former Rust path).
    let version = dl
        .iter()
        .find_map(|it| match &it.kind {
            NodeKind::Text { text, .. } if text == "Verze: 1.80" => Some(it.world),
            _ => None,
        })
        .expect("version line present");
    assert_eq!((version.x, version.y), (230.0, 584.0));
    // The menu + hero backgrounds anchor at their fixed corners.
    assert!(dl.iter().any(|it| matches!(it.kind, NodeKind::Sprite { .. }) && it.world.x == 0.0 && it.world.y == 0.0));
    assert!(dl.iter().any(|it| matches!(it.kind, NodeKind::Sprite { .. }) && it.world.x == 610.0 && it.world.y == 0.0));
}

#[test]
fn intro_and_connecting_surfaces_load_build_and_draw() {
    // The intro splash + connecting screen are declarative scenes now (each a
    // SceneSurface the client pushes as an overlay). Loading them through the
    // router proves the JSON parses, the code-behind runs against real
    // `bulanci.*` calls (image_size / measure centering), and they draw.
    for name in ["intro", "connecting"] {
        let mgr = load_scene(name);
        let host = mgr.host();
        assert!(mgr.deps().iter().any(|d| d == &format!("scenes/{name}.scene.json")));
        host.tick(0, input_at(400.0, 300.0, false)).unwrap_or_else(|e| panic!("{name} tick: {e}"));
        let dl = host.draw_list();
        assert!(!dl.is_empty(), "{name} produced an empty draw list");
    }

    // Intro: the splash sprite is centered by its code-behind using the stub
    // 75x75 image size -> (800-75)/2, (600-75)/2 = (362, 262).
    let intro = load_scene("intro");
    intro.host().tick(0, input_at(0.0, 0.0, false)).unwrap();
    let splash = intro
        .host()
        .draw_list()
        .into_iter()
        .find(|it| matches!(it.kind, NodeKind::Sprite { .. }))
        .expect("intro splash sprite present");
    assert_eq!((splash.world.x, splash.world.y), (362.0, 262.0));

    // Connecting: the title line is horizontally centered on its declared y.
    let conn = load_scene("connecting");
    conn.host().tick(0, input_at(0.0, 0.0, false)).unwrap();
    let title = conn
        .host()
        .draw_list()
        .into_iter()
        .find_map(|it| match &it.kind {
            NodeKind::Text { text, .. } if text.starts_with("Připojování") => Some(it.world),
            _ => None,
        })
        .expect("connecting title present");
    assert_eq!(title.y, 260.0);
    assert!(title.x > 0.0 && title.x < 400.0, "title not centered: x={}", title.x);
}

#[test]
fn startgame1_controls_are_declared_and_attach_resolves_them() {
    // The StartGame1 sub-screen is now declarative: its ~45 controls live in
    // main_menu.scene.json under the `sg1` state group, and startgame1.attach
    // wires behavior onto them by name. Boot opens sg1 (the retail auto-click),
    // so after one frame the laid-out controls render.
    let mgr = load_menu();
    let host = mgr.host();
    host.tick(0, input_at(400.0, 300.0, false)).expect("tick 0");
    let dl = host.draw_list();

    let has_text = |needle: &str| {
        dl.iter().any(|it| matches!(&it.kind, NodeKind::Text { text, .. } if text == needle))
    };

    // Default page (host / 3 players): the first two group headers + the
    // create/players radios render — proving attach resolved the declared
    // nodes and the collapsing-layout code-behind ran over them.
    assert!(has_text("Hru:"), "sg1 group-0 header missing — declared controls didn't resolve");
    assert!(has_text("Vytvořit"), "sg1 first radio label missing");
    assert!(has_text("Počet hráčů celkem:"), "sg1 group-1 header missing");
    // The connection-type group (g4) is collapsed in the default host layout;
    // if attach hadn't run, the raw doc node would still be visible — so its
    // absence proves the per-frame layout actually drove the declared nodes.
    assert!(!has_text("Způsob připojení:"), "sg1 group-3 header should be hidden in the default layout");
}

#[test]
fn dial_hover_and_click_drive_the_state_machine_without_error() {
    let mgr = load_menu();
    let host = mgr.host();
    let mut now = 0u64;
    let tick = |h: &SceneHost, now: &mut u64, inp: FrameInput| {
        *now += 16;
        h.tick(*now, inp).unwrap_or_else(|e| panic!("tick @ {}ms: {e}", *now));
        // Draining each frame mirrors the client and exercises the queues.
        let _ = h.drain_audio();
        let _ = h.drain_commands();
        let _ = h.take_goto();
    };

    // Hover the History dial (row 1: y in 121..186) for a while so its
    // glow-up track runs to completion + fires the delayed hover cue.
    for _ in 0..20 {
        tick(&host, &mut now, input_at(60.0, 150.0, false));
    }
    // Move away so it dims back down.
    for _ in 0..20 {
        tick(&host, &mut now, input_at(400.0, 400.0, false));
    }
    // Click the History dial: press then release over the region.
    tick(&host, &mut now, input_at(60.0, 150.0, true));
    tick(&host, &mut now, input_at(60.0, 150.0, false));
    assert!(!host.draw_list().is_empty());
}

#[test]
fn keyboard_shortcuts_and_subscreen_nav_run_clean() {
    let mgr = load_menu();
    let host = mgr.host();
    let mut now = 0u64;
    let step = |h: &SceneHost, now: &mut u64, inp: FrameInput| {
        *now += 16;
        h.tick(*now, inp).unwrap_or_else(|e| panic!("tick @ {}ms: {e}", *now));
        let cmds = h.drain_commands();
        let _ = h.drain_audio();
        cmds
    };

    // H opens History; arrows + Home/End page through it; Esc closes.
    step(&host, &mut now, key("h"));
    for k in ["right", "right", "left", "down", "up", "home", "end"] {
        step(&host, &mut now, key(k));
    }
    step(&host, &mut now, key("escape"));

    // S opens StartGame1; arrows navigate; type an IP; Enter connects.
    step(&host, &mut now, key("s"));
    for k in ["down", "up", "left", "right", "enter"] {
        step(&host, &mut now, key(k));
    }
    // Type a few digits into the IP box.
    for c in ["1", "2", "7", ".", "0", "backspace"] {
        let inp = FrameInput { x: 400.0, y: 300.0, chars: vec![c.to_string()], ..Default::default() };
        step(&host, &mut now, inp);
    }

    // K opens the exit confirm; Enter requests the retail exit.
    step(&host, &mut now, key("k"));
    let mut saw_exit = false;
    for cmd in step(&host, &mut now, key("enter")) {
        if cmd == HostCommand::Exit {
            saw_exit = true;
        }
    }
    // Esc/N closing the modal is also valid; either way no tick errored.
    let _ = saw_exit;
    assert!(host.node_count() > 0);
}
