#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "tfhe/tfhe.h"
#include "utils.h"
#include "native/HomBitOper.h"

int main() {
    // Initialize parameters and keys for the second part
    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    // Define lengths for testing
    std::vector<int> lengths = {8, 16, 32};

    // Single test case for simplicity
    double input = -2.2;

    // Homomorphic operations for TwosComplement and Abs
    std::vector<std::function<void(LweSample*, const LweSample*, const int, const TFheGateBootstrappingCloudKeySet*)>> otherOperations = {
        HomTwosComplement, HomAbs
    };

    std::vector<std::string> otherOperationNames = {"HomTwosComp", "HomAbs"};

    // Header for other operations
    std::cout << "\nInput: " << input << "\nSecurity Level: 128" << std::endl;
    std::cout << std::left << std::setw(12) << "Function";
    for (auto length : lengths) {
        std::cout << std::right << std::setw(6) << length << "-bit ";
    }
    std::cout << std::endl;

    // Testing other operations
    for (size_t i = 0; i < otherOperations.size(); ++i) {
        std::cout << std::left << std::setw(12) << otherOperationNames[i];
        
        for (auto length : lengths) {
            int32_t plaintext = encodeDouble(length, input);
            LweSample* encInput = encryptBoolean(plaintext, length, params, key);

            auto start = std::chrono::high_resolution_clock::now();
            LweSample* resultEnc = new_gate_bootstrapping_ciphertext_array(length, params);
            otherOperations[i](resultEnc, encInput, length, &key->cloud);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            delete_gate_bootstrapping_ciphertext_array(length, resultEnc);
            delete_gate_bootstrapping_ciphertext_array(length, encInput);

            std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(3) << elapsed.count() << " ";
        }
        std::cout << std::endl;
    }

    // Cleanup keys and parameters for the second part
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

