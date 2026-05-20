# Multiplayer Lobby UI Subsystem

This document provides a comprehensive reverse-engineered analysis of the Multiplayer Lobby UI subsystem in *Bulanci*. This subsystem coordinates the DirectPlay network session browser, host configuration dialogues, and multiplayer lobby text chats.

---

## 1. Overview of Classes

The lobby UI consists of five primary MSVC C++ classes, operating on top of the custom GUI engine (`CWindow` -> `CListViewer` -> `CListBox` / `CEdit` / `CButton`) and the DirectPlay wrapper (`CDSDirectPlay`):

| Class Name | Size | Base Class | Responsibility |
|------------|------|------------|----------------|
| `CTcpIpConfig` | ~0xa0 bytes | `CWindow` | Host/IP entry popup dialog for TCP/IP connection. |
| `CSessionList` | ~0xf4 bytes | `CWindow` | Main session-browser dialog listing active game lobbies. |
| `CSessionItem` | ~0x24 bytes | `CListBoxItem` | List item wrapping a DirectPlay session instance. |
| `CChatEdit` | `0xc4` (196) bytes | `CEdit` | Input text-box for typing chat messages. |
| `CChatList` | `0xe8` (232) bytes | `CListBox` | Text-scrollback display showing formatted chat logs. |

---

## 2. Deep Dive Analysis

### A. CTcpIpConfig (Host Configuration Dialogue)
* **Constructor / Builder:** `CTcpIpConfig::Build` (`0x0040c060`)
* **Behavior:**
  1. Configures the parent window dimensions to `0x138`x`0xa0` (312x160 pixels) using `CWindow::FUN_00405560`.
  2. Allocates and builds a `CStaticText` prompt at (20, 20) with wide-text from `CDSStaticTexts` pool index **31**:  
     `"Zadejte název nebo IP adresu počítače.\nNezadá-li se nic, bude se počítač hledat."`  
     *(English: "Enter name or IP address of the computer. If left empty, the computer will be searched for.")*
  3. Allocates and builds a `CEdit` box at (20, 70) for user entry.
  4. Builds an "Ok" `CButton` (string index **29**) at (73, 115).
  5. Builds a "Zrušit" (Cancel) `CButton` (string index **30**) at (163, 115).

### B. CSessionList (Session Browser Dialogue)
* **Builder:** `CSessionList::Build` (`0x0040c2d0`)
* **Behavior:**
  1. Sets window dimensions to `0x14f`x`0xf4` (335x244 pixels).
  2. Builds a `CStaticText` status label at (20, 20) with string index **32**: `"Inicializuje se hledání..."` *(English: "Initializing search...")*. This text is stored in `this + 0x74`.
  3. Builds a `CListBox` at (20, 50) of size 315x180 pixels (`this + 0x70`) to render the found lobbies.
  4. Builds a "Připojit" (Join) `CButton` (string index **33**) at (83, 200), stored in `this + 0x78` and initially **disabled** via `_Globals::FUN_0042d080`.
  5. Builds a "Zrušit" (Cancel) `CButton` (string index **30**) at (173, 200), stored in `this + 0x7c`.
* **Event Handler:** `CSessionList::OnEvent` (`0x0040adc0`)
  * Listens for control notifications. Specifically, when the session list box triggers a selection change event (`0xd0`), it checks whether any item is currently selected (`*(int*)(list_box + 0xc4) != 0`).
  * If selected, it enables the "Připojit" button; otherwise, it disables it.

### C. CSessionItem (Session Entry Wrapper)
* **Initializer:** `CSessionItem::Initialize` (`0x0040e8b0`)
* **DirectPlay Interface Mapping:**
  * Takes a pointer to a DirectPlay `DPSESSIONDESC2` structure (`param_1`).
  * Extracts the ANSI session name (`lpszSessionNameA`) at offset **`0x30`** (`*(LPCSTR*)(param_1 + 0x30)`).
  * Converts the ANSI name to UTF-16 using `MultiByteToWideChar` wrapper `_Globals::FUN_00401290` (passing the active thread code-page).
  * Constructs a `std::wstring` and passes it to the `CListBoxItem` base constructor (`0x0040b640`) to set the row's display string.
  * Stores the unique 16-byte DirectPlay instance GUID (`guidInstance` at `param_1 + 8`) into the class's member fields at offsets `0x14`, `0x18`, `0x1c`, and `0x20`. This allows the game to reference and connect to this precise session when the player clicks "Připojit".

---

## 3. The Chat Subsystem

The lobby chat runs via tight integration between `CChatEdit` (input) and `CChatList` (output scrollback).

### A. Chat Input Processing (`CChatEdit::OnChar` @ `0x0040cee0`)
This keypress handler intercepts characters. When the user presses **Enter** (`\r`):
1. It validates that the input box has content.
2. It parses the content for cheat codes or commands:
   * **Cheat Code - `"clearchat"`:**  
     If the user types `"clearchat"`, it triggers a clean-up routine: calls `_Globals::FUN_00405c40` on the associated `CChatList` pointer at `this + 0xbc`, followed by calling its virtual clear method to wipe all chat history.
