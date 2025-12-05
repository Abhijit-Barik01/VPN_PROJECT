# Design and Implementation of a Secure Layer-3 VPN System

**Author:** [Your Name]  
**Date:** December 2025  
**Course:** Advanced Network Security

---

## 1. Abstract
This project presents the design and implementation of a custom Layer-3 Virtual Private Network (VPN) system. The system provides secure, encrypted communication over untrusted networks using modern cryptographic primitives (X25519, HKDF, ChaCha20-Poly1305). Implemented in C++20, the VPN leverages the WinTUN driver for high-performance virtual networking on Windows. The result is a functional, production-grade VPN client and server capable of tunneling IP traffic securely.

## 2. Motivation
While established VPN solutions like OpenVPN and WireGuard exist, understanding the underlying mechanisms of secure tunneling requires a ground-up implementation. This project aims to demonstrate mastery of:
- Network programming (UDP, TUN/TAP interfaces).
- Applied cryptography (Key Exchange, AEAD).
- Systems programming in modern C++.

## 3. Literature Review
### 3.1 OpenVPN
OpenVPN uses TLS for key exchange and supports both UDP and TCP. It is highly configurable but carries significant legacy code weight.
### 3.2 WireGuard
WireGuard represents the state-of-the-art, utilizing a noise protocol framework and kernel-space implementation for speed. Our project adopts WireGuard's cryptographic choices (ChaCha20-Poly1305, Curve25519) but runs in user-space for portability and educational clarity.

## 4. Cryptographic Design
The system uses a hybrid cryptosystem:
- **Key Exchange**: Elliptic Curve Diffie-Hellman (ECDH) using X25519. This provides forward secrecy and efficient key generation.
- **Key Derivation**: HKDF-SHA256 is used to derive symmetric session keys (Tx/Rx) from the shared secret.
- **Authenticated Encryption**: ChaCha20-Poly1305 (AEAD) ensures confidentiality, integrity, and authenticity of data packets.

## 5. Protocol Flow
### 5.1 Handshake
1. **ClientHello**: Client sends its ephemeral public key.
2. **ServerHello**: Server receives key, generates its own ephemeral key, computes shared secret, and sends its public key.
3. **Session Established**: Both sides derive Tx/Rx keys.

### 5.2 Data Transfer
- Packets are captured from the TUN interface.
- Encrypted with ChaCha20-Poly1305 using a unique nonce (counter-based).
- Encapsulated in a UDP packet with a header containing the nonce.
- Decrypted by the peer and injected into the TUN interface.

## 6. Implementation Details
- **Language**: C++20 (Smart pointers, RAII, standard threads).
- **Driver**: WinTUN (High-performance Layer 3 TUN driver).
- **Concurrency**: Multi-threaded architecture separating UDP receive loop and TUN read loop.

## 7. Performance
The system uses UDP to avoid TCP-over-TCP meltdown. WinTUN provides efficient ring-buffer I/O, minimizing context switches compared to legacy TAP drivers.

## 8. Limitations & Future Work
- **Authentication**: Currently relies on anonymous ECDH. Future work includes adding Pre-Shared Keys (PSK) or Certificate-based auth.
- **Platform**: Currently Windows-only due to WinTUN. Linux support via `/dev/net/tun` is a planned extension.

## 9. Conclusion
The project successfully demonstrates a secure VPN tunnel. It validates the feasibility of building high-performance network security tools using modern C++ and standard cryptographic libraries.
