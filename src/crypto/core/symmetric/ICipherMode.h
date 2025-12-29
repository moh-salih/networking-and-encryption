#pragma once

#include "crypto/core/types.h"

namespace crypto::core::symmetric {
    using crypto::core::Bytes;

    class ICipherMode {
    public:
        virtual ~ICipherMode() = default;

        virtual void setIV(const Bytes& iv) = 0;

        virtual Bytes encrypt(const Bytes& plaintext) = 0;
        virtual Bytes decrypt(const Bytes& ciphertext) = 0;
    };

}
