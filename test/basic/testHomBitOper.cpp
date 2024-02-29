#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <vector>
#include <iostream>
#include "utils.h" 
#include "native/HomBitOper.h"

using namespace std;

// Function to test HomLShift
void testHomLShift(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data, int shiftAmount, int length) {
    // Encode and encrypt data
    int32_t plaintext = encodeDouble(length, data);
    LweSample* ciphertext = encryptBoolean(plaintext, length, params, key);

    // Prepare result ciphertext
    LweSample* result = new_gate_bootstrapping_ciphertext_array(length, params);

    // Apply HomLShift
    HomLShift(result, ciphertext, length, shiftAmount, &key->cloud);

    // Decrypt and display result
    double shiftedData = decodeDouble(decryptToBinaryVector(result, length, key));
    cout << "Left Shift " << data << " by " << shiftAmount << ": " << shiftedData << endl;

    // Clean up
    delete_gate_bootstrapping_ciphertext_array(length, ciphertext);
    delete_gate_bootstrapping_ciphertext_array(length, result);
}

// Function to test HomRShift
void testHomRShift(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data, int shiftAmount, int length) {
    int32_t plaintext = encodeDouble(length, data);
    LweSample* ciphertext = encryptBoolean(plaintext, length, params, key);
    LweSample* result = new_gate_bootstrapping_ciphertext_array(length, params);

    HomRShift(result, ciphertext, length, shiftAmount, &key->cloud);
    double shiftedData = decodeDouble(decryptToBinaryVector(result, length, key));
    cout << "Right Shift " << data << " by " << shiftAmount << ": " << shiftedData << endl;

    delete_gate_bootstrapping_ciphertext_array(length, ciphertext);
    delete_gate_bootstrapping_ciphertext_array(length, result);
}

// Function to test HomTwosComplement
void testHomTwosComplement(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data, int length) {
    int32_t plaintext = encodeDouble(length, data);
    LweSample* ciphertext = encryptBoolean(plaintext, length, params, key);
    LweSample* result = new_gate_bootstrapping_ciphertext_array(length, params);

    HomTwosComplement(result, ciphertext, length, &key->cloud);
    double negatedData = decodeDouble(decryptToBinaryVector(result, length, key));
    cout << "Two's Complement of " << data << ": " << negatedData << endl;

    delete_gate_bootstrapping_ciphertext_array(length, ciphertext);
    delete_gate_bootstrapping_ciphertext_array(length, result);
}

// Function to test HomAbs
void testHomAbs(const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key, double data, int length) {
    int32_t plaintext = encodeDouble(length, data);
    LweSample* ciphertext = encryptBoolean(plaintext, length, params, key);
    LweSample* result = new_gate_bootstrapping_ciphertext_array(length, params);

    HomAbs(result, ciphertext, length, &key->cloud);
    double absoluteData = decodeDouble(decryptToBinaryVector(result, length, key));
    cout << "Absolute value of " << data << ": " << absoluteData << endl;

    delete_gate_bootstrapping_ciphertext_array(length, ciphertext);
    delete_gate_bootstrapping_ciphertext_array(length, result);
}

int main() {
    // Initialize parameters and key
    const auto params = initializeParams(128);
    const auto key = generateKeySet(params);
    const int length = 16; // Bit length for encoding

    // Define test data
    vector<double> testData = {-2.5, 0, 1.7};

    // Test HomLShift with shiftAmount of 1
    for (const auto& data : testData) {
        testHomRShift(params, key, data, 1, length);
    }
    // Test HomLShift with shiftAmount of 1
    for (const auto& data : testData) {
        testHomLShift(params, key, data, 1, length);
    }
   // Test HomTwosComplement
    for (const auto& data : testData) {
        testHomTwosComplement(params, key, data, length);
    }

    // Test HomAbs
    for (const auto& data : testData) {
        testHomAbs(params, key, data, length);
    }

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

