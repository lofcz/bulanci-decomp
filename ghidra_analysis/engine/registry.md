# Windows Registry Integration in Bulanci

This document provides a comprehensive, reverse-engineered analysis of the Windows Registry usage in the original `bulanci.exe` (v1.0). Every path, key, value, and behavior documented below has been verified directly from the binary's import cross-references and decompiled logic.

---

## 1. Executive Summary

`bulanci.exe` strictly utilizes standard 32-bit Unicode Windows Registry APIs (`advapi32.dll`) to persist and retrieve game settings. There are no ANSI-based registry accesses, nor is there any key/value deletion or enumeration logic present in the binary.

The game uses exactly **one registry subkey** and **two registry values**:

| Root Key | Subkey Path | Value Name | Type | Purpose |
| :--- | :--- | :--- | :--- | :--- |
| `HKEY_LOCAL_MACHINE` | `Software\SleepTeam\Bulanci` | `Windowed` | `REG_DWORD` | Flag for starting the game in windowed mode (`1`) vs. fullscreen (`0`). |
| `HKEY_LOCAL_MACHINE` | `Software\SleepTeam\Bulanci` | `Config` | `REG_BINARY` | Gzip-compressed binary payload containing player profiles, custom key mappings, audio volume levels, and other game options. |

---

## 2. Registry Access Architecture (`CDSRegKey`)

Registry keys are represented internally by a specialized wrapper class, which we designate as `CDSRegKey`.

### Struct Layout
The class is small (12 bytes) and tracks the active handle and the opened subkey's path:

```cpp
struct CDSRegKey {
    HKEY m_hKey;            // +0x00: Handle to the opened/created registry key
    CDSString m_strPath;    // +0x04: Wrapper around wchar_t* holding the subkey path
};
```

### Core API Mappings
All low-level operations are delegated to dedicated member functions that wrap standard win32 registry APIs:

*   **`InitializeRegistryKey` (`0x00437d60`)**  
    Wraps `RegCreateKeyExW`. Opens or creates the subkey with the specified security access mask (`REGSAM`).
*   **`RegOpenKey` (`0x00437e20`)**  
    Wraps `RegOpenKeyExW`. Opens the subkey.
*   **`RegQueryDword` (`0x00437f40`)**  
    Wraps `RegQueryValueExW` for `REG_DWORD` values.
*   **`RegWriteDword` (`0x00437fb0`)**  
    Wraps `RegSetValueExW` with type `REG_DWORD` (`4`).
*   **`RegQueryBinary` (`0x00438160`)**  
    Wraps `RegQueryValueExW` for `REG_BINARY` (`3`) payloads. Allocates memory if the buffer is too small.
*   **`RegSetValue` (`0x00437fe0`)**  
    Wraps `RegSetValueExW` for `REG_BINARY` (`3`) payloads.

---

## 3. Error and Exception Handling

When a registry operation fails (e.g., if a subkey or value does not exist or access is denied), the game invokes a structured error handler:

*   **`ThrowRegKeyException` (`0x00437ed0`)**  
    Formats a Unicode error message in the format `L"%s\\%s"` (combining the subkey and the value name) along with the Windows API error code, and throws a `CDSRegKeyException`.
*   **`CDSRegKeyException` Class (`0x00437b20` / `0x00437b30` / `0x00437ba0` / `0x00437c40`)**  
    A specialized subclass of `CDSException` that formats and holds registry-specific error information.

---

## 4. Key & Value Catalog

### A. Value: `Windowed`

*   **Subkey:** `HKEY_LOCAL_MACHINE\Software\SleepTeam\Bulanci`
*   **Type:** `REG_DWORD`
*   **Default Behavior:** If the key or value is missing, it is treated as `0` (Fullscreen).
*   **Startup Lifecycle:**  
    During `CDSApp_ctor` (`0x0042b170`), the engine calls `RegOpenKey` to open the SleepTeam subkey. It then queries the value `"Windowed"` via `RegQueryDword`. If the returned value is non-zero, `g_pApp->m_bWindowed` is set to `true`. This governs window creation style (uses borderless popup `WS_POPUP`) and sets DirectDraw cooperative level to `DDSCL_NORMAL` (`8`) instead of exclusive fullscreen (`0x11`).
