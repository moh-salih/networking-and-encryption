#include "crypto/core/utils.h"
#include <iomanip>
#include <sstream>

namespace crypto::core::utils {

    int gcd(int a, int b) {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    int modInverse(int a, int m) {
        a = a % m;
        for (int x = 1; x < m; x++) {
            if ((a * x) % m == 1) return x;
        }
        return -1;
    }

    std::string toHex(const Bytes& bytes) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (const auto& byte : bytes) {
            ss << std::setw(2) << static_cast<int>(byte);
        }
        return ss.str();
    }

    Bytes fromHex(const std::string& hex) {
        Bytes bytes;
        for (size_t i = 0; i < hex.length(); i += 2) {
            if (i + 1 >= hex.length()) break; 
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::strtol(byteString.c_str(), nullptr, 16));
            bytes.push_back(byte);
        }
        return bytes;
    }






    void pad(Bytes& data, size_t blockSize) {
        uint8_t paddingValue = static_cast<uint8_t>(blockSize - (data.size() % blockSize));
        for (size_t i = 0; i < paddingValue; ++i) {
            data.push_back(paddingValue);
        }
    }

    bool unpad(Bytes& data) {
        if (data.empty()) return false;
        uint8_t paddingValue = data.back();
        if (paddingValue == 0 || paddingValue > 16) return false; // Invalid padding
        
        // Check if all padding bytes are the same value
        for (size_t i = 0; i < paddingValue; ++i) {
            if (data[data.size() - 1 - i] != paddingValue) return false;
        }
        
        data.resize(data.size() - paddingValue);
        return true;
    }
} // namespace crypto::core::utils