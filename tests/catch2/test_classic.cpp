#include <catch2/catch_all.hpp>
#include <algorithm>
#include <string>
#include "crypto/classic/Caesar.h"
#include "crypto/classic/Vigenere.h"
#include "crypto/classic/Affine.h"
#include "crypto/classic/Playfair.h"
#include "crypto/classic/Hill.h"

using namespace crypto::classic;

// --- Helper Functions for Extensive Testing ---

// Checks if the cipher preserves non-alphabetic characters exactly
bool preserves_formatting(const ICipher& cipher, const std::string& input) {
    std::string encrypted = cipher.encrypt(input);
    if (input.length() != encrypted.length()) return false;
    for (size_t i = 0; i < input.length(); ++i) {
        if (!std::isalpha(input[i]) && input[i] != encrypted[i]) return false;
    }
    return true;
}

// Checks if changing case in plaintext results in the same case change in ciphertext
bool maintains_case_logic(const ICipher& cipher, char lower, char upper) {
    std::string l_str(1, lower);
    std::string u_str(1, upper);
    std::string enc_l = cipher.encrypt(l_str);
    std::string enc_u = cipher.encrypt(u_str);
    return std::islower(enc_l[0]) && std::isupper(enc_u[0]) && 
           std::tolower(enc_u[0]) == enc_l[0];
}

// ============================================================
// CAESAR CIPHER: EXTENSIVE TESTS
// ============================================================
TEST_CASE("Caesar Cipher: Mathematical and Boundary Analysis", "[classic][caesar]") {
    
    SECTION("Identity Property (Shift 0 or 26)") {
        Caesar c0(0);
        Caesar c26(26);
        std::string target = "CryptoAnalysis123!";
        REQUIRE(c0.encrypt(target) == target);
        REQUIRE(c26.encrypt(target) == target);
    }

    SECTION("Large Modular Shifts") {
        // Shift 53 is (2 * 26) + 1, which is effectively shift 1
        Caesar c53(53);
        REQUIRE(c53.encrypt("A") == "B");
    }

    SECTION("Negative Shift Handling") {
        // Shift -1 should behave like Shift 25
        Caesar cNeg(-1);
        REQUIRE(cNeg.encrypt("A") == "Z");
        REQUIRE(cNeg.decrypt("Z") == "A");
    }

    SECTION("Full Alphabet Rotation (Exhaustive)") {
        for (int s = 0; s < 26; ++s) {
            Caesar c(s);
            std::string p = "The Quick Brown Fox Jumps Over The Lazy Dog";
            REQUIRE(c.decrypt(c.encrypt(p)) == p);
        }
    }

    SECTION("Format and Case Integrity") {
        Caesar c(13); // ROT13
        REQUIRE(preserves_formatting(c, "Testing 1... 2... 3... !@#"));
        REQUIRE(maintains_case_logic(c, 'a', 'A'));
    }
}

// ============================================================
// VIGENERE CIPHER: EXTENSIVE TESTS
// ============================================================
TEST_CASE("Vigenere Cipher: Key and Index Synchronization", "[classic][vigenere]") {
    
    SECTION("Single Letter Key (Behaves like Caesar)") {
        Vigenere v("D"); // Shift 3
        REQUIRE(v.encrypt("ABC") == "DEF");
    }

    SECTION("Key Longer than Plaintext") {
        Vigenere v("LONGKEYTHATISWAYTOOLONG");
        REQUIRE(v.decrypt(v.encrypt("Short")) == "Short");
    }

    SECTION("Key with Non-Alphabetic Chars (Resilience)") {
        // Key should normalize to uppercase alpha
        Vigenere v("k e y 123 !");
        REQUIRE(v.encrypt("A") == "K"); 
    }

    SECTION("Key Index Persistence (Punctuation Skipping)") {
        Vigenere v("KEY"); // K=10, E=4, Y=24
        // 'A' uses K -> K
        // ' ' skips
        // 'A' uses E -> E
        REQUIRE(v.encrypt("A A") == "K E");
    }

    SECTION("Empty Plaintext Handling") {
        Vigenere v("KEY");
        REQUIRE(v.encrypt("").empty());
    }
}

// ============================================================
// AFFINE CIPHER: MATHEMATICAL VALIDATION
// ============================================================
TEST_CASE("Affine Cipher: Invertibility and GCD", "[classic][affine]") {
    
    SECTION("Standard Valid Keys") {
        Affine a(3, 5); // 3x + 5 mod 26
        // 'A' (0) -> (3*0 + 5) = 5 ('F')
        // 'B' (1) -> (3*1 + 5) = 8 ('I')
        REQUIRE(a.encrypt("AB") == "FI");
        REQUIRE(a.decrypt("FI") == "AB");
    }

    SECTION("Weak Key: a=1 (Behaves like Caesar)") {
        Affine a(1, 3);
        Caesar c(3);
        REQUIRE(a.encrypt("HELLO") == c.encrypt("HELLO"));
    }

    SECTION("Invalid Key Detection (Professor Requirements)") {
        // 'a' must be coprime to 26 (factors are 2, 13)
        REQUIRE_THROWS_AS(Affine(2, 1), std::invalid_argument);
        REQUIRE_THROWS_AS(Affine(13, 1), std::invalid_argument);
        REQUIRE_THROWS_AS(Affine(26, 1), std::invalid_argument);
        REQUIRE_THROWS_AS(Affine(0, 1), std::invalid_argument);
    }

    SECTION("Exhaustive Coprime Roundtrip") {
        int coprimes[] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
        for (int a : coprimes) {
            Affine aff(a, 10);
            std::string p = "AffineMathIsFun";
            REQUIRE(aff.decrypt(aff.encrypt(p)) == p);
        }
    }
}

