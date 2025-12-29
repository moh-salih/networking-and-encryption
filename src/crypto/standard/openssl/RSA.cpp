#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdexcept>
#include <memory>

#include "crypto/standard/openssl/RSA.h"


namespace {
    using crypto::core::Bytes;
    using crypto::core::asymmetric::KeyPair;

    static EVP_PKEY* load_public_key(const Bytes& pem) {
        BIO* bio = BIO_new_mem_buf(pem.data(), pem.size());
        if (!bio) throw std::runtime_error("BIO_new_mem_buf failed");

        EVP_PKEY* key = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);

        if (!key) throw std::runtime_error("Invalid public key");
        return key;
    }

    static EVP_PKEY* load_private_key(const Bytes& pem) {
        BIO* bio = BIO_new_mem_buf(pem.data(), pem.size());
        if (!bio) throw std::runtime_error("BIO_new_mem_buf failed");

        EVP_PKEY* key =
            PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);

        if (!key) throw std::runtime_error("Invalid private key");
        return key;
    }

    Bytes rsa_encrypt_oaep_sha256_impl( const Bytes& plaintext, const Bytes& public_key_pem) {
        EVP_PKEY* pkey = load_public_key(public_key_pem);
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        if (!ctx) throw std::runtime_error("CTX new failed");

        if (EVP_PKEY_encrypt_init(ctx) <= 0)
            throw std::runtime_error("Encrypt init failed");

        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
        EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
        EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256());


        size_t out_len = 0;
        EVP_PKEY_encrypt(ctx, nullptr, &out_len, plaintext.data(), plaintext.size());

        Bytes ciphertext(out_len);

        if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &out_len, plaintext.data(), plaintext.size()) <= 0)
            throw std::runtime_error("Encrypt failed");

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        ciphertext.resize(out_len);
        return ciphertext;
    }

    Bytes rsa_decrypt_oaep_sha256_impl( const Bytes& ciphertext, const Bytes& private_key_pem) {
        EVP_PKEY* pkey = load_private_key(private_key_pem);
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        if (!ctx) throw std::runtime_error("CTX new failed");

        if (EVP_PKEY_decrypt_init(ctx) <= 0)
            throw std::runtime_error("Decrypt init failed");

        EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
        EVP_PKEY_CTX_set_rsa_oaep_md(ctx, EVP_sha256());
        EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, EVP_sha256());


        size_t out_len = 0;
        EVP_PKEY_decrypt(ctx, nullptr, &out_len, ciphertext.data(), ciphertext.size());

        Bytes plaintext(out_len);

        if (EVP_PKEY_decrypt(ctx, plaintext.data(), &out_len, ciphertext.data(), ciphertext.size()) <= 0)
            throw std::runtime_error("Decrypt failed");

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        plaintext.resize(out_len);
        return plaintext;
    }

    KeyPair rsa_generate_keypair_impl(int bits) {
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        if (!ctx) throw std::runtime_error("CTX new failed");

        if (EVP_PKEY_keygen_init(ctx) <= 0)
            throw std::runtime_error("Keygen init failed");

        if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0)
            throw std::runtime_error("Setting keygen bits failed");

        EVP_PKEY* pkey = nullptr;
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
            throw std::runtime_error("Keygen failed");

        EVP_PKEY_CTX_free(ctx);

        BIO* pub_bio = BIO_new(BIO_s_mem());
        BIO* priv_bio = BIO_new(BIO_s_mem());

        if (!PEM_write_bio_PUBKEY(pub_bio, pkey))
            throw std::runtime_error("Writing public key failed");

        if (!PEM_write_bio_PrivateKey(priv_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr))
            throw std::runtime_error("Writing private key failed");

        EVP_PKEY_free(pkey);

        BUF_MEM* pub_buf;
        BIO_get_mem_ptr(pub_bio, &pub_buf);
        Bytes public_key_pem(pub_buf->data, pub_buf->data + pub_buf->length);
        BIO_free(pub_bio);

        BUF_MEM* priv_buf;
        BIO_get_mem_ptr(priv_bio, &priv_buf);
        Bytes private_key_pem(priv_buf->data, priv_buf->data + priv_buf->length);
        BIO_free(priv_bio);

        return KeyPair{ public_key_pem, private_key_pem };
    }
}


namespace crypto::standard::openssl {

    using crypto::core::Bytes;
    using crypto::core::asymmetric::KeyPair;

    KeyPair RSA::generateKeyPair(size_t bits) {
        auto kp = rsa_generate_keypair_impl(static_cast<int>(bits));
        return KeyPair{
            std::move(kp.public_key),
            std::move(kp.private_key)
        };
    }

    Bytes RSA::encrypt(const Bytes& plaintext, const Bytes& public_key) {
        return rsa_encrypt_oaep_sha256_impl(plaintext, public_key);
    }

    Bytes RSA::decrypt(const Bytes& ciphertext, const Bytes& private_key) {
        return rsa_decrypt_oaep_sha256_impl(ciphertext, private_key);
    }

} // namespace crypto::standard::openssl


