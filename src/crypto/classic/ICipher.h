#pragma once
#include <string>

class ICipher {
public:
    virtual ~ICipher() = default;

    virtual std::string encrypt(const std::string& plaintext) const = 0;
    virtual std::string decrypt(const std::string& ciphertext) const = 0;

    virtual std::string name() const = 0;
};
