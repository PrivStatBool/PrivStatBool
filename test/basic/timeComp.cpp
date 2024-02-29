#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>  
#include "tfhe/tfhe.h" 
#include "utils.h"
#include "native/HomComp.h"  

int main() {
    // Initialize parameters and keys
    auto params = initializeParams(128);  // Adjust security parameter as needed
    auto key = generateKeySet(params);

    // Define lengths for testing
    std::vector<int> lengths = {8, 16, 32};

    // Single test case for simplicity
    double input = 2.5;

    // Homomorphic comparison operations
    std::vector<std::function<void(LweSample*, const LweSample*, const LweSample*, const int, const TFheGateBootstrappingCloudKeySet*)>> operations = {
        HomCompGE, HomCompLE, HomCompG, HomCompL, HomEqui
    };

    std::vector<std::string> operationNames = {"HomCompGE", "HomCompLE", "HomCompG", "HomCompL", "HomEqui"};

    // Header
    std::cout << "Input: " << input << "\nSecurity Level: 128" << std::endl;
    std::cout << std::left << std::setw(12) << "Function";
    for (auto length : lengths) {
        std::cout << std::right << std::setw(6) << length << "-bit ";
    }
    std::cout << std::endl;

    // Operations
    for (size_t i = 0; i < operations.size(); ++i) {
        std::cout << std::left << std::setw(12) << operationNames[i];
        
        for (auto length : lengths) {
            // Encode and encrypt
            int32_t plaintext = encodeDouble(length, input);
            LweSample* encInput = encryptBoolean(plaintext, length, params, key);
            LweSample* encZero = new_gate_bootstrapping_ciphertext_array(length, params);  // Comparing with zero for simplicity
            bootsCONSTANT(encZero, 0, &key->cloud);  // Set second input to 0
            
            // Measure time
            auto start = std::chrono::high_resolution_clock::now();
            LweSample* resultEnc = new_gate_bootstrapping_ciphertext_array(1, params);  // Output is a single bit
            operations[i](resultEnc, encInput, encZero, length, &key->cloud);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            // Cleanup
            delete_gate_bootstrapping_ciphertext_array(1, resultEnc);
            delete_gate_bootstrapping_ciphertext_array(length, encInput);
            delete_gate_bootstrapping_ciphertext_array(length, encZero);

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

