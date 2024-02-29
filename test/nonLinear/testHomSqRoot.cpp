#include <iostream>
#include <tfhe/tfhe.h>
#include "utils.h"
#include "native/HomNonLinearOper.h"
#include "native/HomArith.h"


void testSqRoot(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, int length) {

    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(length, params);

    // Compare ciphertexts
    HomSqRoot(answer, ciphertext1, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, length, key);
    double double_answer = decodeDouble(plain_answer);	

    std::cout << "(exact) Square Root of " << data1 << ": = " << sqrt(data1) << std::endl;
    std::cout << "(ours) Square Root of " << data1 << ": = " << double_answer << std::endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(length, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);

}


int main() {
    std::cout << "Test: SqRoot Correctness" << std::endl;

    auto params = initializeParams(128);
    auto key = generateKeySet(params);
    int length = 8; 

    // Define test cases
    std::vector<double> Cases = {2, 4, 6, 8, 1.5};

    // Run test cases
    for (const auto& Case : Cases) {
        testSqRoot(params, key, Case, length);
    }

    return 0;
}
