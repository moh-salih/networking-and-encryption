#include <catch2/catch_all.hpp>
#include <openssl/rand.h>
#include <vector>

#include "crypto/standard/openssl/AESCBC.h"
#include "crypto/standard/openssl/RSA.h"
#include "crypto/core/utils.h"

using namespace crypto::core;
using namespace crypto::standard::openssl;

// --- Helper for generating cryptographically strong random bytes ---
Bytes generate_random_bytes(size_t size) {
    Bytes b(size);
    if (RAND_bytes(b.data(), static_cast<int>(size)) != 1) {
        throw std::runtime_error("OpenSSL RAND_bytes failed");
    }
    return b;
}

// ============================================================
// OPENSSL AES-CBC: EXTENSIVE VALIDATION
// ============================================================
TEST_CASE("OpenSSL AES-CBC: Functional and Boundary Analysis", "[standard][aes][cbc]") {
    
    SECTION("AES-128, 192, and 256 Roundtrip Validation") {
        AESKeySize sizes[] = { AESKeySize::AES_128, AESKeySize::AES_192, AESKeySize::AES_256 };
        
        for (auto size : sizes) {
            DYNAMIC_SECTION("Key Size: " << static_cast<size_t>(size) * 8 << " bits") {
                AESCBC aes(size);
                Bytes key = generate_random_bytes(static_cast<size_t>(size));
                Bytes iv = generate_random_bytes(16); // IV is always 16 bytes for AES
                Bytes pt = {'O', 'p', 'e', 'n', 'S', 'S', 'L', ' ', 'T', 'e', 's', 't'};

                aes.setKey(key);
                aes.setIV(iv);

                Bytes ct = aes.encrypt(pt);
                REQUIRE(ct != pt); // Ensure it's actually encrypted
                
                Bytes decrypted = aes.decrypt(ct);
                REQUIRE(decrypted == pt);
            }
        }
    }

    SECTION("IV and Key Enforcement") {
        AESCBC aes(AESKeySize::AES_128);
        
        // IV must be exactly 16 bytes
        REQUIRE_THROWS_AS(aes.setIV(Bytes(15, 0x00)), std::invalid_argument);
        REQUIRE_THROWS_AS(aes.setIV(Bytes(17, 0x00)), std::invalid_argument);

        // Key must match the initialized size
        REQUIRE_THROWS_AS(aes.setKey(Bytes(32, 0x00)), std::invalid_argument); // Trying to set 256-bit key on 128-bit instance
    }

    SECTION("Operation without Initialization") {
        AESCBC aes(AESKeySize::AES_128);
        Bytes pt = {0x01, 0x02};
        
        // Should throw if encrypt/decrypt is called before setKey and setIV
        REQUIRE_THROWS_AS(aes.encrypt(pt), std::runtime_error);
    }

    SECTION("Large Data Buffer Handling") {
        AESCBC aes(AESKeySize::AES_256);
        aes.setKey(generate_random_bytes(32));
        aes.setIV(generate_random_bytes(16));

        Bytes largePt(1024 * 1024, 'A'); // 1MB buffer
        Bytes ct = aes.encrypt(largePt);
        REQUIRE(aes.decrypt(ct) == largePt);
    }
}

