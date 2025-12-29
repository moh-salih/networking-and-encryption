#pragma once
#include <vector>
#include <cstdint>

#include "crypto/core/types.h"


namespace crypto::core::symmetric {
    using crypto::core::Bytes;

    class IBlockCipher {
    public:
        virtual ~IBlockCipher() = default;

        virtual void setKey(const Bytes& key) = 0;
        virtual size_t blockSize() const noexcept = 0;
        virtual size_t keySize() const noexcept = 0;

        virtual void encryptBlock(const Bytes& in, Bytes& out) const = 0;
        virtual void decryptBlock(const Bytes& in, Bytes& out) const = 0;
    };

}
