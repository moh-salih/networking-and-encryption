#pragma once
#include <string>
#include <vector>
#include "ICipher.h"

namespace crypto::classic {
    class Hill : public ICipher {
    public:
        Hill(const std::string& keyMatrixStr, int matrixSize);

        std::string encrypt(const std::string& plaintext) const override;
        std::string decrypt(const std::string& ciphertext) const override;  
        std::string name() const override { return "Hill Cipher"; }
    private:
        int mod26(int x) const;
        int determinant(const std::vector<std::vector<int>>& matrix) const;
        std::vector<std::vector<int>> adjugate(const std::vector<std::vector<int>>& matrix) const;
        int modInverse(int a, int m) const;

        std::vector<std::vector<int>> m_keyMatrix;
        std::vector<std::vector<int>> m_inverseKeyMatrix;
        int m_matrixSize;
    };
} // namespace crypto::classic