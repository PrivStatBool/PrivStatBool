#include <vector>
#include <tfhe/tfhe.h>
#include <iostream>
#include "native/HomMatrix.h"
#include "native/HomStatBasic.h"
#include "native/HomSort.h"
#include "utils.h"
#include "utilMat.h"
#include "plain/plainStat.h"
#include "plain/plainSort.h"
#include "experiment/experiFunc.h"
#include "experiment/metric.h"
#include <fstream>
#include <functional>

// Define the function type for homomorphic operations: > 1d
using HomOpFunction = std::function<LweSampleVector(const LweSampleMatrix&, const int, const TFheGateBootstrappingCloudKeySet*)>;

// Define the function type for plaintext operations: > 1d
using PlainOpFunction = std::function<std::vector<double>(const std::vector<std::vector<double>>&)>;

// for 1d function HE type
using Hom1dFunction = std::function<LweSampleVector(const LweSampleVector&, int, const TFheGateBootstrappingCloudKeySet*)>;

// for 1d function Plain type
using Plain1dFunction = std::function<std::vector<double>(const std::vector<double>&)>;


/*		Experiments with dimension over than 1		*/
ExperimentResult runExperiment(int securityLevel, int length, const std::vector<std::vector<double>>& dataset, HomOpFunction homOp, PlainOpFunction plainOp) {
    // Initialize TFHE parameters and keys
    auto params = initializeParams(securityLevel); 
    auto key = generateKeySet(params);
    
    // Encrypt dataset
    LweSampleMatrix EncData = EncryptDataset(dataset, length, params, key);

    // Prepare for homomorphic operation and timing
    LweSampleVector ctx_answer(dataset[0].size()); // Assuming dataset[0].size() gives the number of columns
    for (auto& sample : ctx_answer) {
        sample = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    // Start timing the homomorphic operation
    auto start = std::chrono::high_resolution_clock::now();

    // Perform the homomorphic operation passed as a parameter
    ctx_answer = homOp(EncData, length, &key->cloud);

    // Stop timing
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;

    // Decrypt and decode
    std::vector<double> decryptedResult = decryptLweVector(ctx_answer, length, key);

    // Compute the exact result using the plaintext operation passed as a parameter
    std::vector<double> exactResult = plainOp(dataset); 

    // Compute accuracy
    double accuracy = evaluateAccuracy(decryptedResult, exactResult); 

    // Cleanup
    CleanupVector(ctx_answer, length); 
    CleanupMatrix(EncData, length); 
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    // Package and return the results
    return {seconds, decryptedResult, exactResult, accuracy};
}

void runExperimentsAndOutputToCSV(const std::string& filename, const std::string& dataFilePath, const std::vector<int>& securityLevels, const std::vector<int>& lengths, int trials, int rowNum, int colNum, HomOpFunction homOp, PlainOpFunction plainOp) {
    // Open file for writing
    std::ofstream outFile(filename);

    // Write header
    outFile << "Security Level,Length,Time (s)";
    for (int i = 0; i < colNum; ++i) {
        outFile << ",Hom Result " << i + 1;
    }
    for (int i = 0; i < colNum; ++i) {
        outFile << ",Exact Result " << i + 1;
    }
    outFile << ",Accuracy\n";

    // Iterate over parameters and run experiments
    for (int securityLevel : securityLevels) {
        for (int length : lengths) {
            // Log the start of experiments for the current security level and length
            std::cout << "Running experiments for security level: " << securityLevel << ", length: " << length << std::endl;

            for (int trial = 0; trial < trials; ++trial) {
                // Load dataset from the provided file path
                std::vector<std::vector<double>> dataset = loadPartialDataFromCSV(dataFilePath, rowNum, colNum);

                // Run experiment
                ExperimentResult result = runExperiment(securityLevel, length, dataset, homOp, plainOp);

                // Write results to file
                outFile << securityLevel << "," << length << "," << result.time;

                // Write each homomorphic result
                for (double homResult : result.homResults) {
                    outFile << "," << homResult;
                }

                // Write each exact result
                for (double exactResult : result.exactResults) {
                    outFile << "," << exactResult;
                }

                // Write accuracy
                outFile << "," << result.accuracy << "\n";
            }
        }
    }

    // Close file
    outFile.close();
}

/*		Experiments with dimension equal to 1		*/
ExperimentResult run1dExperiment(int securityLevel, int length, const std::vector<double>& dataset, Hom1dFunction homOp, Plain1dFunction plainOp) {
    // Initialize TFHE parameters and keys
    auto params = initializeParams(securityLevel); 
    auto key = generateKeySet(params);

    // Encrypt dataset into an LweSampleVector
    LweSampleVector encryptedData = EncryptVector(dataset, length, params, key);

    // Define sortedEncrypted to store the sorted result
    LweSampleVector sortedEncrypted(dataset.size(), nullptr);
    for (auto &sample : sortedEncrypted) {
        sample = new_gate_bootstrapping_ciphertext_array(length, params);
    }

    // Start timing the homomorphic operation
    auto start = std::chrono::high_resolution_clock::now();

    // Perform the homomorphic sort operation and store the result in sortedEncrypted
    sortedEncrypted = homOp(encryptedData, length, &key->cloud);

    // Stop timing
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    double seconds = duration.count() / 1000.0;

    // Decrypt and decode the sorted encrypted data
    std::vector<double> decryptedSortedResult = decryptLweVector(sortedEncrypted, length, key);

    // Compute the exact sorted result using the plaintext sort operation
    std::vector<double> exactSortedResult = plainOp(dataset);

    // Compute accuracy if applicable (for sorting, this might involve checking the order of elements)
    double accuracy = evaluateAccuracy(decryptedSortedResult, exactSortedResult);

    // Cleanup
    CleanupVector(encryptedData, length);
    CleanupVector(sortedEncrypted, length);
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);
    
    // Package and return the results
    return {seconds, decryptedSortedResult, exactSortedResult, accuracy};
}


void run1dExperimentsAndOutputToCSV(const std::string& filename, const std::string& dataFilePath, const std::vector<int>& securityLevels, const std::vector<int>& lengths, int trials, Hom1dFunction homOp, Plain1dFunction plainOp) {
    std::ofstream outFile(filename);

    // Write header
    outFile << "Security Level,Length,Time (s),Accuracy\n";

    for (int securityLevel : securityLevels) {
        for (int length : lengths) {
            std::cout << "Running 1D experiments for security level: " << securityLevel << ", length: " << length << std::endl;

            for (int trial = 0; trial < trials; ++trial) {
                std::vector<double> dataset = loadDataFromCSV(dataFilePath); // Load the dataset for each trial
                ExperimentResult result = run1dExperiment(securityLevel, length, dataset, homOp, plainOp);

                // Write results to file
                outFile << securityLevel << "," << length << "," << result.time;

                // Write each homomorphic result
                for (double homResult : result.homResults) {
                    outFile << "," << homResult;
                }

                // Write each exact result
                for (double exactResult : result.exactResults) {
                    outFile << "," << exactResult;
                }

                // Write accuracy
                outFile << "," << result.accuracy << "\n";
 
            }
        }
    }

    outFile.close();
}

