#include <iostream>
#include <vector>
#include <chrono>
#include "utils.h"  
#include "utilMat.h"
#include "plain/plainStat.h"  
#include "native/HomMatrix.h"  
#include "native/HomStatBasic.h"  
#include "experiment/metric.h"


int main() {
    std::cout << "Demo: Covariance" << std::endl;

    // Initialize TFHE parameters and keyset
    auto params = initializeParams(128);  // Adjust the security parameter as needed
    auto key = generateKeySet(params);

    // Specify the dataset dimensions and bit length for encryption
    int rowNum = 4;  
    int colNum = 2;  
    int length = 16;  

    // Load a dataset (adjust the path to your dataset)
    std::vector<std::vector<double>> dataset = loadPartialDataFromCSV("../../../data/3d_toy_data.csv", rowNum, colNum);
    printMatrix(dataset, "Data Matrix:");

    // Encrypt the dataset
    LweSampleMatrix encryptedData = EncryptDataset(dataset, length, params, key);

    // Calculate skewness for the encrypted data
    auto startHom = std::chrono::high_resolution_clock::now();
    LweSample* covarianceEncrypted = HomCovariance(encryptedData, length, &key->cloud);
    auto stopHom = std::chrono::high_resolution_clock::now();

    // Decrypt and decode the homomorphic skewness result
    std::vector<int> plCovariance = decryptToBinaryVector(covarianceEncrypted, length, key); 
    double covarianceDecrypted = decodeDouble(plCovariance);	

    std::cout << "Decrypted Results: " << covarianceDecrypted << std::endl;

    // Calculate and display the plain skewness for comparison
    double covariancePlain = plainCovariance(dataset);
    std::cout << "Exact Results: " << covariancePlain << std::endl;

    // Measure and print computation times
    auto durationHom = std::chrono::duration_cast<std::chrono::milliseconds>(stopHom - startHom);
    std::cout << "Time taken: " << durationHom.count() / 1000.0 << " seconds" << std::endl;

    // Accuracy using MAPE
    double accuracy = calculateMAPEScalr(covariancePlain, covarianceDecrypted); 
    std::cout << "Accuracy (using MAPE): " << accuracy << std::endl;

    // Cleanup allocated resources
    CleanupMatrix(encryptedData, length);
    delete_gate_bootstrapping_ciphertext_array(length, covarianceEncrypted);

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

