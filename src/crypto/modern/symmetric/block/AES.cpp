#include <stdexcept>
#include <cstring>
#include <numeric>
#include <algorithm>

#include "crypto/modern/symmetric/block/AES.h"
#include "crypto/core/utils.h"


namespace {
    const uint8_t S_BOX[256] = {
        //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
    };

    const uint8_t INV_S_BOX[256] = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
        0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
        0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
        0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
        0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
        0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
        0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
        0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
        0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
        0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
        0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
        0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
        0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
        0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
        0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d 
    };

    const uint8_t RCON[11] = { 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36  };
     

    inline uint8_t xtime(uint8_t x) { return (x << 1) ^ (((x >> 7) & 1) * 0x1b); }

    uint8_t multiply(uint8_t x, uint8_t y) {
        uint8_t res = 0;
        for (int i = 0; i < 8; i++) {
            if ((y >> i) & 1) res ^= x;
            x = xtime(x);
        }
        return res;
    }

    void secure_zeroize(void* buffer, size_t size) {
        volatile uint8_t* p = static_cast<volatile uint8_t*>(buffer);
        for (size_t i = 0; i < size; ++i) {
            p[i] = 0;
        }
    }

    static uint32_t get_word(const std::array<uint8_t, crypto::modern::block::symmetric::MAX_AES_SCHEDULE_SIZE>& key_schedule, size_t index) {
        size_t start = index * 4;
        return (static_cast<uint32_t>(key_schedule[start]) << 24) |
            (static_cast<uint32_t>(key_schedule[start + 1]) << 16) |
            (static_cast<uint32_t>(key_schedule[start + 2]) << 8) |
            static_cast<uint32_t>(key_schedule[start + 3]);
    }
    
    static void set_word(std::array<uint8_t, crypto::modern::block::symmetric::MAX_AES_SCHEDULE_SIZE>& key_schedule, size_t index, uint32_t word) {
        size_t start = index * 4;
        key_schedule[start]     = static_cast<uint8_t>((word >> 24) & 0xFF);
        key_schedule[start + 1] = static_cast<uint8_t>((word >> 16) & 0xFF);
        key_schedule[start + 2] = static_cast<uint8_t>((word >> 8) & 0xFF);
        key_schedule[start + 3] = static_cast<uint8_t>(word & 0xFF);
    }

    static uint32_t sub_word(uint32_t word) {
        return (static_cast<uint32_t>(S_BOX[(word >> 24) & 0xFF]) << 24) |
            (static_cast<uint32_t>(S_BOX[(word >> 16) & 0xFF]) << 16) |
            (static_cast<uint32_t>(S_BOX[(word >> 8) & 0xFF]) << 8) |
            static_cast<uint32_t>(S_BOX[word & 0xFF]);
    }
}


namespace crypto::modern::block::symmetric {

    

    AES::AES(size_t keySizeBytes): m_keySizeBytes(keySizeBytes){
        if (keySizeBytes != 16 && keySizeBytes != 24 && keySizeBytes != 32) {
            throw std::invalid_argument("Invalid AES key size");
        }
    }

    AES::~AES() {
        secure_zeroize(m_roundKeys.data(), m_roundKeys.size());
    }

    void AES::setKey(const Bytes& key) {
        if (key.size() != m_keySizeBytes) {
            throw std::invalid_argument("Key size does not match the initialized size");
        }
        keyExpansion(key);
    }

    void AES::encryptBlock(const Bytes& plaintext, Bytes& output) const {
        if (plaintext.size() != 16) throw std::invalid_argument("Block size error");
        output.resize(16);

        State state;
        // 1. Copy plaintext into State (Column-major order)
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                state[j][i] = plaintext[i * 4 + j];

        int Nr = (m_keySizeBytes / 4) + 6; 

        // Initial Round
        addRoundKey(state, 0);

        // Main Rounds
        for (int round = 1; round < Nr; round++) {
            subBytes(state);
            shiftRows(state);
            mixColumns(state); 
            addRoundKey(state, round);
        }

        // Final Round (No MixColumns)
        subBytes(state);
        shiftRows(state);
        addRoundKey(state, Nr);

        // Copy State back to Output
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                output[i * 4 + j] = state[j][i];
    }


