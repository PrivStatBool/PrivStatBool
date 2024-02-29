#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "tfhe/tfhe.h"
#include "utils.h"
#include "native/HomMinMax.h"  
#include "native/HomMatrix.h"

int main() {
    // Initialize parameters and keys
    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    // Define lengths for testing
    std::vector<int> lengths = {8, 16, 32};

    // Test cases
    std::vector<std::pair<double, double>> testCases = {
        {2.5, -1.7}
    };

    // Homomorphic operations for max and min
    std::vector<std::function<void(LweSample*, const LweSample*, const LweSample*, const int, const TFheGateBootstrappingCloudKeySet*)>> minMaxOperations = {
        HomMax, HomMin
    };

    std::vector<std::string> minMaxOperationNames = {"HomMax", "HomMin"};

    // Header for min and max operations
    std::cout << "Security Level: 128" << std::endl;
    std::cout << std::left << std::setw(12) << "Function";
    for (auto length : lengths) {
        std::cout << std::right << std::setw(6) << length << "-bit ";
    }
    std::cout << std::endl;

    // Testing min and max operations
    for (size_t i = 0; i < minMaxOperations.size(); ++i) {
        std::cout << std::left << std::setw(12) << minMaxOperationNames[i];
        
        for (auto length : lengths) {
            for (auto& testCase : testCases) {
                double a = testCase.first;
                double b = testCase.second;

                int32_t plaintextA = encodeDouble(length, a);
                int32_t plaintextB = encodeDouble(length, b);
                LweSample* encA = encryptBoolean(plaintextA, length, params, key); 
                LweSample* encB = encryptBoolean(plaintextB, length, params, key); 

                auto start = std::chrono::high_resolution_clock::now();
                LweSample* resultEnc = new_gate_bootstrapping_ciphertext_array(length, params);
                minMaxOperations[i](resultEnc, encA, encB, length, &key->cloud);
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> elapsed = end - start;

                delete_gate_bootstrapping_ciphertext_array(length, resultEnc);
                delete_gate_bootstrapping_ciphertext_array(length, encA);
                delete_gate_bootstrapping_ciphertext_array(length, encB);

                std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(3) << elapsed.count() << " ";
            }
        }
        std::cout << std::endl;
    }

    // Cleanup keys and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

