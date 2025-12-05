# VPN Testing Guide

This guide provides comprehensive testing scenarios for the VPN implementation.

## Prerequisites

Before testing, ensure you have:
- Built the VPN server and client executables
- Administrator privileges (required for TUN interface)
- Two command prompt windows (Administrator mode)

## Test Scenarios

### 1. Basic Connectivity Test (Ping)

**Purpose**: Verify the tunnel is established and basic IP connectivity works.

**Steps**:
1. Start VPN Server:
```cmd
cd C:\VPN_PROJECT\build\bin\Release
.\vpn_server.exe
```

Expected output:
```
Starting VPN Server...
TUN Device created. Please configure IP: 10.0.0.1/24
Command: netsh interface ip set address name="VPNServer" static 10.0.0.1 255.255.255.0
Listening on UDP 51820
```

2. Configure server interface:
```cmd
netsh interface ip set address name="VPNServer" static 10.0.0.1 255.255.255.0
```

3. Start VPN Client (in another terminal):
```cmd
cd C:\VPN_PROJECT\build\bin\Release
.\vpn_client.exe 127.0.0.1
```

Expected output:
```
Starting VPN Client...
TUN Device created. Please configure IP: 10.0.0.2/24
Command: netsh interface ip set address name="VPNClient" static 10.0.0.2 255.255.255.0
```

4. Configure client interface:
```cmd
netsh interface ip set address name="VPNClient" static 10.0.0.2 255.255.255.0
```

5. Test connectivity:
```cmd
ping 10.0.0.1
```

**Expected Result**: Successful ping responses with low latency (<5ms for local loopback).

---

### 2. Throughput Testing (iperf3)

**Purpose**: Measure bandwidth and throughput through the VPN tunnel.

**Prerequisites**: Install iperf3
```cmd
# Using chocolatey
choco install iperf3
```

**Steps**:
1. Start iperf3 server on server side:
```cmd
iperf3 -s -B 10.0.0.1
```

2. Run iperf3 client from client side:
```cmd
# TCP test
iperf3 -c 10.0.0.1 -t 30

# UDP test
iperf3 -c 10.0.0.1 -u -b 100M -t 30
```

**Expected Result**: Document throughput in Mbps/Gbps, compare with baseline.

---

### 3. File Transfer Test

**Purpose**: Verify data integrity through the tunnel.

**Steps**:
1. Create test file on client side:
```cmd
# Create 100MB test file
fsutil file createnew testfile.bin 104857600
certutil -hashfile testfile.bin SHA256 > hash_original.txt
```

2. Start HTTP server on client side (using Python):
```cmd
python -m http.server 8080 --bind 10.0.0.2
```

3. Download from server side:
```cmd
curl http://10.0.0.2:8080/testfile.bin -o received_file.bin
certutil -hashfile received_file.bin SHA256 > hash_received.txt
```

4. Compare hashes:
```cmd
fc hash_original.txt hash_received.txt
```

**Expected Result**: Hashes match, indicating no data corruption.

---

### 4. HTTP/HTTPS Traffic Test

**Purpose**: Verify application-layer protocols work through VPN.

**Steps**:
1. Start a web server on server side:
```cmd
python -m http.server 80 --bind 10.0.0.1
```

2. Access from client side:
```cmd
curl http://10.0.0.1/
# Or open in browser: http://10.0.0.1/
```

**Expected Result**: Web page loads successfully.

---

### 5. DNS Resolution Test

**Purpose**: Test DNS queries through the tunnel.

**Steps**:
1. Use nslookup with VPN server as DNS:
```cmd
nslookup google.com 10.0.0.1
```

**Note**: This requires running a DNS server on 10.0.0.1 or routing DNS through the tunnel.

---

### 6. Latency Test

**Purpose**: Measure round-trip time through the VPN.

**Steps**:
```cmd
ping 10.0.0.1 -n 100
```

**Metrics to capture**:
- Minimum RTT
- Maximum RTT
- Average RTT
- Packet loss percentage

---

### 7. Encryption Verification

**Purpose**: Verify traffic is encrypted (requires Wireshark).

**Steps**:
1. Start Wireshark capture on loopback interface
2. Filter: `udp.port == 51820`
3. Send ping through VPN
4. Examine UDP payload

**Expected Result**: Payload should appear as random bytes (encrypted), not readable IP packets.

---

### 8. Reconnection Test

**Purpose**: Test connection resilience.

**Steps**:
1. Establish VPN connection
2. Start continuous ping: `ping 10.0.0.1 -t`
3. Kill and restart server
4. Observe reconnection behavior

**Expected Result**: Document recovery time and behavior.

---

### 9. Multiple Packet Sizes Test

**Purpose**: Test fragmentation and MTU handling.

**Steps**:
```cmd
# Small packets
ping 10.0.0.1 -l 64

# Medium packets  
ping 10.0.0.1 -l 1400

# Large packets
ping 10.0.0.1 -l 8000
```

**Expected Result**: All sizes should work correctly.

---

### 10. Concurrent Connections Stress Test

**Purpose**: Test server handling multiple clients.

**Steps**:
1. Start multiple client instances with different IPs
2. Monitor server performance
3. Test cross-client communication

**Expected Result**: Server handles multiple clients without crashes.

---

## Automated Test Script

Save as `run_tests.ps1`:

```powershell
# VPN Automated Test Suite
Write-Host "VPN Test Suite" -ForegroundColor Green

# Test 1: Ping
Write-Host "`n[Test 1] Connectivity Test" -ForegroundColor Cyan
ping 10.0.0.1 -n 10

# Test 2: Traceroute
Write-Host "`n[Test 2] Route Test" -ForegroundColor Cyan
tracert -d -h 5 10.0.0.1

# Test 3: Port connectivity
Write-Host "`n[Test 3] TCP Connectivity" -ForegroundColor Cyan
Test-NetConnection -ComputerName 10.0.0.1 -Port 80

Write-Host "`nTests completed!" -ForegroundColor Green
```

Run with: `powershell -ExecutionPolicy Bypass -File run_tests.ps1`

---

## Results Template

When documenting results for GitHub, use this template:

```markdown
### Test Results

**Date**: [Date]
**System**: Windows 10/11
**Configuration**: Local loopback

| Test | Result | Metrics |
|------|--------|---------|
| Ping | ✅ Pass | RTT: 2ms avg |
| Throughput | ✅ Pass | 850 Mbps |
| File Transfer | ✅ Pass | SHA256 verified |
| Latency | ✅ Pass | 1ms min, 5ms max |

**Screenshots**: [Attach here]
```

---

## Troubleshooting

### Issue: "TUN Device creation failed"
- **Solution**: Run as Administrator

### Issue: "Connection timeout"
- **Solution**: Check firewall rules, verify server is running

### Issue: "Permission denied"
- **Solution**: Enable developer mode or run as admin

---

## Next Steps

After completing tests:
1. Document all results in `RESULTS.md`
2. Capture screenshots of successful tests
3. Create performance comparison charts
4. Add results to GitHub README
