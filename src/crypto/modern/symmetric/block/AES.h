#pragma once
#include <array>

#include "crypto/core/symmetric/IBlockCipher.h"

namespace crypto::modern::block::symmetric{
    static constexpr size_t MAX_AES_SCHEDULE_SIZE = 240;

    using crypto::core::Bytes;

    class AES final : public crypto::core::symmetric::IBlockCipher{
    public:
        AES(size_t keySizeBytes);
        ~AES() override;

        void setKey(const Bytes& bytes) override;

        void encryptBlock(const Bytes& plaintext, Bytes& output) const override;

        void decryptBlock(const Bytes& ciphertext, Bytes& output) const override;

        size_t blockSize() const noexcept override;

        size_t keySize() const noexcept override;

    private:
        std::array<uint8_t, MAX_AES_SCHEDULE_SIZE> m_roundKeys = {};
        size_t m_keySizeBytes;


        void keyExpansion(const Bytes& key);

    private:
        using State = uint8_t[4][4];

        void subBytes(State& state) const;
        void shiftRows(State& state) const;
        void mixColumns(State& state) const;
        void addRoundKey(State& state, size_t round) const;

        void invSubBytes(State& state) const;
        void invShiftRows(State& state) const;
        void invMixColumns(State& state) const;
    };

} // namespace crypto::modern::symmetric::block