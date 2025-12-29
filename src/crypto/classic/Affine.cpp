#include <cctype>
#include <stdexcept>

#include "crypto/classic/Affine.h"
#include "crypto/core/utils.h"

namespace crypto::classic { 
    Affine::Affine(int a, int b): m_a(a), m_b(b) {
        if (core::utils::gcd(m_a, 26) != 1) {
            throw std::invalid_argument("Invalid 'a' value: must be coprime with 26");
        }
    }

    std::string Affine::encrypt(const std::string& plaintext) const {
        std::string ciphertext;

        for (char ch : plaintext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                char encryptedChar = static_cast<char>(base + (m_a * (ch - base) + m_b) % 26);
                ciphertext += encryptedChar;
            } else {
                ciphertext += ch; // Non-alphabetic characters are unchanged
            }
        }
        return ciphertext;
    }

    std::string Affine::decrypt(const std::string& ciphertext) const {
        std::string plaintext;
        int a_inv = core::utils::modInverse(m_a, 26);
        if (a_inv == -1) {
            throw std::runtime_error("Modular inverse for the given 'a' does not exist.");
        }

        for (char ch : ciphertext) {
            if (std::isalpha(ch)) {
                char base = std::isupper(ch) ? 'A' : 'a';
                char decryptedChar = static_cast<char>(base + (a_inv * ((ch - base - m_b + 26) % 26)) % 26);
                plaintext += decryptedChar;
            } else {
                plaintext += ch; // Non-alphabetic characters are unchanged
            }
        }
        return plaintext;
    }
}