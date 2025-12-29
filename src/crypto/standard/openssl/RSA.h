#pragma once
#include <vector>
#include <cstdint>

#include "crypto/core/asymmetric/IAsymmetricCipher.h"

namespace crypto::standard::openssl {
    using namespace crypto::core::asymmetric;

    class RSA : public crypto::core::asymmetric::IAsymmetricCipher {
    public:
        KeyPair generateKeyPair(size_t bits) override;

        Bytes encrypt(const Bytes& plaintext, const Bytes& public_key) override;

        Bytes decrypt(const Bytes& ciphertext, const Bytes& private_key) override;
    };
}
