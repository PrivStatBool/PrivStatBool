#include <iostream>
#include <vector>
#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include "utils.h"
#include "utilMat.h"
#include "native/HomSort.h"
#include "native/HomMatrix.h"
#include "plain/plainSort.h"  
#include "experiment/metric.h"


int main() {
    std::cout << "Demo: Range" << std::endl;

    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    int length = 16;

    // Load dataset from CSV
    std::vector<double> dataset = loadDataFromCSV("../../../data/sort_data.csv");
    printVector(dataset, "Original Dataset:");

    // Encrypt dataset
    LweSampleVector ciphertext = EncryptVector(dataset, length, params, key);

    // Result will store the sorted encrypted data
    LweSample* ctx_answer = new_gate_bootstrapping_ciphertext_array(length, params);

    // Bubble Sort the encrypted data
    auto start = std::chrono::high_resolution_clock::now();
    ctx_answer = HomRange(ciphertext, length, &key->cloud);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << "Time taken: " << seconds << " seconds" << std::endl;

    // Decrypt and decode (range)
    std::vector<int> rangeResultPlain = decryptToBinaryVector(ctx_answer, length, key);
    double rangeResult = decodeDouble(rangeResultPlain);    

    std::cout << "Decrypted Result: " << rangeResult << std::endl;

    // Calculate and display the plain skewness for comparison
    double answerPlain = plainRange(dataset);
    std::cout << "Exact Results: " << answerPlain << std::endl;

    // Accuracy using MAPE
    double accuracy = calculateMAPEScalr(answerPlain, rangeResult); 
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

   // Clean up all pointers
    CleanupVector(ciphertext, length);
    delete_gate_bootstrapping_ciphertext_array(length, ctx_answer);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}
