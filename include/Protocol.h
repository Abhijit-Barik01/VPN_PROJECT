#pragma once
#include <cstdint>
#include <vector>
#include <array>

namespace vpn::protocol {

    enum class PacketType : uint8_t {
        ClientHello = 0x01,
        ServerHello = 0x02,
        Data = 0x03
    };

    struct PacketHeader {
        PacketType type;
    };

    // Serialized: [Type][PublicKey][Nonce]
    // PublicKey: 32 bytes
    // Nonce: 12 bytes (Random)
    constexpr size_t HANDSHAKE_SIZE = 1 + 32 + 12;

    // Serialized: [Type][Nonce][Ciphertext...]
    // Nonce: 12 bytes
    constexpr size_t DATA_HEADER_SIZE = 1 + 12;

    std::vector<uint8_t> CreateClientHello(const std::vector<uint8_t>& pub_key);
    std::vector<uint8_t> CreateServerHello(const std::vector<uint8_t>& pub_key);
    std::vector<uint8_t> CreateDataPacket(const std::vector<uint8_t>& nonce, const std::vector<uint8_t>& ciphertext);

    struct ParsedPacket {
        PacketType type;
        std::vector<uint8_t> payload; // Remaining data
    };

    ParsedPacket ParsePacket(const std::vector<uint8_t>& data);

}
