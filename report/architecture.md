# System Architecture

## High-Level Overview

```mermaid
graph TD
    User[User Applications] -->|IP Packets| TUN[WinTUN Interface]
    TUN -->|Raw Packets| VPN[VPN Client/Server Process]
    VPN -->|Encrypt| UDP[UDP Socket]
    UDP -->|Encrypted Packets| Internet((Internet))
    Internet -->|Encrypted Packets| UDP
    UDP -->|Decrypt| VPN
    VPN -->|Raw Packets| TUN
    TUN -->|IP Packets| User
```

## Protocol State Machine

```mermaid
sequenceDiagram
    participant Client
    participant Server
    
    Note over Client: Generate Ephemeral Key (Ec, Ec_pub)
    Client->>Server: ClientHello [Ec_pub, Nonce]
    
    Note over Server: Generate Ephemeral Key (Es, Es_pub)
    Note over Server: Shared = ECDH(Es, Ec_pub)
    Note over Server: Keys = HKDF(Shared)
    Server->>Client: ServerHello [Es_pub, Nonce]
    
    Note over Client: Shared = ECDH(Ec, Es_pub)
    Note over Client: Keys = HKDF(Shared)
    Note over Client: Session Established
    
    Client->>Server: Data [Nonce, EncryptedPayload]
    Server->>Client: Data [Nonce, EncryptedPayload]
```

## Packet Formats

### ClientHello / ServerHello
| Field | Size | Description |
|-------|------|-------------|
| Type | 1 | 0x01 (Client) / 0x02 (Server) |
| PubKey| 32 | X25519 Public Key |
| Nonce | 12 | Random Nonce |

### Data Packet
| Field | Size | Description |
|-------|------|-------------|
| Type | 1 | 0x03 (Data) |
| Nonce | 12 | Packet Nonce (Counter) |
| Payload| N | Encrypted IP Packet + Tag (16 bytes) |
