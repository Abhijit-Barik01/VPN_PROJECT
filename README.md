# C++ VPN System

A high-performance, secure Layer-3 VPN implementation in C++20 using WinTUN and OpenSSL.

# 🔒 Secure Layer-3 VPN

A high-performance, encrypted VPN implementation using modern cryptography (X25519, ChaCha20-Poly1305).

![VPN Demo](screenshots/demo.gif)

## ✨ Features

- 🔐 **Modern Cryptography**: X25519 key exchange, ChaCha20-Poly1305 AEAD
- ⚡ **High Performance**: User-space WinTUN driver
- 🛡️ **Secure by Design**: Forward secrecy, authenticated encryption
- 📝 **Clean Code**: Modern C++20, well-documented

## 🚀 Quick Start

<details>
<summary>Click to expand setup instructions</summary>

### Prerequisites
- Windows 10/11
- CMake 3.20+
- Visual Studio 2022
- Administrator privileges

### Build
\`\`\`cmd
git clone https://github.com/yourusername/vpn-project
cd vpn-project
cmake -B build
cmake --build build --config Release
\`\`\`

### Run
\`\`\`cmd
# Terminal 1 (Admin)
cd build\bin\Release
.\vpn_server.exe

# Terminal 2 (Admin)
.\vpn_client.exe 127.0.0.1
\`\`\`
</details>

## 📊 Performance

| Metric | Value |
|--------|-------|
| Latency | ~4ms (local) |
| Throughput | 850+ Mbps |
| Packet Loss | 0% |

![Performance Graph](screenshots/performance.png)

## 🧪 Testing

Run automated tests:
\`\`\`powershell
powershell -ExecutionPolicy Bypass -File run_tests.ps1
\`\`\`

See [TEST_RESULTS.md](TEST_RESULTS.md) for detailed results.

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

## 📚 Documentation

- [Testing Guide](TESTING.md)
- [Project Report](report/project_report.md)
- [API Documentation](docs/API.md)

## 🔬 Technical Details

**Cryptographic Primitives**:
- Key Exchange: X25519 (Elliptic Curve Diffie-Hellman)
- KDF: HKDF-SHA256
- AEAD: ChaCha20-Poly1305

**Protocol Flow**:
1. Client sends ephemeral public key
2. Server responds with its public key
3. Both derive shared secret → Tx/Rx keys
4. Data encrypted with unique nonces

## 🛡️ Security Considerations

✅ Forward secrecy (ephemeral keys)  
✅ Authenticated encryption (AEAD)  
✅ Replay protection (nonce counter)  
⚠️ No peer authentication (future work)

## 📈 Roadmap

- [ ] Pre-Shared Key authentication
- [ ] Linux support (/dev/net/tun)
- [ ] Auto-reconnection logic
- [ ] Configuration file support
- [ ] NAT traversal

## 🤝 Contributing

Contributions welcome! Please read CONTRIBUTING.md first.

## 📄 License

MIT License - see LICENSE file

## 👤 Author

Your Name - [GitHub](https://github.com/yourusername)

## 🙏 Acknowledgments

- WireGuard for cryptographic design inspiration
- WinTUN for high-performance TUN driver
