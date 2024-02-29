#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "tfhe/tfhe.h"
#include "utils.h"
#include "native/HomBitOper.h"

int main() {
    // Initialize parameters and keys
    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    // Define lengths for testing
    std::vector<int> lengths = {8, 16, 32};

    // Single test case for simplicity
    double input = -2.2;
    int shiftAmount = 1;  // Example shift amount for LShift and RShift

    // Homomorphic operations for shifting
    std::vector<std::function<void(LweSample*, const LweSample*, const int, const int, const TFheGateBootstrappingCloudKeySet*)>> shiftOperations = {
        HomLShift, HomRShift
    };

    std::vector<std::string> shiftOperationNames = {"HomLShift", "HomRShift"};

    // Header for shift operations
    std::cout << "Input: " << input << "\nSecurity Level: 128" << std::endl;
    std::cout << std::left << std::setw(12) << "Function";
    for (auto length : lengths) {
        std::cout << std::right << std::setw(6) << length << "-bit ";
    }
    std::cout << std::endl;

    // Testing shift operations
    for (size_t i = 0; i < shiftOperations.size(); ++i) {
        std::cout << std::left << std::setw(12) << shiftOperationNames[i];
        
        for (auto length : lengths) {
            int32_t plaintext = encodeDouble(length, input);
            LweSample* encInput = encryptBoolean(plaintext, length, params, key);

            auto start = std::chrono::high_resolution_clock::now();
            LweSample* resultEnc = new_gate_bootstrapping_ciphertext_array(length, params);
            shiftOperations[i](resultEnc, encInput, length, shiftAmount, &key->cloud);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            delete_gate_bootstrapping_ciphertext_array(length, resultEnc);
            delete_gate_bootstrapping_ciphertext_array(length, encInput);

            std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(6) << elapsed.count() << " ";
        }
        std::cout << std::endl;
    }

    // Cleanup keys and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

