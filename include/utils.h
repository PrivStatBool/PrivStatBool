#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <functional>
#include <tfhe/tfhe.h>

// encryption params and key initialization
TFheGateBootstrappingParameterSet* initializeParams(int minimum_lambda);
TFheGateBootstrappingSecretKeySet* generateKeySet(TFheGateBootstrappingParameterSet* params);

// encoding
int32_t encodeDouble(int length, double data); 
LweSample* encodeDoublePlain(int length, double data, const TFheGateBootstrappingCloudKeySet* bk); 

// decoding
double decodeDouble(const std::vector<int>& binaryVector); 
double decodeDoublePlain(const LweSample* noiselessEncoding, int length); 

// encrypt
LweSample* encryptBoolean(int32_t plaintext, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key); 

// decrypt
std::vector<int> decryptToBinaryVector(const LweSample* ciphertext, int length, const TFheGateBootstrappingSecretKeySet* key); 

// display
void print_double_data(double * data_double, int num_data);

// timing
template<typename Func, typename... Args>
double measureTime(Func func, Args... args) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Call the function with the provided arguments
    func(std::forward<Args>(args)...);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    return elapsed.count();
}

// load data
std::vector<double> loadDataFromCSV(const std::string& filename);
std::vector<std::vector<double>> loadPartialDataFromCSV(const std::string& filename, int rowNum, int colNum); 


#endif
