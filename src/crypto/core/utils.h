#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "crypto/core/types.h" 

namespace crypto::core::utils {
    // Math Utilities
    int gcd(int a, int b);
    int modInverse(int a, int m);

    // Data Conversion Utilities
    std::string toHex(const Bytes& bytes);
    Bytes fromHex(const std::string& hex);


    void pad(Bytes& data, size_t blockSize);

    bool unpad(Bytes& data);
}