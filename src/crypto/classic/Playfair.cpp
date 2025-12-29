#include <cctype>
#include <algorithm>
#include <vector>
#include <utility>

#include "crypto/classic/Playfair.h"

namespace { // unnamed namespace = internal linkage for this translation unit only
    using Matrix = std::vector<std::vector<char>>;
    
    Matrix generateKeyMatrix(const std::string& key) {
        Matrix matrix(5, std::vector<char>(5, ' '));
        std::string adjustedKey;
        std::vector<bool> used(26, false);

        for (char ch : key) {
            if (std::isalpha(ch)) {
                ch = std::toupper(ch);
                if (ch == 'J') ch = 'I'; // Treat 'I' and 'J' as the same letter
                if (!used[ch - 'A']) {
                    adjustedKey += ch;
                    used[ch - 'A'] = true;
                }
            }
        }

        for (char ch = 'A'; ch <= 'Z'; ++ch) {
            if (ch == 'J') continue; // Skip 'J'
            if (!used[ch - 'A']) {
                adjustedKey += ch;
                used[ch - 'A'] = true;
            }
        }

        for (size_t i = 0; i < 25; ++i) {
            matrix[i / 5][i % 5] = adjustedKey[i];
        }

        return matrix;
    }

    std::pair<int, int> findPosition(const Matrix& matrix, char ch) {
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (matrix[i][j] == ch)
                    return {i, j};
            }
        }
        return {-1, -1}; // should never happen
    }

    std::string preprocessPlaintext(const std::string& plaintext) {
        std::string adjusted;
        for (char ch : plaintext) {
            if (std::isalpha(ch)) {
                ch = std::toupper(ch);
                if (ch == 'J') ch = 'I';
                adjusted += ch;
            }
        }

        std::string result;
        for (size_t i = 0; i < adjusted.length(); ++i) {
            result += adjusted[i];
            if (i + 1 < adjusted.length()) {
                if (adjusted[i] == adjusted[i + 1]) {
                    result += 'X'; // Insert 'X' between identical letters
                } else {
                    result += adjusted[i + 1];
                    ++i;
                }
            }
        }

        if (result.length() % 2 != 0)
            result += 'X'; // Padding if odd length

        return result;
    }

} // namespace


// ---------------------------------------------------------------------------
namespace crypto::classic { 
    
    std::string Playfair::encrypt(const std::string& plaintext) const {
        Matrix keyMatrix = generateKeyMatrix(m_key);
        std::string adjustedText = preprocessPlaintext(plaintext);
        std::string ciphertext;

        for (size_t i = 0; i < adjustedText.length(); i += 2) {
            char first = adjustedText[i];
            char second = adjustedText[i + 1];
            auto [row1, col1] = findPosition(keyMatrix, first);
            auto [row2, col2] = findPosition(keyMatrix, second);

            if (row1 == row2) { // Same row
                ciphertext += keyMatrix[row1][(col1 + 1) % 5];
                ciphertext += keyMatrix[row2][(col2 + 1) % 5];
            } else if (col1 == col2) { // Same column
                ciphertext += keyMatrix[(row1 + 1) % 5][col1];
                ciphertext += keyMatrix[(row2 + 1) % 5][col2];
            } else { // Rectangle swap
                ciphertext += keyMatrix[row1][col2];
                ciphertext += keyMatrix[row2][col1];
            }
        }

        return ciphertext;
    }
    
    std::string Playfair::decrypt(const std::string& ciphertext) const {
        Matrix keyMatrix = generateKeyMatrix(m_key);
        std::string plaintext;

        // Filter input: allow only A–Z, treat J as I
        std::string filtered;
        for (char ch : ciphertext) {
            if (std::isalpha(static_cast<unsigned char>(ch))) {
                ch = std::toupper(ch);
                if (ch == 'J') ch = 'I';
                filtered += ch;
            }
        }

        // Ensure even length — pad with 'X' if needed
        if (filtered.size() % 2 != 0) {
            filtered.push_back('X');
        }

        // Process pairs safely
        for (size_t i = 0; i + 1 < filtered.size(); i += 2) {
            char first = filtered[i];
            char second = filtered[i + 1];
            auto [row1, col1] = findPosition(keyMatrix, first);
            auto [row2, col2] = findPosition(keyMatrix, second);

            if (row1 == row2) { // Same row
                plaintext += keyMatrix[row1][(col1 + 4) % 5];
                plaintext += keyMatrix[row2][(col2 + 4) % 5];
            } else if (col1 == col2) { // Same column
                plaintext += keyMatrix[(row1 + 4) % 5][col1];
                plaintext += keyMatrix[(row2 + 4) % 5][col2];
            } else { // Rectangle swap
                plaintext += keyMatrix[row1][col2];
                plaintext += keyMatrix[row2][col1];
            }
        }

        // Optional cleanup: remove trailing X padding if it looks artificial
        if (!plaintext.empty() && plaintext.back() == 'X') {
            plaintext.pop_back();
        }

        return plaintext;
    }

} // namespace crypto::classic
