#include <stdexcept>

#include <openssl/evp.h>

#include "crypto/standard/openssl/AESCBC.h"


namespace crypto::standard::openssl {

    static const EVP_CIPHER* cipher_from_key_size(AESKeySize size) {
        switch (size) {
            case AESKeySize::AES_128: return EVP_aes_128_cbc();
            case AESKeySize::AES_192: return EVP_aes_192_cbc();
            case AESKeySize::AES_256: return EVP_aes_256_cbc();
            default: return nullptr;
        }
    }

    AESCBC::AESCBC(AESKeySize keySize): m_keySize(keySize) {}

    AESCBC::~AESCBC() {
        OPENSSL_cleanse(m_key.data(), m_key.size());
        OPENSSL_cleanse(m_iv.data(), m_iv.size());
    }

    void AESCBC::setKey(const Bytes& key) {
        if (key.size() != static_cast<size_t>(m_keySize)) {
            throw std::invalid_argument("Invalid AES key size");
        }
        m_key = key;
    }

    void AESCBC::setIV(const Bytes& iv) {
        if (iv.size() != 16) {
            throw std::invalid_argument("Invalid IV size (must be 16 bytes)");
        }
        m_iv = iv;
    }

    size_t AESCBC::keySize() const noexcept {
        return static_cast<size_t>(m_keySize);
    }

    size_t AESCBC::blockSize() const noexcept {
        return 16;
    }




    Bytes AESCBC::encrypt(const Bytes& plaintext) const {
        if (m_key.empty() || m_iv.empty())
            throw std::runtime_error("Key or IV not set");

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

        const EVP_CIPHER* cipher = cipher_from_key_size(m_keySize);
        if (!cipher) throw std::runtime_error("Unsupported AES key size");

        Bytes ciphertext(plaintext.size() + blockSize());
        int len = 0, total = 0;

        if (EVP_EncryptInit_ex(ctx, cipher, nullptr, m_key.data(), m_iv.data()) != 1)
            throw std::runtime_error("EncryptInit failed");

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                            plaintext.data(),
                            static_cast<int>(plaintext.size())) != 1)
            throw std::runtime_error("EncryptUpdate failed");

        total = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
            throw std::runtime_error("EncryptFinal failed");

        total += len;
        ciphertext.resize(total);

        EVP_CIPHER_CTX_free(ctx);
        return ciphertext;
    }
    Bytes AESCBC::decrypt(const Bytes& ciphertext) const {
        if (m_key.empty() || m_iv.empty())
            throw std::runtime_error("Key or IV not set");

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");

        const EVP_CIPHER* cipher = cipher_from_key_size(m_keySize);
        if (!cipher) throw std::runtime_error("Unsupported AES key size");

        Bytes plaintext(ciphertext.size());
        int len = 0, total = 0;

        if (EVP_DecryptInit_ex(ctx, cipher, nullptr, m_key.data(), m_iv.data()) != 1)
            throw std::runtime_error("DecryptInit failed");

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                            ciphertext.data(),
                            static_cast<int>(ciphertext.size())) != 1)
            throw std::runtime_error("DecryptUpdate failed");

        total = len;

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1)
            throw std::runtime_error("DecryptFinal failed");

        total += len;
        plaintext.resize(total);

        EVP_CIPHER_CTX_free(ctx);
        return plaintext;
    }





} // namespace crypto::standard::openssl