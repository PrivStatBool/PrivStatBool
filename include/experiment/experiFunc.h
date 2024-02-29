#ifndef EXPERIFUNC_H
#define EXPERIFUNC_H

#include <vector>
#include <functional>
#include <tfhe/tfhe.h>
#include "native/HomMatrix.h"

// Define the function type for homomorphic operations
using HomOpFunction = std::function<LweSampleVector(const LweSampleMatrix&, const int, const TFheGateBootstrappingCloudKeySet*)>;

// Define the function type for plaintext operations
using PlainOpFunction = std::function<std::vector<double>(const std::vector<std::vector<double>>&)>;

// for 1d function HE type
using Hom1dFunction = std::function<LweSampleVector(const LweSampleVector&, int, const TFheGateBootstrappingCloudKeySet*)>;

// for 1d function Plain type
using Plain1dFunction = std::function<std::vector<double>(const std::vector<double>&)>;


struct ExperimentResult {
    double time; // Time taken for the homomorphic operation
    std::vector<double> homResults; // Results of the homomorphic operation
    std::vector<double> exactResults; // Exact results for comparison
    double accuracy; // Accuracy of the homomorphic results compared to the exact results
};

ExperimentResult runExperiment(int securityLevel, int length, const std::vector<std::vector<double>>& dataset, HomOpFunction homOp, PlainOpFunction plainOp); 
void runExperimentsAndOutputToCSV(const std::string& filename, const std::string& dataFilePath, const std::vector<int>& securityLevels, const std::vector<int>& lengths, int trials, int rowNum, int colNum, HomOpFunction homOp, PlainOpFunction plainOp); 

ExperimentResult run1dExperiment(int securityLevel, int length, const std::vector<double>& dataset, Hom1dFunction homOp, Plain1dFunction plainOp);
void run1dExperimentsAndOutputToCSV(const std::string& filename, const std::string& dataFilePath, const std::vector<int>& securityLevels, const std::vector<int>& lengths, int trials, Hom1dFunction homOp, Plain1dFunction plainOp); 
 
#endif
