#include "KeyExchange.h"
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/ec.h>

namespace vpn::crypto {

    struct KeyExchange::Impl {
        EVP_PKEY* pkey = nullptr;

        Impl() {}
        ~Impl() {
            if (pkey) EVP_PKEY_free(pkey);
        }
    };

    KeyExchange::KeyExchange() : pImpl(std::make_unique<Impl>()) {}
    KeyExchange::~KeyExchange() = default;

    void KeyExchange::Generate() {
        if (pImpl->pkey) EVP_PKEY_free(pImpl->pkey);
        
        // Generate X25519 key
        pImpl->pkey = EVP_PKEY_Q_keygen(NULL, NULL, "X25519");
        if (!pImpl->pkey) {
            throw CryptoException("Failed to generate X25519 key");
        }
    }

    Bytes KeyExchange::GetPublicKey() const {
        if (!pImpl->pkey) throw CryptoException("Key not generated");

        size_t len = 0;
        if (EVP_PKEY_get1_encoded_public_key(pImpl->pkey, NULL) <= 0) {
             // For X25519, we can use EVP_PKEY_get_raw_public_key
             // But let's check if get1_encoded returns SubjectPublicKeyInfo or raw.
             // X25519 usually uses get_raw_public_key.
        }

        // Use raw public key for X25519
        size_t key_len = PUBLIC_KEY_LEN;
        Bytes key(key_len);
        if (EVP_PKEY_get_raw_public_key(pImpl->pkey, key.data(), &key_len) != 1) {
             throw CryptoException("Failed to get raw public key");
        }
        return key;
    }

    Bytes KeyExchange::DeriveSharedSecret(const Bytes& peer_public_key) {
        if (!pImpl->pkey) throw CryptoException("Key not generated");
        if (peer_public_key.size() != PUBLIC_KEY_LEN) throw CryptoException("Invalid peer key length");

        EVP_PKEY* peer_pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, NULL, peer_public_key.data(), peer_public_key.size());
        if (!peer_pkey) throw CryptoException("Failed to create peer key");

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pImpl->pkey, NULL);
        if (!ctx) {
            EVP_PKEY_free(peer_pkey);
            throw CryptoException("Failed to create context");
        }

        if (EVP_PKEY_derive_init(ctx) <= 0 ||
            EVP_PKEY_derive_set_peer(ctx, peer_pkey) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(peer_pkey);
            throw CryptoException("Failed to init derive");
        }

        size_t secret_len = 0;
        if (EVP_PKEY_derive(ctx, NULL, &secret_len) <= 0) {
             EVP_PKEY_CTX_free(ctx);
             EVP_PKEY_free(peer_pkey);
             throw CryptoException("Failed to get secret length");
        }

        Bytes secret(secret_len);
        if (EVP_PKEY_derive(ctx, secret.data(), &secret_len) <= 0) {
             EVP_PKEY_CTX_free(ctx);
             EVP_PKEY_free(peer_pkey);
             throw CryptoException("Failed to derive secret");
        }

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(peer_pkey);
        return secret;
    }

}
