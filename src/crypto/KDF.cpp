#include "KDF.h"
#include <openssl/kdf.h>
#include <openssl/params.h>
#include <openssl/core_names.h>

namespace vpn::crypto {

    Bytes KDF::Derive(const Bytes& secret, const Bytes& salt, const Bytes& info, size_t length) {
        EVP_KDF* kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
        if (!kdf) throw CryptoException("Failed to fetch HKDF");

        EVP_KDF_CTX* kctx = EVP_KDF_CTX_new(kdf);
        EVP_KDF_free(kdf);
        if (!kctx) throw CryptoException("Failed to create KDF context");

        OSSL_PARAM params[5];
        params[0] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, (char*)"SHA256", 0);
        params[1] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, (void*)secret.data(), secret.size());
        params[2] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, (void*)salt.data(), salt.size());
        params[3] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, (void*)info.data(), info.size());
        params[4] = OSSL_PARAM_construct_end();

        Bytes output(length);
        if (EVP_KDF_derive(kctx, output.data(), length, params) <= 0) {
            EVP_KDF_CTX_free(kctx);
            throw CryptoException("Failed to derive key");
        }

        EVP_KDF_CTX_free(kctx);
        return output;
    }

}
