#include <catch2/catch_all.hpp>
#include <vector>
#include <algorithm>

#include "crypto/modern/symmetric/block/AES.h"
#include "crypto/modern/symmetric/block/DES.h"
#include "crypto/modern/asymmetric/RSA.h"
#include "crypto/core/utils.h"

using namespace crypto::core;
using namespace crypto::modern;

// ============================================================
// MANUAL AES-128 NIST VALIDATION
// ============================================================
TEST_CASE("Modern AES: NIST Standard Vector Validation", "[modern][aes][nist]") {
    block::symmetric::AES aes(16);
    aes.setKey(utils::fromHex("000102030405060708090a0b0c0d0e0f"));
    
    SECTION("Encryption matches Appendix C.1") {
        Bytes pt = utils::fromHex("00112233445566778899aabbccddeeff");
        Bytes expected_ct = utils::fromHex("69c4e0d86a7b0430d8cdb78070b4c55a");
        Bytes actual_ct;
        aes.encryptBlock(pt, actual_ct);
        REQUIRE(utils::toHex(actual_ct) == utils::toHex(expected_ct));
    }
}

// ============================================================
// MANUAL DES NIST VALIDATION
// ============================================================
TEST_CASE("Modern DES: Standard Vector Validation", "[modern][des][nist]") {
    block::symmetric::DES des;
    // Official DES Test Vector (FIPS 46-3)
    // Key: 0123456789ABCDEF
    // Plaintext: 4E6F772069732074 ("Now is t")
    // Expected Ciphertext: 3FA40E8A984D4815
    des.setKey(utils::fromHex("0123456789ABCDEF"));
    
    SECTION("Block Encryption correctness") {
        Bytes pt = utils::fromHex("4E6F772069732074");
        Bytes expected_ct = utils::fromHex("3FA40E8A984D4815");
        Bytes actual_ct;
        des.encryptBlock(pt, actual_ct);
        REQUIRE(utils::toHex(actual_ct) == utils::toHex(expected_ct));
    }

    SECTION("Block Decryption correctness") {
        Bytes ct = utils::fromHex("3FA40E8A984D4815");
        Bytes expected_pt = utils::fromHex("4E6F772069732074");
        Bytes actual_pt;
        des.decryptBlock(ct, actual_pt);
        REQUIRE(utils::toHex(actual_pt) == utils::toHex(expected_pt));
    }
}

// ============================================================
// MANUAL RSA EDUCATIONAL VALIDATION
// ============================================================
TEST_CASE("Modern RSA: Educational Logic Check", "[modern][rsa]") {
    crypto::modern::asymmetric::RSA rsa;
    auto kp = rsa.generateKeyPair(1024);

    SECTION("Single Byte Roundtrip") {
        // Use a value smaller than n (n=3233 in our demo primes)
        Bytes pt = { 'K' }; // ASCII 75
        Bytes ct = rsa.encrypt(pt, kp.public_key);
        Bytes decrypted = rsa.decrypt(ct, kp.private_key);
        
        REQUIRE(decrypted == pt);
    }
}

// ============================================================
// CORE UTILITIES
// ============================================================
TEST_CASE("Core Utils: Padding Integrity", "[core][utils]") {
    SECTION("PKCS#7 AES (16-byte)") {
        Bytes data = {0xAA, 0xBB};
        utils::pad(data, 16);
        REQUIRE(data.size() == 16);
        REQUIRE(utils::unpad(data) == true);
        REQUIRE(data.size() == 2);
    }

    SECTION("PKCS#7 DES (8-byte)") {
        Bytes data = {0x01, 0x02, 0x03};
        utils::pad(data, 8);
        REQUIRE(data.size() == 8);
        REQUIRE(utils::unpad(data) == true);
        REQUIRE(data.size() == 3);
    }
}