3. **Regular Chat Messages:**  
   If it is a standard message, it retrieves the sender's color ID/index and invokes `CGame::NetSendChat` on the global game manager (`this + 0xc0`). It then clears the input buffer.

### B. Chat Network Wire Format (`CGame::NetSendChat` @ `0x00413b90`)
Chat transmission is implemented directly on top of DirectPlay. When broadcasting a message:
1. It first calls `CGame::BroadcastEvent` (`0x0042eff0`) with event ID `0xe1` to display the chat message locally in the sender's own `CChatList` dialogue immediately.
2. It then serializes and transmits the chat packet over DirectPlay if the connection is active (`this + 0x1dc`):

```
+--------------------+---------------------+-----------------------------------+
|  Type ID (1 byte)  | Player ID (1 byte)  |  Null-Terminated UTF-16 (Var len) |
+--------------------+---------------------+-----------------------------------+
|     0x06 (Chat)    |    Color/Player     |        L"Message content\0"       |
+--------------------+---------------------+-----------------------------------+
```

The payload is packaged into a buffer of size `2 * text_len + 4` bytes and broadcasted using `_Globals::CDSDirectPlay_Send`.

### C. Chat Output & The Developer Credits Easter Egg (`CChatList::OnEvent` @ `0x0040d0b0`)
Handles the local message broadcast (`0xe1`) to populate the chat list box:
1. Looks up the sender's player name using their index via `_Globals::FUN_00412910`.
2. Formats the string as: `L"%s: %s"` (e.g., `"Player1: hello"`).
3. Allocates a `CListBoxItem`, constructs it with the formatted string, and appends it to the list box (`_Globals::FUN_00407710` with index `-1` representing the end).
4. Recalculates horizontal scrollbars via `CChatList::RecalculateHorizontalExtent` (`0x00405ea0`) to ensure long lines can be scrolled through.
5. Scrolls to the bottom to focus the latest message.

#### 🕵️‍♂️ The `"dormancy6363"` Cheat Code / Easter Egg
During the string analysis of the inbound message, the game checks if the user has typed the secret phrase:
`"dormancy6363"`

If matched, the game prints out a hidden **credits roll** directly into the multiplayer chat window! The list box receives the following sequential entries:
* `L"DeepSlumber: Tuto hru vyvíjeli:"` *(Czech: "DeepSlumber: This game was developed by:")*
* `L"   Radek Matějka"` (Lead programmer)
* `L"   Lubor Kopecký"` (Artist/Animator)
* `L"   Kamila Hurníková"` (Producer/Management)
* `L"   Jaroslav Wagner"` (Creative Manager / Sound)

---

## 4. Renamed Functions in Ghidra

| Original Address | Original Symbol Name | New Reverse-Engineered Name | Class Context |
|------------------|----------------------|-----------------------------|---------------|
| `0x0040b9f0` | `FUN_0040b9f0` | `CChatEdit::GetClassMeta` | `CChatEdit` |
| `0x0040ce50` | `FUN_0040ce50` | `CChatEdit::Build` | `CChatEdit` |
| `0x0040cee0` | `FUN_0040cee0` | `CChatEdit::OnChar` | `CChatEdit` |
| `0x0040eca0` | `FUN_0040eca0` | `CChatEdit::Create` | `CChatEdit` |
| `0x00413b90` | `CGame_NetSendChat_t06` | `CGame::NetSendChat` | `CGame` |
| `0x0042eff0` | `FUN_0042eff0` | `CGame::BroadcastEvent` | `CGame` |
| `0x0040d010` | `FUN_0040d010` | `CChatList::Build` | `CChatList` |
| `0x0040d090` | `FUN_0040d090` | `CChatList::GetClassMeta` | `CChatList` |
| `0x0040d0b0` | `FUN_0040d0b0` | `CChatList::OnEvent` | `CChatList` |
| `0x0040ff20` | `FUN_0040ff20` | `CChatList::Create` | `CChatList` |
| `0x0040b770` | `FUN_0040b770` | `CSessionItem::GetClassMeta` | `CSessionItem` |
| `0x0040e8b0` | `FUN_0040e8b0` | `CSessionItem::Initialize` | `CSessionItem` |
| `0x0040adc0` | `FUN_0040adc0` | `CSessionList::OnEvent` | `CSessionList` |
| `0x0040c2d0` | `FUN_0040c2d0` | `CSessionList::Build` | `CSessionList` |
| `0x0040c530` | `FUN_0040c530` | `CSessionList::GetClassMeta` | `CSessionList` |
| `0x0040c060` | `CTcpIpConfig_ctor` | `CTcpIpConfig::Build` | `CTcpIpConfig` |
| `0x0040c2c0` | `FUN_0040c2c0` | `CTcpIpConfig::GetClassMeta`| `CTcpIpConfig` |
