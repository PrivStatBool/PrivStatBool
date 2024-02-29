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
    std::cout << "Demo: Bubble Sort Encrypted Array in Descending Order" << std::endl;

    auto params = initializeParams(80);
    auto key = generateKeySet(params);

    int length = 16;

    // Load dataset from CSV
    std::vector<double> dataset = loadDataFromCSV("../../../data/sort_data.csv");
    printVector(dataset, "Original Dataset:");

    // Encrypt dataset
    LweSampleVector ciphertext = EncryptVector(dataset, length, params, key);

    // Result will store the sorted encrypted data
    LweSampleVector ctx_answer(dataset.size(), nullptr);
    for (auto &sample : ctx_answer) {
        sample = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    // Bubble Sort the encrypted data
    auto start = std::chrono::high_resolution_clock::now();
    ctx_answer = HomBubbleSort(ciphertext, length, &key->cloud);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;
    std::cout << "Time taken: " << seconds << " seconds" << std::endl;

    // Decrypt and decode
    std::vector<double> sortedResult = decryptLweVector(ctx_answer, length, key);
    printVector(sortedResult, "Decrypted Results:");

    // Calculate and display the plain skewness for comparison
    std::vector<double> answerPlain = plainSort(dataset);
    printVector(answerPlain, "Exact Results:");

    // Accuracy using MAPE
    double accuracy = evaluateAccuracy(sortedResult, answerPlain); 
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Clean up all pointers
    CleanupVector(ciphertext, length);
    CleanupVector(ctx_answer, length);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

