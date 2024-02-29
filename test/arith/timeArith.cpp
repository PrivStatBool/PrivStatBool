#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip> // For std::setprecision and std::setw
#include "tfhe/tfhe.h" 
#include "utils.h"
#include "native/HomArith.h"

// Function to calculate relative error
double relativeError(double homResult, double realResult) {
    return std::fabs((homResult - realResult) / realResult);
}

int main() {
    // Initialize parameters and keys
    auto params = initializeParams(128); // Adjust security parameter as needed
    auto key = generateKeySet(params);

    // Define lengths for testing
    std::vector<int> lengths = {8, 16, 32};

    // Test cases
    std::vector<std::pair<double, double>> testCases = {
        {3.5, 2.3}
    };

    // Homomorphic operations
    std::vector<std::function<void(LweSample*, const LweSample*, const LweSample*, const int, const TFheGateBootstrappingCloudKeySet*)>> operations = {
        HomAdd, HomSubt, HomMult, HomDiv
    };

    std::vector<std::string> operationNames = {"HomAdd", "HomSubt", "HomMult", "HomDiv"};

    // Header
    std::cout << "Input: " << testCases[0].first << ", " << testCases[0].second << std::endl;
    std::cout << "Security Level: 128" << std::endl;
    std::cout << std::left << std::setw(12) << "Function";
    for (auto length : lengths) {
        std::cout << std::right << std::setw(6) << length << "-bit ";
        // std::cout << std::right << std::setw(10) << length;
    }
    std::cout << std::endl;

    // Operations
    for (size_t i = 0; i < operations.size(); ++i) {
        std::cout << std::left << std::setw(12) << operationNames[i];
        
        for (auto length : lengths) {
            double a = testCases[0].first;
            double b = testCases[0].second;

            // Encode and encrypt
            int32_t plaintext1 = encodeDouble(length, a);
            int32_t plaintext2 = encodeDouble(length, b);
            LweSample* encA = encryptBoolean(plaintext1, length, params, key); 
            LweSample* encB = encryptBoolean(plaintext2, length, params, key); 

            // Measure time
            auto start = std::chrono::high_resolution_clock::now();
            LweSample* resultEnc = new_gate_bootstrapping_ciphertext_array(length, params);
            operations[i](resultEnc, encA, encB, length, &key->cloud);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            // Cleanup
            delete_gate_bootstrapping_ciphertext_array(length, resultEnc);
            delete_gate_bootstrapping_ciphertext_array(length, encA);
            delete_gate_bootstrapping_ciphertext_array(length, encB);

            // Output aligned time results
            std::cout << std::right << std::setw(10) << std::fixed << std::setprecision(2) << elapsed.count() << " ";
        }
        std::cout << std::endl;
    }

    // Cleanup keys and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

