#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <iostream>
#include "utils.h" 
#include "native/HomComp.h"

using namespace std;

void testCompGE(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, double data2, int length) {
    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    int32_t plaintext2 = encodeDouble(length, data2);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
    LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    // Compare ciphertexts
    HomCompGE(answer, ciphertext1, ciphertext2, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, 1, key);
    int result = plain_answer[0]; // Result should be 0 or 1

    cout << "Comparing " << data1 << " >= " << data2 << ": " << result << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
}

void testCompLE(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, double data2, int length) {
    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    int32_t plaintext2 = encodeDouble(length, data2);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
    LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    // Compare ciphertexts
    HomCompLE(answer, ciphertext1, ciphertext2, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, 1, key);
    int result = plain_answer[0]; // Result should be 0 or 1

    cout << "Comparing " << data1 << " <= " << data2 << ": " << result << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
}

void testCompG(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, double data2, int length) {
    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    int32_t plaintext2 = encodeDouble(length, data2);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
    LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    // Compare ciphertexts
    HomCompG(answer, ciphertext1, ciphertext2, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, 1, key);
    int result = plain_answer[0]; // Result should be 0 or 1

    cout << "Comparing " << data1 << " > " << data2 << ": " << result << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
}

void testCompL(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, double data2, int length) {
    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    int32_t plaintext2 = encodeDouble(length, data2);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
    LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    // Compare ciphertexts
    HomCompL(answer, ciphertext1, ciphertext2, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, 1, key);
    int result = plain_answer[0]; // Result should be 0 or 1

    cout << "Comparing " << data1 << " < " << data2 << ": " << result << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
}

void testCompEQ(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data1, double data2, int length) {
    // Encode and encrypt data
    int32_t plaintext1 = encodeDouble(length, data1);
    int32_t plaintext2 = encodeDouble(length, data2);
    LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
    LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

    // Prepare result ciphertext
    LweSample* answer = new_gate_bootstrapping_ciphertext_array(1, params);

    // Compare ciphertexts
    HomEqui(answer, ciphertext1, ciphertext2, length, &key->cloud);

    // Decrypt and rebuild plaintext answer
    std::vector<int> plain_answer = decryptToBinaryVector(answer, 1, key);
    int result = plain_answer[0]; // Result should be 0 or 1

    cout << "Comparing " << data1 << " == " << data2 << ": " << result << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(1, answer);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
}


int main() {
    cout << "Test: CompGE Correctness" << endl;

    auto params = initializeParams(128);
    auto key = generateKeySet(params);
    int length = 16; // Bit length for encoding

    // Define test cases: {data1, data2}
    vector<pair<double, double>> testCases = {
	{1.1, 1.1},    // Equal Case where data 1 = data 2
        {1.2, 2.3},    // Positive-Positive where data1 < data2
        {2.3, 1.2},    // Positive-Positive where data1 > data2
        {1.2, -2.3},   // Positive-Negative
        {-2.3, 1.2},   // Negative-Positive
        {-1.2, -2.3},  // Negative-Negative where data1 > data2
        {-2.3, -1.2}   // Negative-Negative where data1 < data2
    };

    // Run test cases
    for (const auto& testCase : testCases) {
        testCompGE(params, key, testCase.first, testCase.second, length);
    }


    cout << "Test: CompLE Correctness" << endl;

    // Run test cases
    for (const auto& testCase : testCases) {
        testCompLE(params, key, testCase.first, testCase.second, length);
    }

    cout << "Test: CompG Correctness" << endl;

    // Run test cases
    for (const auto& testCase : testCases) {
        testCompG(params, key, testCase.first, testCase.second, length);
    }

    cout << "Test: CompL Correctness" << endl;

    // Run test cases
    for (const auto& testCase : testCases) {
        testCompL(params, key, testCase.first, testCase.second, length);
    }

    cout << "Test: HomEQ Correctness" << endl;

    // Run test cases
    for (const auto& testCase : testCases) {
        testCompEQ(params, key, testCase.first, testCase.second, length);
    }

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

