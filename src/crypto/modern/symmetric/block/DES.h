#pragma once
#include <array>
#include <cstdint>

#include "crypto/core/symmetric/IBlockCipher.h"

namespace crypto::modern::block::symmetric {

    using crypto::core::Bytes;

    class DES final : public crypto::core::symmetric::IBlockCipher {
    public:
        DES();
        ~DES() override = default;

        void setKey(const Bytes& key) override;
        void encryptBlock(const Bytes& in, Bytes& out) const override;
        void decryptBlock(const Bytes& in, Bytes& out) const override;

        size_t blockSize() const noexcept override { return 8; }
        size_t keySize() const noexcept override { return 8; }

    private:
        // 16 subkeys, each 48 bits (stored in 64-bit integers for convenience)
        std::array<uint64_t, 16> m_subkeys{};

        // Core DES functions
        void keyExpansion(uint64_t key);
        uint32_t feistel(uint32_t half_block, uint64_t subkey) const;
        
        // Bit permutation helper
        static uint64_t permute(uint64_t input, const uint8_t* table, int n);
    };

} // namespace crypto::modern::block::symmetric