#pragma once
#include "ICipher.h"
#include <string>

namespace crypto::classic {  
    class Playfair : public ICipher {
    public:
        explicit Playfair(const std::string& key) : m_key(key) {}
        std::string encrypt(const std::string& plaintext) const override;
        std::string decrypt(const std::string& ciphertext) const override;
        std::string name() const override { return "Playfair Cipher"; }
    private:
        std::string m_key;
    };
}