    void AES::decryptBlock(const Bytes& ciphertext, Bytes& output) const {
        if (ciphertext.size() != 16) throw std::invalid_argument("Block size error");
        output.resize(16);

        State state;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                state[j][i] = ciphertext[i * 4 + j];

        int Nr = (m_keySizeBytes / 4) + 6;

        // Initial Round
        addRoundKey(state, Nr);

        // Main Rounds
        for (int round = Nr - 1; round > 0; round--) {
            invShiftRows(state);
            invSubBytes(state);
            addRoundKey(state, round);
            invMixColumns(state);
        }

        // Final Round
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, 0);

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                output[i * 4 + j] = state[j][i];
    }

    size_t AES::blockSize() const noexcept {
        return 16; // AES block size is always 16 bytes
    }

    size_t AES::keySize() const noexcept {
        return m_keySizeBytes;
    }

    void AES::keyExpansion(const Bytes& key) {
        secure_zeroize(m_roundKeys.data(), m_roundKeys.size());

        const size_t Nk = m_keySizeBytes / 4;
        const size_t Nb = 4;
        size_t Nr; 
        if (Nk == 4) Nr = 10; else if (Nk == 6) Nr = 12; else Nr = 14;

        const size_t total_words = Nb * (Nr + 1);
        
        // 1. Initialize the first Nk words
        for (size_t i = 0; i < Nk; ++i) {
            // Correct way to load the initial key using the helper function
            uint32_t temp_word = 
                (static_cast<uint32_t>(key[i*4]) << 24) |
                (static_cast<uint32_t>(key[i*4 + 1]) << 16) |
                (static_cast<uint32_t>(key[i*4 + 2]) << 8) |
                static_cast<uint32_t>(key[i*4 + 3]);
            set_word(m_roundKeys, i, temp_word);
        }

        // 2. Main expansion loop
        uint32_t temp;
        for (size_t i = Nk; i < total_words; ++i) {
            temp = get_word(m_roundKeys, i - 1);

            if (i % Nk == 0) {
                // RotWord and SubWord (RotWord is handled inside sub_word in the standard)
                // But your sub_word only does substitution. Let's fix the logic:
                
                // Step 1: RotWord (Rotate left by 1 byte)
                temp = (temp << 8) | (temp >> 24);
                
                // Step 2: SubWord (Substitute each byte)
                temp = sub_word(temp);
                
                // Step 3: XOR with Rcon (i/Nk is the round index, starting at 1)
                temp ^= (static_cast<uint32_t>(RCON[i / Nk]) << 24);
                
            } else if (Nk > 6 && i % Nk == 4) { // Only for AES-256
                // Step 1: SubWord (No rotation here)
                temp = sub_word(temp);
            }
            
            // XOR with the word Nk positions before the current word
            set_word(m_roundKeys, i, get_word(m_roundKeys, i - Nk) ^ temp);
        }
    }

    void AES::subBytes(State& state) const {
        for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    state[i][j] = S_BOX[state[i][j]];
        }

    void AES::shiftRows(State& state) const {
        uint8_t temp;

        // Row 1: Shift left by 1
        temp = state[1][0];
        state[1][0] = state[1][1];
        state[1][1] = state[1][2];
        state[1][2] = state[1][3];
        state[1][3] = temp;

        // Row 2: Shift left by 2
        temp = state[2][0];
        uint8_t temp2 = state[2][1];
        state[2][0] = state[2][2];
        state[2][1] = state[2][3];
        state[2][2] = temp;
        state[2][3] = temp2;

        // Row 3: Shift left by 3 (or right by 1)
        temp = state[3][3];
        state[3][3] = state[3][2];
        state[3][2] = state[3][1];
        state[3][1] = state[3][0];
        state[3][0] = temp;
    }

    void AES::addRoundKey(State& state, size_t round) const {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // Pull from the expanded m_roundKeys array
                // Round keys are stored as words (4 bytes)
                state[j][i] ^= m_roundKeys[round * 16 + i * 4 + j];
            }
        }
    }

    void AES::mixColumns(State& state) const {
        uint8_t tmp[4];
        for (int i = 0; i < 4; i++) {
            tmp[0] = xtime(state[0][i]) ^ (xtime(state[1][i]) ^ state[1][i]) ^ state[2][i] ^ state[3][i];
            tmp[1] = state[0][i] ^ xtime(state[1][i]) ^ (xtime(state[2][i]) ^ state[2][i]) ^ state[3][i];
            tmp[2] = state[0][i] ^ state[1][i] ^ xtime(state[2][i]) ^ (xtime(state[3][i]) ^ state[3][i]);
            tmp[3] = (xtime(state[0][i]) ^ state[0][i]) ^ state[1][i] ^ state[2][i] ^ xtime(state[3][i]);

            state[0][i] = tmp[0];
            state[1][i] = tmp[1];
            state[2][i] = tmp[2];
            state[3][i] = tmp[3];
        }
    }



    void AES::invSubBytes(State& state) const {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                state[i][j] = INV_S_BOX[state[i][j]];
    }

    void AES::invShiftRows(State& state) const {
        uint8_t temp;
        // Row 1: Shift right by 1
        temp = state[1][3];
        state[1][3] = state[1][2];
        state[1][2] = state[1][1];
        state[1][1] = state[1][0];
        state[1][0] = temp;

        // Row 2: Shift right by 2
        temp = state[2][3];
        uint8_t temp2 = state[2][2];
        state[2][3] = state[2][1];
        state[2][2] = state[2][0];
        state[2][1] = temp;
        state[2][0] = temp2;

        // Row 3: Shift right by 3
        temp = state[3][0];
        state[3][0] = state[3][1];
        state[3][1] = state[3][2];
        state[3][2] = state[3][3];
        state[3][3] = temp;
    }

    void AES::invMixColumns(State& state) const {
        uint8_t tmp[4];
        for (int i = 0; i < 4; i++) {
            uint8_t a = state[0][i], b = state[1][i], c = state[2][i], d = state[3][i];
            state[0][i] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
            state[1][i] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
            state[2][i] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
            state[3][i] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
        }
    }
} // namespace crypto::modern::symmetric