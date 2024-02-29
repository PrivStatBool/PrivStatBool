#include <iostream>
#include <vector>
#include <chrono>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include "utils.h"  
#include "utilMat.h"  
#include "native/HomSort.h"  
#include "native/HomMatrix.h"  
#include "plain/plainSort.h"
#include "experiment/metric.h"


int main() {
    std::cout << "Demo: Quartile" << std::endl;

    // Initialize TFHE parameters and keyset
    auto params = initializeParams(128);  // Adjust the security parameter as needed
    auto key = generateKeySet(params);

    int length = 16;  // Bit length for encryption

    // Load dataset from CSV
    std::vector<double> dataset = loadDataFromCSV("../../../data/sort_data.csv");
    printVector(dataset, "Original Dataset:");

    // Encrypt dataset
    LweSampleVector ciphertext = EncryptVector(dataset, length, params, key);

    // Perform Homomorphic Quartile calculation
    auto startHom = std::chrono::high_resolution_clock::now();
    LweSampleVector quartilesEncrypted = HomQuartile(ciphertext, length, &key->cloud);
    auto stopHom = std::chrono::high_resolution_clock::now();
    auto durationHom = std::chrono::duration_cast<std::chrono::milliseconds>(stopHom - startHom);
    std::cout << "Time taken: " << durationHom.count() / 1000.0 << " seconds" << std::endl;

    // Decrypt and print Homomorphic Quartiles
    std::vector<double> quartilesDecrypted = decryptLweVector(quartilesEncrypted, length, key);
    printVector(quartilesDecrypted, "Decrypted Results: ");

    // Perform Plain Quartile calculation
    std::vector<double> quartilesPlain = plainQuartile(dataset);

    // Print Plain Quartiles
    printVector(quartilesPlain, "Exact Results :");

    // Accuracy using MAPE
    double accuracy = evaluateAccuracy(quartilesDecrypted, quartilesPlain); 
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Clean up all pointers
    CleanupVector(ciphertext, length);
    CleanupVector(quartilesEncrypted, length);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

