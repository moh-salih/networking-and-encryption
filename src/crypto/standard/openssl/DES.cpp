#include <stdexcept>
#include <openssl/evp.h>
#include "crypto/standard/openssl/DES.h"

namespace crypto::standard::openssl {

    DES::DES() {}

    DES::~DES() {
        // Securely wipe key and IV from memory
        OPENSSL_cleanse(m_key.data(), m_key.size());
        OPENSSL_cleanse(m_iv.data(), m_iv.size());
    }

    void DES::setKey(const Bytes& key) {
        if (key.size() != 8) {
            throw std::invalid_argument("DES key must be exactly 8 bytes (64 bits)");
        }
        m_key = key;
    }

    void DES::setIV(const Bytes& iv) {
        if (iv.size() != 8) {
            throw std::invalid_argument("DES IV must be exactly 8 bytes");
        }
        m_iv = iv;
    }

    Bytes DES::encrypt(const Bytes& plaintext) const {
        if (m_key.empty() || m_iv.empty()) {
            throw std::runtime_error("Key or IV not set for DES");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("Failed to create EVP context");

        // Initialize DES CBC
        if (EVP_EncryptInit_ex(ctx, EVP_des_cbc(), nullptr, m_key.data(), m_iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES encryption initialization failed");
        }

        Bytes ciphertext(plaintext.size() + blockSize());
        int len = 0, total = 0;

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), static_cast<int>(plaintext.size())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES encryption update failed");
        }
        total = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES encryption finalization failed");
        }
        total += len;

        ciphertext.resize(total);
        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }

    Bytes DES::decrypt(const Bytes& ciphertext) const {
        if (m_key.empty() || m_iv.empty()) {
            throw std::runtime_error("Key or IV not set for DES");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("Failed to create EVP context");

        if (EVP_DecryptInit_ex(ctx, EVP_des_cbc(), nullptr, m_key.data(), m_iv.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES decryption initialization failed");
        }

        Bytes plaintext(ciphertext.size());
        int len = 0, total = 0;

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), static_cast<int>(ciphertext.size())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES decryption update failed");
        }
        total = len;

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DES decryption finalization failed");
        }
        total += len;

        plaintext.resize(total);
        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    }

} // namespace crypto::standard::openssl