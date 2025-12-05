#pragma once
#include "CryptoDefs.h"
#include "KeyExchange.h"
#include "AEAD.h"
#include <vector>
#include <cstdint>

namespace vpn {

    class Session {
    public:
        Session(bool is_server);
        
        // Handshake
        std::vector<uint8_t> InitiateHandshake(); // Returns ClientHello
        std::vector<uint8_t> HandleHandshake(const std::vector<uint8_t>& packet); // Returns response (ServerHello) or empty

        // Data
        std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext);
        std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& packet_payload); // Payload starts with Nonce

        bool IsEstablished() const { return established_; }

    private:
        bool is_server_;
        bool established_ = false;
        
        crypto::KeyExchange key_exchange_;
        std::vector<uint8_t> shared_secret_;
        std::vector<uint8_t> tx_key_;
        std::vector<uint8_t> rx_key_;
        
        // Nonce counters (simple increment)
        uint64_t tx_nonce_counter_ = 0;
        
        std::vector<uint8_t> GenerateNonce(uint64_t counter);
    };

}
