#pragma once
#include <cstdint>
#include <vector>

#include "crypto/core/asymmetric/IAsymmetricCipher.h"

namespace crypto::modern::asymmetric {

class RSA : public crypto::core::asymmetric::IAsymmetricCipher {
public:
    RSA() = default;

    crypto::core::asymmetric::KeyPair generateKeyPair(size_t bits) override;

    crypto::core::Bytes encrypt(
        const crypto::core::Bytes& plaintext,
        const crypto::core::Bytes& public_key
    ) override;

    crypto::core::Bytes decrypt(
        const crypto::core::Bytes& ciphertext,
        const crypto::core::Bytes& private_key
    ) override;

private:
    bool isPrime(uint64_t n) const;
    uint64_t modPow(uint64_t base, uint64_t exp, uint64_t mod) const;
    uint64_t mulMod(uint64_t a, uint64_t b, uint64_t m) const;
    // helpers
    crypto::core::Bytes encodeUint64(uint64_t v) const;
    uint64_t decodeUint64(const crypto::core::Bytes& b, size_t offset) const;
};

}
