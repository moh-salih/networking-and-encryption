#include <cctype>
#include <stdexcept>

#include "crypto/classic/Vigenere.h"

namespace crypto::classic { 
    std::string Vigenere::encrypt(const std::string& plaintext) const {
        std::string ciphertext;
        size_t keyLength = m_key.length();
        size_t keyIndex = 0;

        for (char ch : plaintext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                char keyChar = std::toupper(m_key[keyIndex % keyLength]);
                int shift = keyChar - 'A';
                char encryptedChar = static_cast<char>(base + (ch - base + shift) % 26);
                ciphertext += encryptedChar;
                keyIndex++;
            } else {
                ciphertext += ch; // Non-alphabetic characters are unchanged
            }
        }
        return ciphertext;
    }

    std::string Vigenere::decrypt(const std::string& ciphertext) const {
        std::string plaintext;
        size_t keyLength = m_key.length();
        size_t keyIndex = 0;

        for (char ch : ciphertext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                char keyChar = std::toupper(m_key[keyIndex % keyLength]);
                int shift = keyChar - 'A';
                char decryptedChar = static_cast<char>(base + (ch - base - shift + 26) % 26);
                plaintext += decryptedChar;
                keyIndex++;
            } else {
                plaintext += ch; // Non-alphabetic characters are unchanged
            }
        }
        return plaintext;
    }
} // namespace crypto::classic