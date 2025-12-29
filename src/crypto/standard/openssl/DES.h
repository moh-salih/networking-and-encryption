#pragma once
#include <vector>
#include <cstdint>
#include "crypto/core/types.h"

namespace crypto::standard::openssl {

    using crypto::core::Bytes;

    /**
     * @brief Wrapper for OpenSSL's DES implementation in CBC mode.
     * Block size: 8 bytes
     * Key size: 8 bytes
     */
    class DES {
    public:
        DES();
        ~DES();

        void setKey(const Bytes& key);
        void setIV(const Bytes& iv);

        Bytes encrypt(const Bytes& plaintext) const;
        Bytes decrypt(const Bytes& ciphertext) const;

        size_t keySize() const noexcept { return 8; }
        size_t blockSize() const noexcept { return 8; }

    private:
        Bytes m_key;
        Bytes m_iv;
    };

} // namespace crypto::standard::openssl