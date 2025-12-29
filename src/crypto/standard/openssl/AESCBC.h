#pragma once
#include <vector>
#include <cstdint>


#include "crypto/core/types.h"

namespace crypto::standard::openssl {

    using crypto::core::Bytes;

     enum class AESKeySize : size_t {
        AES_128 = 16,
        AES_192 = 24,
        AES_256 = 32
    };


    class AESCBC {
    public:
        explicit AESCBC(AESKeySize keySize);
        ~AESCBC();

        void setKey(const Bytes& key);
        void setIV(const Bytes& iv);

        Bytes encrypt(const Bytes& plaintext) const;
        Bytes decrypt(const Bytes& ciphertext) const;

        size_t keySize() const noexcept;
        size_t blockSize() const noexcept; 

    private:
        Bytes m_key;
        Bytes m_iv;
        AESKeySize m_keySize;

        
    };

}