*   **Shutdown Lifecycle:**  
    During `CDSApp::CDSApp_dtor` (destructor/shutdown sequence at `0x0042b3d0`), the game opens the subkey and writes the current value of `g_pApp->m_bWindowed` to the registry as a `REG_DWORD` using `RegWriteDword`. This persists the player's last chosen window mode (e.g. toggled via Alt+Enter).

### B. Value: `Config`

*   **Subkey:** `HKEY_LOCAL_MACHINE\Software\SleepTeam\Bulanci`
*   **Type:** `REG_BINARY`
*   **Startup Lifecycle:**  
    During initialization, the function `CBulanci::LoadConfig` (`0x0040a440`) is called:
    1. It opens the subkey `HKLM\Software\SleepTeam\Bulanci`.
    2. It queries the `"Config"` value using `RegQueryBinary`.
    3. The retrieved payload is standard Gzip-compressed binary data.
    4. The game instantiates a `CDSGZipStream` to decompress the binary data.
    5. It parses the decompressed buffer sequentially to recover:
        *   Player names and keyboard control bindings.
        *   Audio volumes (SFX and Music).
        *   Custom gameplay options and settings.
*   **Shutdown/Save Lifecycle:**  
    When settings are modified or when the game exits, the function `CBulanci::SaveConfig` (`0x00409cd0`) is called:
    1. It instantiates an in-memory stream wrapper `CDSEasyMemStream`.
    2. It initializes a `CDSGZipStream` over the memory stream to enable compression.
    3. It serializes player profiles, keyboard configurations, volume indices, and game settings into the compressed stream.
    4. It writes the compressed buffer to `HKLM\Software\SleepTeam\Bulanci\Config` as `REG_BINARY` using `RegSetValue`.

---

## 5. Reverse Engineering Notes

*   **Unicode Exclusivity:** All registry path and value names are loaded into memory as UTF-16 wide strings (e.g., `"Windowed"` is stored as `57 00 69 00 6e 00 64 00 6f 00 77 00 65 00 64 00` starting at address `0x00486f40`).
*   **No ANSI Fallbacks:** Inspecting `advapi32.dll` imports confirms `RegOpenKeyExA`, `RegQueryValueExA`, etc., have 0 cross-references in the PE.

---

## 6. OpenBulanci Interop Policy

The cross-platform OpenBulanci reimplementation (`open_bulanci/client/src/settings.rs`) respects the original keys but deliberately separates "read" and "write" roots:

*   **`HKLM\Software\SleepTeam\Bulanci` is treated as READ-ONLY.** OpenBulanci never writes here, even if it has the privilege to do so. This preserves the original install's settings as immutable historical data — useful for users migrating from the legacy executable, and required to keep the original game's separate installation working alongside OpenBulanci.
*   **OpenBulanci writes to `HKCU\Software\OpenBulanci`** under a new product-namespaced subkey. The value names (`Windowed`, `Config`) are reused verbatim so a future "import legacy settings" path is a one-shot copy.
*   **Read priority order on Windows:** `HKCU\Software\OpenBulanci` → `HKLM\Software\SleepTeam\Bulanci` → portable `config.json` next to the exe → file fallback.
*   **Write priority order on Windows:** `HKCU\Software\OpenBulanci` → file fallback (`config.json` next to the exe). The file fallback activates when the registry hive cannot be opened at all — e.g. under an IIS application-pool identity with no user profile loaded, or a Windows Service running as a restricted account.
*   **Portable mode** (any OS): if `config.json` next to the executable exists and is valid JSON (or is empty as a marker), it is used exclusively. The Windows registry is not touched at all in this mode. Drop an empty `config.json` next to the binary to opt in.
