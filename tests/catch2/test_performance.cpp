#include <catch2/catch_all.hpp>
#include "crypto/modern/symmetric/block/AES.h"
#include "crypto/modern/symmetric/block/DES.h"
#include "crypto/standard/openssl/AESCBC.h"
#include "crypto/standard/openssl/DES.h"

using namespace crypto::core;

TEST_CASE("Throughput Benchmark: Manual vs Library", "[benchmark]") {
    Bytes key8(8, 0x01);
    Bytes key16(16, 0x01);
    Bytes block8(8, 0x02);
    Bytes block16(16, 0x02);
    Bytes out(16);

    // --- DES Performance ---
    crypto::modern::block::symmetric::DES manualDes;
    manualDes.setKey(key8);
    
    crypto::standard::openssl::DES standardDes;
    standardDes.setKey(key8);
    standardDes.setIV(Bytes(8, 0x00));

    BENCHMARK("Manual DES Block Encrypt") {
        return manualDes.encryptBlock(block8, out);
    };

    BENCHMARK("OpenSSL DES Encrypt (includes context init)") {
        return standardDes.encrypt(block8);
    };

    // --- AES Performance ---
    crypto::modern::block::symmetric::AES manualAes(16);
    manualAes.setKey(key16);

    BENCHMARK("Manual AES-128 Block Encrypt") {
        return manualAes.encryptBlock(block16, out);
    };
}