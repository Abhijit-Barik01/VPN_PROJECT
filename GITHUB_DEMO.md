# GitHub Demo Guide

This guide helps you create professional demonstration materials for GitHub.

## Quick Demo Script

Use this script to quickly demonstrate VPN functionality:

### 1. One-Command Demo

Save as `demo.ps1`:

```powershell
Write-Host "=== VPN Demo Script ===" -ForegroundColor Green
Write-Host ""

Write-Host "Step 1: Testing VPN connectivity..." -ForegroundColor Cyan
ping 10.0.0.1 -n 4

Write-Host "`nStep 2: Checking interfaces..." -ForegroundColor Cyan
Get-NetAdapter | Where-Object {$_.Name -like "*VPN*"} | Format-Table Name, Status

Write-Host "`nStep 3: Route table..." -ForegroundColor Cyan
route print | Select-String "10.0.0"

Write-Host "`n=== Demo Complete ===" -ForegroundColor Green
```

### 2. Recording Terminal Output

**Using PowerShell Transcription**:
```powershell
# Start recording
Start-Transcript -Path "demo_output.txt"

# Run your commands
.\vpn_server.exe
# ... your demo commands ...

# Stop recording
Stop-Transcript
```

**Using Windows Terminal** (Built-in):
- Right-click terminal → Export text

### 3. Capturing Screenshots

**Using Windows Snipping Tool**:
- Press `Win + Shift + S`
- Select area
- Save to `screenshots/` folder

**Using PowerShell** (automated):
```powershell
Add-Type -AssemblyName System.Windows.Forms
$screen = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
$bitmap = New-Object System.Drawing.Bitmap($screen.Width, $screen.Height)
$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
$graphics.CopyFromScreen($screen.Location, [System.Drawing.Point]::Empty, $screen.Size)
$bitmap.Save("screenshot.png")
```

---

## Creating GIFs for GitHub

### Option 1: ScreenToGif (Recommended)

1. Download: https://www.screentogif.com/
2. Record your terminal
3. Edit frames
4. Export as GIF
5. Add to README

### Option 2: Using FFmpeg

```cmd
# Install ffmpeg
choco install ffmpeg

# Convert video to GIF
ffmpeg -i demo.mp4 -vf "fps=10,scale=800:-1:flags=lanczos" -c:v gif demo.gif
```

---

## GitHub README Structure

Here's a professional README structure for your VPN project:

```markdown
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
```

---

## Screenshot Checklist

Create these screenshots for your GitHub repo:

- [ ] **demo.gif**: Animated GIF showing server start → client connect → ping
- [ ] **server_running.png**: Server terminal with "Listening on UDP 51820"
- [ ] **client_connected.png**: Client terminal with successful connection
- [ ] **ping_success.png**: Ping results showing 0% loss
- [ ] **wireshark_encrypted.png**: Wireshark showing encrypted UDP traffic
- [ ] **architecture_diagram.png**: System architecture (can use draw.io)
- [ ] **performance_chart.png**: Throughput/latency graphs

---

## Publishing Checklist

Before pushing to GitHub:

- [ ] Add comprehensive README.md
- [ ] Include LICENSE file (MIT recommended)
- [ ] Add .gitignore for build artifacts
- [ ] Create release builds
- [ ] Tag first release (v1.0.0)
- [ ] Write release notes
- [ ] Add topics/tags to repo
- [ ] Create GitHub Pages site (optional)

---

## Example Commands to Capture

### Terminal 1 - Server
```cmd
C:\VPN_PROJECT\build\bin\Release> .\vpn_server.exe
Starting VPN Server...
TUN Device created. Please configure IP: 10.0.0.1/24
Command: netsh interface ip set address name="VPNServer" static 10.0.0.1 255.255.255.0
Listening on UDP 51820
[Then configure and show "Ready" message]
```

### Terminal 2 - Client
```cmd
C:\VPN_PROJECT\build\bin\Release> .\vpn_client.exe 127.0.0.1
Starting VPN Client...
TUN Device created. Please configure IP: 10.0.0.2/24
Command: netsh interface ip set address name="VPNClient" static 10.0.0.2 255.255.255.0
Connected to server: 127.0.0.1:51820
```

### Terminal 3 - Testing
```cmd
C:\VPN_PROJECT> ping 10.0.0.1

Pinging 10.0.0.1 with 32 bytes of data:
Reply from 10.0.0.1: bytes=32 time=4ms TTL=128
Reply from 10.0.0.1: bytes=32 time=4ms TTL=128

Ping statistics for 10.0.0.1:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

---

## Advanced: Create Performance Graphs

Use Python to create graphs:

```python
import matplotlib.pyplot as plt

# RTT data
packets = list(range(1, 101))
rtt = [4.2, 4.1, 4.3, ...] # Your actual data

plt.figure(figsize=(10, 6))
plt.plot(packets, rtt)
plt.xlabel('Packet Number')
plt.ylabel('RTT (ms)')
plt.title('VPN Latency Over Time')
plt.grid(True)
plt.savefig('latency_graph.png', dpi=300)
```

---

## Tips for Professional Presentation

1. **Use Badges**: Add status badges to README
   ```markdown
   ![Build](https://img.shields.io/badge/build-passing-brightgreen)
   ![License](https://img.shields.io/badge/license-MIT-blue)
   ```

2. **Code Highlighting**: Use proper language tags in code blocks

3. **Collapsible Sections**: Use `<details>` tags for long content

4. **Tables**: Use tables for structured data

5. **Emojis**: Add emojis to headings for visual appeal (don't overdo it)

6. **Links**: Link to specific files and line numbers

7. **Mermaid Diagrams**: Use for architecture diagrams
   ```mermaid
   sequenceDiagram
       Client->>Server: ClientHello (pub key)
       Server->>Client: ServerHello (pub key)
       Client->>Server: Encrypted Data
   ```
