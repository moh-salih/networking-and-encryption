#pragma once
#include <vector>
#include <cstdint>


#include "crypto/core/types.h"

namespace crypto::core::asymmetric {

    using crypto::core::Bytes;

    struct KeyPair {
        Bytes public_key;
        Bytes private_key;
    };

    class IAsymmetricCipher {
    public:
        virtual ~IAsymmetricCipher() = default;

        virtual KeyPair generateKeyPair(size_t bits) = 0;

        virtual Bytes encrypt(const Bytes& plaintext, const Bytes& public_key) = 0;

        virtual Bytes decrypt(const Bytes& ciphertext, const Bytes& private_key) = 0;
    };
}
