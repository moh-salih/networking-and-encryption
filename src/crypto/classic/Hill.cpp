#include <stdexcept>

#include "crypto/classic/Hill.h"

namespace crypto::classic { 
    
    Hill::Hill(const std::string& keyMatrixStr, int matrixSize): m_matrixSize(matrixSize) {

        if (keyMatrixStr.size() < static_cast<size_t>(matrixSize * matrixSize)) {
            throw std::invalid_argument("Hill: key string too short for matrix size.");
        }

        m_keyMatrix.resize(matrixSize, std::vector<int>(matrixSize, 0));
        m_inverseKeyMatrix.resize(matrixSize, std::vector<int>(matrixSize, 0));

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                m_keyMatrix[i][j] = std::toupper(keyMatrixStr[i * matrixSize + j]) - 'A';
            }
        }

        int det = determinant(m_keyMatrix);
        int detInv = modInverse(det, 26);
        auto adj = adjugate(m_keyMatrix);

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                m_inverseKeyMatrix[i][j] = mod26(adj[i][j] * detInv);
            }
        }
    }

    std::string Hill::encrypt(const std::string& plaintext) const {
        std::string ciphertext;
        int n = m_matrixSize;
        for (size_t i = 0; i < plaintext.length(); i += n) {
            std::vector<int> block(n, 0);
            for (int j = 0; j < n; ++j) {
                if (i + j < plaintext.length()) {
                    block[j] = plaintext[i + j] - 'A';
                }
            }

            for (int row = 0; row < n; ++row) {
                int val = 0;
                for (int col = 0; col < n; ++col) {
                    val += m_keyMatrix[row][col] * block[col];
                }
                ciphertext += static_cast<char>(mod26(val) + 'A');
            }
        }
        return ciphertext;
    }

    std::string Hill::decrypt(const std::string& ciphertext) const {
        std::string plaintext;
        int n = m_matrixSize;
        for (size_t i = 0; i < ciphertext.length(); i += n) {
            std::vector<int> block(n, 0);
            for (int j = 0; j < n; ++j) {
                if (i + j < ciphertext.length()) {
                    block[j] = ciphertext[i + j] - 'A';
                }
            }

            for (int row = 0; row < n; ++row) {
                int val = 0;
                for (int col = 0; col < n; ++col) {
                    val += m_inverseKeyMatrix[row][col] * block[col];
                }
                plaintext += static_cast<char>(mod26(val) + 'A');
            }
        }
        return plaintext;
    }

    int Hill::mod26(int x) const {
        return (x % 26 + 26) % 26;
    }

    int Hill::determinant(const std::vector<std::vector<int>>& matrix) const {
        int n = matrix.size();
        if (n == 1) return matrix[0][0];
        if (n == 2) return mod26(matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]);

        int det = 0;
        for (int p = 0; p < n; ++p) {
            std::vector<std::vector<int>> subMatrix(n - 1, std::vector<int>(n - 1));
            for (int i = 1; i < n; ++i) {
                int colIndex = 0;
                for (int j = 0; j < n; ++j) {
                    if (j == p) continue;
                    subMatrix[i - 1][colIndex++] = matrix[i][j];
                }
            }
            det += (p % 2 == 0 ? 1 : -1) * matrix[0][p] * determinant(subMatrix);
            det = mod26(det);
        }
        return mod26(det);
    }

    std::vector<std::vector<int>> Hill::adjugate(const std::vector<std::vector<int>>& matrix) const {
        int n = matrix.size();
        std::vector<std::vector<int>> adj(n, std::vector<int>(n, 0));

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::vector<std::vector<int>> subMatrix(n - 1, std::vector<int>(n - 1));
                int rowIndex = 0;
                for (int row = 0; row < n; ++row) {
                    if (row == i) continue;
                    int colIndex = 0;
                    for (int col = 0; col < n; ++col) {
                        if (col == j) continue;
                        subMatrix[rowIndex][colIndex++] = matrix[row][col];
                    }
                    rowIndex++;
                }
                int sign = ((i + j) % 2 == 0) ? 1 : -1;
                adj[j][i] = mod26(sign * determinant(subMatrix)); // Note the transpose here
            }
        }
        return adj;
    }

    int Hill::modInverse(int a, int m) const {
        a = a % m;
        for (int x = 1; x < m; ++x) {
            if ((a * x) % m == 1) {
                return x;
            }
        }
        throw std::runtime_error("Modular inverse does not exist.");
    }

} // namespace crypto::classic