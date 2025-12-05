#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include <stdexcept>

namespace vpn::crypto {

    using Byte = uint8_t;
    using Bytes = std::vector<Byte>;

    // Constants
    constexpr size_t KEY_LEN = 32;
    constexpr size_t NONCE_LEN = 12;
    constexpr size_t TAG_LEN = 16;
    constexpr size_t PUBLIC_KEY_LEN = 32;
    constexpr size_t PRIVATE_KEY_LEN = 32;
    constexpr size_t SHARED_SECRET_LEN = 32;

    class CryptoException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

}
