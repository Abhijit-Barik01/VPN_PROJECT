#pragma once
#include "CryptoDefs.h"
#include <memory>

namespace vpn::crypto {

    class KeyExchange {
    public:
        KeyExchange();
        ~KeyExchange();

        // Generate ephemeral key pair
        void Generate();

        // Get public key
        Bytes GetPublicKey() const;

        // Derive shared secret from peer's public key
        Bytes DeriveSharedSecret(const Bytes& peer_public_key);

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

}
