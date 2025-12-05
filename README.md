# Professional C++ VPN System

A high-performance, secure Layer-3 VPN implementation in C++20 using WinTUN and OpenSSL.

## 🏗️ Architecture

\`\`\`
┌─────────────┐         UDP/51820        ┌─────────────┐
│   Client    │◄────────────────────────►│   Server    │
│  10.0.0.2   │   Encrypted Tunnel       │  10.0.0.1   │
└──────┬──────┘                          └──────┬──────┘
       │                                        │
   ┌───▼────┐                              ┌───▼────┐
   │  TUN   │                              │  TUN   │
   │ Device │                              │ Device │
   └────────┘                              └────────┘
\`\`\`

## Features
- **Security**: X25519 Key Exchange, HKDF-SHA256, ChaCha20-Poly1305.
- **Performance**: UDP transport, multi-threaded architecture.
- **Platform**: Windows (WinTUN).

## Prerequisites
- **CMake** 3.20+
- **Visual Studio 2019/2022** (C++20 support)
- **OpenSSL** (Install via `vcpkg` or Windows installer)
- **WinTUN** (Download `wintun.dll` from [wintun.net](https://www.wintun.net/))

## Build Instructions
1. Create a build directory:
   ```powershell
   mkdir build
   cd build
   ```
2. Run CMake:
   ```powershell
   cmake ..
   ```
3. Build:
   ```powershell
   cmake --build . --config Release
   ```

## Running
1. Place `wintun.dll` in the `bin/Release` folder.
2. Run Server:
   ```powershell
   ./bin/Release/vpn_server.exe
   ```
3. Run Client:
   ```powershell
   ./bin/Release/vpn_client.exe
   ```
# VPN_PROJECT
<img width="879" height="879" alt="Screenshot 2025-12-02 213858" src="https://github.com/user-attachments/assets/04836eef-e74d-4205-9238-81e58086ffaa" />
