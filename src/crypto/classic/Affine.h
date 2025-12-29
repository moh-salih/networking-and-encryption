#pragma once
#include <string>

#include "crypto/classic/ICipher.h"

namespace crypto::classic {  
    class Affine : public ICipher {
    public:
        
        explicit Affine(int a, int b);
        std::string encrypt(const std::string& plaintext) const override;
        std::string decrypt(const std::string& ciphertext) const override;
        std::string name() const override { return "Affine Cipher"; }
    private:
        int m_a; // Multiplicative key
        int m_b; // Additive key
    };
}