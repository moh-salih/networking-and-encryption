#include <cctype>
#include "crypto/classic/Caesar.h"

namespace crypto::classic { 
    std::string Caesar::encrypt(const std::string& plaintext) const {
        std::string ciphertext;
        for (char ch : plaintext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                // Use mathematical modulo to handle negative shifts
                int val = (ch - base + m_shift) % 26;
                if (val < 0) val += 26;
                ciphertext += static_cast<char>(base + val);
            } else {
                ciphertext += ch;
            }
        }
        return ciphertext;
    }

    std::string Caesar::decrypt(const std::string& ciphertext) const {
        std::string plaintext;
        for (char ch : ciphertext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                // Use mathematical modulo to handle negative shifts
                int val = (ch - base - m_shift) % 26;
                if (val < 0) val += 26;
                plaintext += static_cast<char>(base + val);
            } else {
                plaintext += ch;
            }
        }
        return plaintext;
    }
}