// ============================================================
// PLAYFAIR CIPHER: DIGRAPH LOGIC
// ============================================================
TEST_CASE("Playfair Cipher: Digraph and Digram Rules", "[classic][playfair]") {
    
    SECTION("Standard Digraph Rules") {
        Playfair p("KEYWORD");
        /* Matrix:
           K E Y W O
           R D A B C
           F G H I L
           M N P Q S
           T U V X Z  */
        
        // Same Row: 'K', 'E' -> 'E', 'Y'
        REQUIRE(p.encrypt("KE") == "EY");
        // Same Column: 'K', 'R' -> 'R', 'F'
        REQUIRE(p.encrypt("KR") == "RF");
        // Rectangle: 'K', 'D' -> 'E', 'R'
        REQUIRE(p.encrypt("KD") == "ER");
    }

    SECTION("The 'J' Problem") {
        Playfair p("JUNGLE"); // J becomes I
        std::string enc = p.encrypt("JACK");
        std::string dec = p.decrypt(enc);
        // Standard Playfair: Decrypting 'I' usually stays 'I'
        REQUIRE(dec.find('I') != std::string::npos);
    }

    SECTION("Duplicate Letters in Digraph (Padding)") {
        Playfair p("KEY");
        // "BALLOON" -> "BA" "LX" "LO" "ON"
        std::string enc = p.encrypt("BALLOON");
        REQUIRE(enc.length() == 8); 
    }

    SECTION("Odd Length Message") {
        Playfair p("KEY");
        // "HELLO" -> "HE" "LL" "O" -> "HE" "LX" "OX"
        REQUIRE(p.encrypt("HELLO").length() == 6);
    }
}

// ============================================================
// HILL CIPHER: MATRIX ALGEBRA
// ============================================================
TEST_CASE("Hill Cipher: Matrix Dimensions and Inverses", "[classic][hill]") {
    
    SECTION("2x2 Basic Test") {
        // Key "DDCF" -> [[3, 3], [2, 5]]
        // Det = 3*5 - 3*2 = 15 - 6 = 9 (Coprime to 26)
        Hill h("DDCF", 2);
        std::string p = "HELP";
        std::string enc = h.encrypt(p);
        REQUIRE(enc != p);
        REQUIRE(h.decrypt(enc) == p);
    }

    SECTION("3x3 textbook Example") {
        Hill h("GYBNQKURP", 3);
        REQUIRE(h.encrypt("ACT") == "POH");
        REQUIRE(h.decrypt("POH") == "ACT");
    }

    SECTION("Non-Invertible Matrix (Exception Handling)") {
        // Matrix [[1, 1], [1, 1]] Det = 0
        REQUIRE_THROWS(Hill("AAAA", 2));
        
        // Matrix [[2, 2], [2, 2]] Det = 0
        REQUIRE_THROWS(Hill("CCCC", 2));
    }

    SECTION("Matrix Size Mismatch") {
        // Provided 4 chars for 3x3 (needs 9)
        REQUIRE_THROWS_AS(Hill("ABCD", 3), std::invalid_argument);
    }

    SECTION("Large Data Buffer (Throughput check)") {
        Hill h("GYBNQKURP", 3);
        std::string large(999, 'A');
        REQUIRE(h.decrypt(h.encrypt(large)) == large);
    }
}

// ============================================================
// CROSS-CIPHER INTEGRITY
// ============================================================
TEST_CASE("Classic Ciphers: General Robustness", "[classic][robustness]") {
    std::vector<std::unique_ptr<ICipher>> ciphers;
    ciphers.push_back(std::make_unique<Caesar>(7));
    ciphers.push_back(std::make_unique<Vigenere>("SECRET"));
    ciphers.push_back(std::make_unique<Affine>(7, 10));

    std::string testTexts[] = {
        "Short",
        "A",
        "The quick brown fox jumps over the lazy dog.",
        "NUMBERS 1234567890",
        "!@#$%^&*()_+~",
        "Multiple\nLines\nWith\tTabs"
    };

    for (const auto& cipher : ciphers) {
        for (const auto& text : testTexts) {
            DYNAMIC_SECTION("Cipher: " << cipher->name() << " Text: " << text) {
                std::string encrypted = cipher->encrypt(text);
                REQUIRE(cipher->decrypt(encrypted) == text);
            }
        }
    }
}