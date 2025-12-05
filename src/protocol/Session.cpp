#include "Session.h"
#include "Protocol.h"
#include "KDF.h"
#include <stdexcept>
#include <cstring>

namespace vpn {

    Session::Session(bool is_server) : is_server_(is_server) {
        key_exchange_.Generate();
    }

    std::vector<uint8_t> Session::InitiateHandshake() {
        if (is_server_) throw std::runtime_error("Server cannot initiate handshake");
        return protocol::CreateClientHello(key_exchange_.GetPublicKey());
    }

    std::vector<uint8_t> Session::HandleHandshake(const std::vector<uint8_t>& packet) {
        auto pp = protocol::ParsePacket(packet);
        
        if (is_server_) {
            if (pp.type != protocol::PacketType::ClientHello) return {};
            
            // Extract Peer Public Key (first 32 bytes of payload)
            if (pp.payload.size() < 32) return {};
            std::vector<uint8_t> peer_key(pp.payload.begin(), pp.payload.begin() + 32);
            
            shared_secret_ = key_exchange_.DeriveSharedSecret(peer_key);
            
            // Derive Keys
            // HKDF(secret, salt, info)
            // We use empty salt and info for simplicity, or "VPNv1"
            std::vector<uint8_t> salt(32, 0);
            std::vector<uint8_t> info = {'V', 'P', 'N', '1'};
            
            // Derive 64 bytes (32 for Tx, 32 for Rx)
            auto keys = crypto::KDF::Derive(shared_secret_, salt, info, 64);
            
            // Server: Tx = keys[0..31], Rx = keys[32..63]
            // Client: Tx = keys[32..63], Rx = keys[0..31] (Opposite)
            tx_key_.assign(keys.begin(), keys.begin() + 32);
            rx_key_.assign(keys.begin() + 32, keys.end());
            
            established_ = true;
            return protocol::CreateServerHello(key_exchange_.GetPublicKey());
        } else {
            if (pp.type != protocol::PacketType::ServerHello) return {};
            
            if (pp.payload.size() < 32) return {};
            std::vector<uint8_t> peer_key(pp.payload.begin(), pp.payload.begin() + 32);
            
            shared_secret_ = key_exchange_.DeriveSharedSecret(peer_key);
            
            std::vector<uint8_t> salt(32, 0);
            std::vector<uint8_t> info = {'V', 'P', 'N', '1'};
            auto keys = crypto::KDF::Derive(shared_secret_, salt, info, 64);
            
            // Client: Tx = keys[32..63], Rx = keys[0..31]
            rx_key_.assign(keys.begin(), keys.begin() + 32);
            tx_key_.assign(keys.begin() + 32, keys.end());
            
            established_ = true;
            return {};
        }
    }

    std::vector<uint8_t> Session::Encrypt(const std::vector<uint8_t>& plaintext) {
        if (!established_) throw std::runtime_error("Session not established");
        
        auto nonce = GenerateNonce(tx_nonce_counter_++);
        auto ciphertext = crypto::AEAD::Encrypt(tx_key_, nonce, plaintext);
        
        return protocol::CreateDataPacket(nonce, ciphertext);
    }

    std::vector<uint8_t> Session::Decrypt(const std::vector<uint8_t>& packet_payload) {
        if (!established_) throw std::runtime_error("Session not established");
        
        // Payload: [Nonce 12][Ciphertext...]
        if (packet_payload.size() < 12) return {};
        
        std::vector<uint8_t> nonce(packet_payload.begin(), packet_payload.begin() + 12);
        std::vector<uint8_t> ciphertext(packet_payload.begin() + 12, packet_payload.end());
        
        auto plaintext = crypto::AEAD::Decrypt(rx_key_, nonce, ciphertext);
        if (!plaintext) return {}; // Decrypt failed
        
        return *plaintext;
    }

    std::vector<uint8_t> Session::GenerateNonce(uint64_t counter) {
        std::vector<uint8_t> nonce(12, 0);
        // Little endian 64-bit counter
        for(int i=0; i<8; ++i) nonce[i] = (counter >> (8*i)) & 0xFF;
        return nonce;
    }

}
