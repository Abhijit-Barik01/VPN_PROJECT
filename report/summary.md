# Project Summary: Secure C++ VPN

## Overview
This project is a fully functional Layer-3 VPN implemented in C++20. It demonstrates advanced concepts in systems programming, network security, and cryptography.

## Key Features
- **Modern C++**: Uses C++20 features, RAII, and smart pointers.
- **Security**: Implements X25519 Key Exchange and ChaCha20-Poly1305 encryption (same as WireGuard).
- **Architecture**: Clean separation of concerns (Crypto, Protocol, TUN, Transport).
- **Performance**: Uses WinTUN driver for high-throughput networking.

## How to Evaluate
1. **Build**: Open the project in Visual Studio or use CMake.
   - `mkdir build && cd build && cmake .. && cmake --build .`
2. **Run**:
   - Start `vpn_server.exe`.
   - Start `vpn_client.exe`.
   - Observe the handshake log.
3. **Code Review**:
   - `src/crypto`: OpenSSL wrappers.
   - `src/tun`: WinTUN driver interaction.
   - `src/protocol`: Packet serialization and state machine.

## Academic Value
This project goes beyond simple socket programming by implementing a full virtual network interface and a custom secure protocol, satisfying the requirements for a capstone or graduate-level systems project.
