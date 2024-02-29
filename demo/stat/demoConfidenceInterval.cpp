#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <tfhe/tfhe.h>
#include "utils.h"
#include "utilMat.h"
#include "native/HomMatrix.h"
#include "native/HomStatBasic.h"
#include "plain/plainStat.h"
#include "experiment/metric.h"



int main() {
    std::cout << "Demo: Confidence Interval" << std::endl;

    // Initialize TFHE parameters and keyset
    auto params = initializeParams(128);
    auto key = generateKeySet(params);

    int rowNum = 4;
    int colNum = 2;

    // Bit length for encryption and desired confidence level
    int length = 16;
    double confidenceLevel = 95.0;

    // Load a dataset from CSV
    std::vector<std::vector<double>> dataset = loadPartialDataFromCSV("../../../data/3d_toy_data.csv", rowNum, colNum);
    printMatrix(dataset, "Data Matrix:");
 
    // Encrypt the dataset
    LweSampleMatrix encryptedData = EncryptDataset(dataset, length, params, key);

    // Calculate the confidence interval for the encrypted data
    auto start = std::chrono::high_resolution_clock::now();
    LweSampleMatrix ciEncrypted = HomConfInter(encryptedData, confidenceLevel, length, &key->cloud);
    auto stop = std::chrono::high_resolution_clock::now();

    // Measure and print computation time
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() / 1000.0 << " seconds" << std::endl;

    // Decrypt and decode the result
    std::vector<std::vector<double>> ciDecrypted = decryptLweMatrix(ciEncrypted, length, key);
    printMatrix(ciDecrypted, "Decrypted Results:");

    // Calculate and display the plain confidence interval for comparison
    std::vector<std::vector<double>> ciPlain = plainConfidenceInterval(dataset, confidenceLevel);
    printMatrix(ciPlain, "Exact Results:");

    // Accuracy using MAPE
    double accuracy = calculateMAPEMatrix(ciPlain, ciDecrypted);

    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Cleanup allocated resources
    CleanupMatrix(encryptedData, length);
    CleanupMatrix(ciEncrypted, length);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

