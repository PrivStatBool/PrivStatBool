#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <tfhe/tfhe.h>
#include "utils.h"
#include "utilMat.h"
#include "native/HomSort.h"
#include "native/HomMatrix.h"
#include "plain/plainSort.h"
#include "experiment/metric.h"

int main() {
    std::cout << "Demo: Percentile" << std::endl;

    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    int length = 16;
    double percentileValue = 50;  // Desired percentile

    // Load dataset from CSV
    std::vector<double> dataset = loadDataFromCSV("../../../data/sort_data.csv");
    printVector(dataset, "Original Dataset:");

    // Encrypt dataset
    LweSampleVector ciphertext = EncryptVector(dataset, length, params, key);

    // Calculate the desired percentile of the encrypted data
    auto start = std::chrono::high_resolution_clock::now();
    LweSample* ctx_answer = HomPercentile(ciphertext, percentileValue, length, &key->cloud);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << "Time taken: " << seconds << " seconds" << std::endl;

    // Decrypt and decode the result
    std::vector<int> percentileResultPlain = decryptToBinaryVector(ctx_answer, length, key);
    double percentileResult = decodeDouble(percentileResultPlain);
    std::cout << "Decrypted Result: " << percentileResult << std::endl;

    // compare with plainResult
    double plainResult = plainPercentile(dataset, percentileValue);
    std::cout << "Exact Result: " << plainResult << std::endl;

    // Accuracy using MAPE
    double accuracy = calculateMAPEScalr(plainResult, percentileResult); 
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Clean up all pointers
    CleanupVector(ciphertext, length);
    delete_gate_bootstrapping_ciphertext_array(length, ctx_answer);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

