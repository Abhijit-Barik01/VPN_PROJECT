#pragma once
#include "CryptoDefs.h"

namespace vpn::crypto {

    class KDF {
    public:
        // HKDF-SHA256 Extract + Expand
        static Bytes Derive(const Bytes& secret, const Bytes& salt, const Bytes& info, size_t length);
    };

}
