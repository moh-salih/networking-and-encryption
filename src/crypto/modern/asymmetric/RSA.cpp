#include "RSA.h"
#include "crypto/core/utils.h"

#include <stdexcept>

using crypto::core::Bytes;
using crypto::core::asymmetric::KeyPair;
using crypto::core::utils::gcd;
using crypto::core::utils::modInverse;

namespace crypto::modern::asymmetric {

bool RSA::isPrime(uint64_t n) const {
    if (n < 2) return false;
    for (uint64_t i = 2; i * i <= n; ++i)
        if (n % i == 0) return false;
    return true;
}

uint64_t RSA::mulMod(uint64_t a, uint64_t b, uint64_t m) const {
    uint64_t res = 0;
    a %= m;
    while (b > 0) {
        if (b % 2 == 1) res = (res + a) % m;
        a = (a * 2) % m;
        b /= 2;
    }
    return res;
}

uint64_t RSA::modPow(uint64_t base, uint64_t exp, uint64_t mod) const {
    uint64_t result = 1;
    base %= mod;

    while (exp > 0) {
        if (exp & 1)
            result = mulMod(result, base, mod); // Safe multiplication
        base = mulMod(base, base, mod);         // Safe squaring
        exp >>= 1;
    }
    return result;
}



Bytes RSA::encodeUint64(uint64_t v) const {
    Bytes b(8);
    for (int i = 0; i < 8; ++i)
        b[7 - i] = static_cast<uint8_t>(v >> (i * 8));
    return b;
}


uint64_t RSA::decodeUint64(const Bytes& b, size_t offset) const {
    if (b.size() < offset + 8)
        throw std::invalid_argument("Invalid key format");

    uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v = (v << 8) | b[offset + i];
    return v;
}

KeyPair RSA::generateKeyPair(size_t /*bits*/) {
    // fixed small primes (educational)
    uint64_t p = 61;
    uint64_t q = 53;

    uint64_t n   = p * q;
    uint64_t phi = (p - 1) * (q - 1);

    uint64_t e = 17;
    if (gcd(e, phi) != 1)
        throw std::runtime_error("Invalid public exponent");

    uint64_t d = modInverse(e, phi);
    if (d == static_cast<uint64_t>(-1))
        throw std::runtime_error("No modular inverse");

    KeyPair kp;
    kp.public_key.insert(kp.public_key.end(), encodeUint64(n).begin(), encodeUint64(n).end());
    kp.public_key.insert(kp.public_key.end(), encodeUint64(e).begin(), encodeUint64(e).end());

    kp.private_key.insert(kp.private_key.end(), encodeUint64(n).begin(), encodeUint64(n).end());
    kp.private_key.insert(kp.private_key.end(), encodeUint64(d).begin(), encodeUint64(d).end());

    return kp;
}

Bytes RSA::encrypt(const Bytes& plaintext, const Bytes& public_key) {
    if (plaintext.size() != 1)
        throw std::invalid_argument("RSA demo supports 1-byte messages");

    uint64_t n = decodeUint64(public_key, 0);
    uint64_t e = decodeUint64(public_key, 8);

    uint64_t m = plaintext[0];
    if (m >= n)
        throw std::invalid_argument("Message too large");

    uint64_t c = modPow(m, e, n);
    return encodeUint64(c);
}

Bytes RSA::decrypt(const Bytes& ciphertext, const Bytes& private_key) {
    uint64_t n = decodeUint64(private_key, 0);
    uint64_t d = decodeUint64(private_key, 8);

    uint64_t c = decodeUint64(ciphertext, 0);
    uint64_t m = modPow(c, d, n);

    return Bytes{ static_cast<uint8_t>(m) };
}

}
