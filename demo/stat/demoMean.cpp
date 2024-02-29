#include <iostream>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <vector>
#include <chrono>
#include "utils.h"
#include "utilMat.h"
#include "native/HomMatrix.h"
#include "native/HomStatBasic.h"
#include "experiment/metric.h" 
#include "plain/plainStat.h"

int main() {
    std::cout << "Demo: Mean of elements" << std::endl;

    // Initialize TFHE parameters and keys
    auto params = initializeParams(128); 
    auto key = generateKeySet(params);
    int length = 16;
    int rowNum = 5;
    int colNum = 2;

    // Load dataset from CSV
    std::vector<std::vector<double>> dataset = loadPartialDataFromCSV("../../../data/3d_toy_data.csv", rowNum, colNum);
    printMatrix(dataset, "Data matrix:");

    // Encrypt dataset
    LweSampleMatrix EncData = EncryptDataset(dataset, length, params, key);

    // Initialize ctx_answer vector with colNum elements, then allocate memory for LweSamples
    LweSampleVector ctx_answer(colNum);
    for (int i = 0; i < colNum; ++i) {
        ctx_answer[i] = new_gate_bootstrapping_ciphertext_array(length, params);
    }
    
    auto start = std::chrono::high_resolution_clock::now();

    // Demo: Find Mean
    ctx_answer = HomMean(EncData, length, &key->cloud); 

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << "Time taken: " << seconds << " seconds" << std::endl;

    // Decrypt and decode
    std::vector<double> decryptedResults = decryptLweVector(ctx_answer, length, key);
    printVector(decryptedResults, "Decrypted Results:");

    // Compute exact mean results for comparison
    std::vector<double> plainMeans = plainMean(dataset); // Implement this function similar to plainSum
    printVector(plainMeans, "Exact Result:");

    // Accuracy using MAPE
    double accuracy = evaluateAccuracy(decryptedResults, plainMeans); // Ensure this function calculates MAPE
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Clean up all pointers
    CleanupMatrix(EncData, length); 
    CleanupVector(ctx_answer, length); 

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

