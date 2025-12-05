#pragma once
#include "CryptoDefs.h"
#include <optional>

namespace vpn::crypto {

    class AEAD {
    public:
        // Encrypt data. Returns ciphertext + tag appended.
        static Bytes Encrypt(const Bytes& key, const Bytes& nonce, const Bytes& plaintext, const Bytes& aad = {});

        // Decrypt data. Input ciphertext should have tag appended.
        static std::optional<Bytes> Decrypt(const Bytes& key, const Bytes& nonce, const Bytes& ciphertext, const Bytes& aad = {});
    };

}
