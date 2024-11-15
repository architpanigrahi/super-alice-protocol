//
// Created by Kaaviya Ramkumar on 14/11/24.
//

#ifndef ENCRYPTION_MANAGER_H
#define ENCRYPTION_MANAGER_H

#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "logger.hpp"

namespace alice {
    class EncryptionManager {
    public:
        static const int CHACHA20_KEY_SIZE = 32;
        static const int CHACHA20_NONCE_SIZE = 12;

        EncryptionManager();

        std::vector<uint8_t> encrypt(const std::vector<uint8_t>& buffer) const;
        std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encrypted_buffer) const;

    private:
        std::vector<uint8_t> key;
    };
};


#endif //ENCRYPTION_MANAGER_H