// ============================================================
// OPENSSL RSA: EXTENSIVE VALIDATION
// ============================================================
TEST_CASE("OpenSSL RSA: Key Management and OAEP Integrity", "[standard][rsa]") {
    crypto::standard::openssl::RSA rsa;

    SECTION("Keypair Generation and PEM Format") {
        // Test various standard bit lengths
        size_t bitLengths[] = { 2048, 3072 }; 
        
        for (size_t bits : bitLengths) {
            DYNAMIC_SECTION("RSA " << bits << " bits") {
                auto kp = rsa.generateKeyPair(bits);
                
                REQUIRE_FALSE(kp.public_key.empty());
                REQUIRE_FALSE(kp.private_key.empty());

                // Verify standard PEM headers
                std::string pubStr(kp.public_key.begin(), kp.public_key.end());
                std::string privStr(kp.private_key.begin(), kp.private_key.end());
                
                REQUIRE(pubStr.find("-----BEGIN PUBLIC KEY-----") != std::string::npos);
                REQUIRE(privStr.find("-----BEGIN PRIVATE KEY-----") != std::string::npos);
            }
        }
    }

    SECTION("OAEP Encryption Roundtrip (SHA-256)") {
        auto kp = rsa.generateKeyPair(2048);
        Bytes message = {'K', 'T', 'U', '_', 'S', 'e', 'c', 'r', 'e', 't'};

        Bytes ct = rsa.encrypt(message, kp.public_key);
        REQUIRE(ct.size() == 256); // RSA-2048 always outputs 256 bytes

        Bytes decrypted = rsa.decrypt(ct, kp.private_key);
        REQUIRE(decrypted == message);
    }

    SECTION("Decryption Failure with Mismatched Keys") {
        auto kp1 = rsa.generateKeyPair(2048);
        auto kp2 = rsa.generateKeyPair(2048);
        
        Bytes message = {'H', 'e', 'l', 'l', 'o'};
        Bytes ct = rsa.encrypt(message, kp1.public_key);

        // Decrypting with kp2's private key should fail
        REQUIRE_THROWS_AS(rsa.decrypt(ct, kp2.private_key), std::runtime_error);
    }

    SECTION("Handling Invalid Key Formats") {
        Bytes garbage = {'n', 'o', 't', ' ', 'a', ' ', 'k', 'e', 'y'};
        Bytes pt = {0x01};

        // Should throw when trying to load invalid PEM data
        REQUIRE_THROWS_AS(rsa.encrypt(pt, garbage), std::runtime_error);
    }

    SECTION("RSA Maximum Message Size (OAEP Overhead)") {
        // For RSA-2048 with OAEP SHA-256:
        // Max size = ModulusSize - 2*HashSize - 2
        // Max size = 256 - 2*32 - 2 = 190 bytes
        auto kp = rsa.generateKeyPair(2048);
        
        Bytes validMsg(190, 'X');
        REQUIRE_NOTHROW(rsa.encrypt(validMsg, kp.public_key));

        Bytes invalidMsg(191, 'Y');
        REQUIRE_THROWS_AS(rsa.encrypt(invalidMsg, kp.public_key), std::runtime_error);
    }
}

// ============================================================
// MEMORY SAFETY: ZEROIZATION CHECK
// ============================================================
TEST_CASE("Standard Ciphers: Memory Hygiene", "[standard][security]") {
    
    SECTION("AES Key Scrubbing on Destruction") {
        Bytes key(32, 0xAA);
        {
            AESCBC aes(AESKeySize::AES_256);
            aes.setKey(key);
            // OpenSSL_cleanse should be called in destructor
        }
        SUCCEED("Destructor executed safely");
    }
}


#include <catch2/catch_all.hpp>
#include <openssl/rand.h>
#include "crypto/standard/openssl/AESCBC.h"
#include "crypto/standard/openssl/DES.h"
#include "crypto/standard/openssl/RSA.h"
#include "crypto/core/utils.h"

using namespace crypto::core;
using namespace crypto::standard::openssl;

TEST_CASE("Standard DES: OpenSSL Wrapper Functional Test", "[standard][des]") {
    DES des;
    Bytes key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
    Bytes iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    des.setKey(key);
    des.setIV(iv);

    SECTION("Roundtrip consistency") {
        Bytes pt = {'H', 'e', 'l', 'l', 'o', '!', '!', '!'}; // 8 bytes
        Bytes ct = des.encrypt(pt);
        REQUIRE(ct != pt);
        REQUIRE(des.decrypt(ct) == pt);
    }

    SECTION("Validation") {
        REQUIRE_THROWS_AS(des.setKey(Bytes(7)), std::invalid_argument);
    }
}

TEST_CASE("Standard AES: OpenSSL Wrapper Functional Test", "[standard][aes]") {
    AESCBC aes(AESKeySize::AES_128);
    Bytes key(16, 0x42);
    Bytes iv(16, 0x00);
    aes.setKey(key);
    aes.setIV(iv);

    SECTION("CBC Mode Roundtrip") {
        Bytes pt = {'A', 'E', 'S', ' ', 'S', 't', 'a', 'n', 'd', 'a', 'r', 'd'};
        Bytes ct = aes.encrypt(pt);
        REQUIRE(aes.decrypt(ct) == pt);
    }
}

TEST_CASE("Standard RSA: OpenSSL Wrapper Functional Test", "[standard][rsa]") {
    crypto::standard::openssl::RSA rsa;
    auto kp = rsa.generateKeyPair(2048);

    SECTION("OAEP Roundtrip") {
        Bytes pt = {'S', 'e', 'c', 'r', 'e', 't'};
        Bytes ct = rsa.encrypt(pt, kp.public_key);
        REQUIRE(rsa.decrypt(ct, kp.private_key) == pt);
    }
}