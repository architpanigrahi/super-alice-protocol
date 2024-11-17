// test/test_encryption_manager.cpp

#include <boost/test/unit_test.hpp>
#include <alice/encryption_manager.hpp>
#include <vector>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(encryption_manager_encrypt_decrypt)
{
    alice::EncryptionManager encryptor;

    std::vector<uint8_t> original_data = {10, 20, 30, 40, 50};

    std::vector<uint8_t> encrypted_data = encryptor.encrypt(original_data);

    // Ensure the encrypted data is not the same as the original (basic check)
    BOOST_CHECK(encrypted_data != original_data);

    std::vector<uint8_t> decrypted_data = encryptor.decrypt(encrypted_data);

    BOOST_CHECK(decrypted_data == original_data);
}

BOOST_AUTO_TEST_CASE(encryption_manager_different_ciphertexts)
{
    alice::EncryptionManager encryptor;

    std::vector<uint8_t> original_data = {10, 20, 30, 40, 50};

    std::vector<uint8_t> encrypted_data1 = encryptor.encrypt(original_data);
    std::vector<uint8_t> encrypted_data2 = encryptor.encrypt(original_data);

    // Verify that the two ciphertexts are different due to unique nonces
    BOOST_CHECK(encrypted_data1 != encrypted_data2);

    std::vector<uint8_t> decrypted_data1 = encryptor.decrypt(encrypted_data1);
    std::vector<uint8_t> decrypted_data2 = encryptor.decrypt(encrypted_data2);

    BOOST_CHECK(decrypted_data1 == original_data);
    BOOST_CHECK(decrypted_data2 == original_data);
}

BOOST_AUTO_TEST_CASE(encryption_manager_invalid_decryption)
{
    alice::EncryptionManager encryptor;

    std::vector<uint8_t> original_data = {10, 20, 30, 40, 50};
    std::vector<uint8_t> encrypted_data = encryptor.encrypt(original_data);

    // Modify the encrypted data by flipping bits to simulate corruption
    encrypted_data[0] ^= 0xFF;

    BOOST_CHECK(encryptor.decrypt(encrypted_data) != original_data);
}
