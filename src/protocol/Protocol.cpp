#include "Protocol.h"
#include "CryptoDefs.h"
#include <stdexcept>
#include <cstring>
#include <random>

namespace vpn::protocol {

    using namespace vpn::crypto;

    std::vector<uint8_t> CreateClientHello(const std::vector<uint8_t>& pub_key) {
        std::vector<uint8_t> packet;
        packet.push_back(static_cast<uint8_t>(PacketType::ClientHello));
        packet.insert(packet.end(), pub_key.begin(), pub_key.end());
        
        // Random nonce for handshake (optional, but good for uniqueness)
        // We just fill 12 bytes
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for(int i=0; i<12; ++i) packet.push_back(static_cast<uint8_t>(dis(gen)));

        return packet;
    }

    std::vector<uint8_t> CreateServerHello(const std::vector<uint8_t>& pub_key) {
        std::vector<uint8_t> packet;
        packet.push_back(static_cast<uint8_t>(PacketType::ServerHello));
        packet.insert(packet.end(), pub_key.begin(), pub_key.end());
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for(int i=0; i<12; ++i) packet.push_back(static_cast<uint8_t>(dis(gen)));

        return packet;
    }

    std::vector<uint8_t> CreateDataPacket(const std::vector<uint8_t>& nonce, const std::vector<uint8_t>& ciphertext) {
        std::vector<uint8_t> packet;
        packet.push_back(static_cast<uint8_t>(PacketType::Data));
        packet.insert(packet.end(), nonce.begin(), nonce.end());
        packet.insert(packet.end(), ciphertext.begin(), ciphertext.end());
        return packet;
    }

    ParsedPacket ParsePacket(const std::vector<uint8_t>& data) {
        if (data.empty()) throw std::runtime_error("Empty packet");
        
        ParsedPacket pp;
        pp.type = static_cast<PacketType>(data[0]);
        if (data.size() > 1) {
            pp.payload.assign(data.begin() + 1, data.end());
        }
        return pp;
    }

}
