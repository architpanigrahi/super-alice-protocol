//
// Created by Kaaviya Ramkumar on 14/11/24.
//

#include "alice/encryption_manager.hpp"

namespace alice
{
    EncryptionManager::EncryptionManager()
    {
        key.resize(CHACHA20_KEY_SIZE);
        // // Generate a secure key for an instance of EncryptionManager
        // RAND_bytes(key.data(), CHACHA20_KEY_SIZE);
        key = {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
            0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
            0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20};
    }

    std::vector<uint8_t> EncryptionManager::encrypt(const std::vector<uint8_t> &buffer) const
    {
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            Logger::log(LogLevel::ERROR, "Failed to create encryption context");
            throw std::runtime_error("Failed to create encryption context");
        }

        std::vector<uint8_t> nonce(CHACHA20_NONCE_SIZE);
        if (RAND_bytes(nonce.data(), CHACHA20_NONCE_SIZE) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed to generate random nonce");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to generate random nonce");
        }

        std::vector<uint8_t> ciphertext(buffer.size() + CHACHA20_NONCE_SIZE);

        int len = 0;
        int ciphertext_len = 0;

        if (EVP_EncryptInit_ex(ctx, EVP_chacha20(), nullptr, key.data(), nonce.data()) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed to initialize encryption");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed to initialize encryption");
        }

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, buffer.data(), buffer.size()) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed during update encryption");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed during update encryption");
        }
        ciphertext_len = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed during final encryption");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed during final encryption");
        }
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        ciphertext.insert(ciphertext.begin(), nonce.begin(), nonce.end());
        ciphertext.resize(ciphertext_len + CHACHA20_NONCE_SIZE);

        return ciphertext;
    }

    std::vector<uint8_t> EncryptionManager::decrypt(const std::vector<uint8_t> &encrypted_buffer) const
    {
        if (encrypted_buffer.size() < CHACHA20_NONCE_SIZE)
        {
            Logger::log(LogLevel::ERROR, "Invalid ciphertext size");
            throw std::runtime_error("Invalid ciphertext size");
        }

        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            Logger::log(LogLevel::ERROR, "Failed to create encryption context");
            throw std::runtime_error("Failed to create encryption context");
        }

        std::vector<uint8_t> nonce(encrypted_buffer.begin(), encrypted_buffer.begin() + CHACHA20_NONCE_SIZE);
        std::vector<uint8_t> ciphertext(encrypted_buffer.begin() + CHACHA20_NONCE_SIZE, encrypted_buffer.end());
        std::vector<uint8_t> decrypted_data(ciphertext.size());

        int len;
        int decrypted_len;

        EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, key.data(), nonce.data());
        if (EVP_DecryptUpdate(ctx, decrypted_data.data(), &len, ciphertext.data(), ciphertext.size()) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed during update decryption");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed during update decryption");
        }
        decrypted_len = len;

        if (EVP_DecryptFinal_ex(ctx, decrypted_data.data() + len, &len) != 1)
        {
            Logger::log(LogLevel::ERROR, "Failed during final decryption");
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Failed during final decryption");
        }
        decrypted_len += len;

        decrypted_data.resize(decrypted_len);
        EVP_CIPHER_CTX_free(ctx);

        return decrypted_data;
    }
};