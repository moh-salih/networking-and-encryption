#pragma once
#include "ICipher.h"

namespace crypto::classic {  
    class Vigenere : public ICipher {
    public:
        explicit Vigenere(const std::string& key) : m_key(key) {}
        std::string encrypt(const std::string& plaintext) const override;
        std::string decrypt(const std::string& ciphertext) const override;
        std::string name() const override { return "Vigenere Cipher"; }
    private:
        std::string m_key;
    };
}