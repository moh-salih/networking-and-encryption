#pragma once
#include "ICipher.h"

namespace crypto::classic {  
    class Caesar : public ICipher {
    public:
        explicit Caesar(int shift) : m_shift(shift % 26) {}
        std::string encrypt(const std::string& plaintext) const override;
        std::string decrypt(const std::string& ciphertext) const override;
        std::string name() const override { return "Caesar Cipher"; }
    private:
        int m_shift;
    };
}