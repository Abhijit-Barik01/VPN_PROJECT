#include "AEAD.h"
#include <openssl/evp.h>

namespace vpn::crypto {

    Bytes AEAD::Encrypt(const Bytes& key, const Bytes& nonce, const Bytes& plaintext, const Bytes& aad) {
        if (key.size() != KEY_LEN) throw CryptoException("Invalid key length");
        if (nonce.size() != NONCE_LEN) throw CryptoException("Invalid nonce length");

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw CryptoException("Failed to create cipher context");

        if (EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, key.data(), nonce.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw CryptoException("Failed to init encryption");
        }

        // Set AAD
        int len;
        if (!aad.empty()) {
            if (EVP_EncryptUpdate(ctx, NULL, &len, aad.data(), (int)aad.size()) != 1) {
                EVP_CIPHER_CTX_free(ctx);
                throw CryptoException("Failed to set AAD");
            }
        }

        Bytes ciphertext(plaintext.size() + TAG_LEN); // Reserve space for tag? No, tag is separate in OpenSSL usually, but we append it.
        // Actually, ciphertext size is same as plaintext. Tag is retrieved separately.
        
        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), (int)plaintext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw CryptoException("Failed to encrypt");
        }
        int ciphertext_len = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw CryptoException("Failed to finalize encryption");
        }
        ciphertext_len += len;
        ciphertext.resize(ciphertext_len); // Should be same size

        // Get Tag
        Bytes tag(TAG_LEN);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, TAG_LEN, tag.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw CryptoException("Failed to get tag");
        }

        EVP_CIPHER_CTX_free(ctx);

        // Append tag to ciphertext
        ciphertext.insert(ciphertext.end(), tag.begin(), tag.end());
        return ciphertext;
    }

    std::optional<Bytes> AEAD::Decrypt(const Bytes& key, const Bytes& nonce, const Bytes& ciphertext, const Bytes& aad) {
        if (key.size() != KEY_LEN) return std::nullopt;
        if (nonce.size() != NONCE_LEN) return std::nullopt;
        if (ciphertext.size() < TAG_LEN) return std::nullopt;

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) return std::nullopt;

        if (EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), NULL, key.data(), nonce.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return std::nullopt;
        }

        // Set AAD
        int len;
        if (!aad.empty()) {
            if (EVP_DecryptUpdate(ctx, NULL, &len, aad.data(), (int)aad.size()) != 1) {
                EVP_CIPHER_CTX_free(ctx);
                return std::nullopt;
            }
        }

        // Separate tag
        Bytes tag(ciphertext.end() - TAG_LEN, ciphertext.end());
        Bytes actual_ciphertext(ciphertext.begin(), ciphertext.end() - TAG_LEN);

        // Set Tag
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, TAG_LEN, tag.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return std::nullopt;
        }

        Bytes plaintext(actual_ciphertext.size());
        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, actual_ciphertext.data(), (int)actual_ciphertext.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return std::nullopt;
        }
        int plaintext_len = len;

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            return std::nullopt; // Auth failed
        }
        plaintext_len += len;
        plaintext.resize(plaintext_len);

        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    }

}
