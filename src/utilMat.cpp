#include "native/HomMatrix.h"
#include <tfhe/tfhe.h>
#include <vector>
#include <iostream>
#include "utils.h"

// Encryption: Vector
LweSampleVector EncryptVector(const std::vector<double>& dataset, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key) {
    LweSampleVector ciphertextVector(dataset.size());

    for (size_t i = 0; i < dataset.size(); ++i) {
        int32_t plaintext = encodeDouble(length, dataset[i]);
        ciphertextVector[i] = encryptBoolean(plaintext, length, params, key);
    }

    return ciphertextVector;
}


// Encryption: normally Matrix
LweSampleMatrix EncryptDataset(const std::vector<std::vector<double>>& dataset, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key){

    int M = dataset.size();
    int N = dataset[0].size();  
    LweSampleMatrix ciphertextMatrix(M, std::vector<LweSample*>(N));

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            int32_t plaintext = encodeDouble(length, dataset[i][j]);
            ciphertextMatrix[i][j] = encryptBoolean(plaintext, length, params, key);
        }
    }

    return ciphertextMatrix;

}

// Decryption: Vector
std::vector<double> decryptLweVector(const LweSampleVector& encryptedVector, const int length, const TFheGateBootstrappingSecretKeySet* key) {
    std::vector<double> resultVector(encryptedVector.size(), 0.0);

    for (size_t i = 0; i < encryptedVector.size(); ++i) {
        std::vector<int> decryptedBinary = decryptToBinaryVector(encryptedVector[i], length, key);
        double decodedValue = decodeDouble(decryptedBinary);
        resultVector[i] = decodedValue;
    }

    return resultVector;
}

// Decryption: Matrix
std::vector<std::vector<double>> decryptLweMatrix(const LweSampleMatrix& encryptedMatrix, const int length, const TFheGateBootstrappingSecretKeySet* key) {
    std::vector<std::vector<double>> resultMatrix(encryptedMatrix.size(), std::vector<double>(encryptedMatrix[0].size(), 0.0));

    for (size_t i = 0; i < encryptedMatrix.size(); ++i) {
        for (size_t j = 0; j < encryptedMatrix[i].size(); ++j) {
            std::vector<int> decryptedBinary = decryptToBinaryVector(encryptedMatrix[i][j], length, key);
            double decodedValue = decodeDouble(decryptedBinary);
            resultMatrix[i][j] = decodedValue;
        }
    }

    return resultMatrix;
}

// Clean LweSample Pointers in a Vector
void CleanupVector(LweSampleVector& vector, int length) {
    for (auto& element : vector) {
        delete_gate_bootstrapping_ciphertext_array(length, element);
    }

}

// Clean LweSample Pointers
void CleanupMatrix(LweSampleMatrix& matrix, int length){

    for (auto& row : matrix) {
        for (auto& cell : row) {
            delete_gate_bootstrapping_ciphertext_array(length, cell);
        }
    }

}

// Print Vector
void printVector(const std::vector<double>& vec, const std::string& title = "") {
    if (!title.empty()) {
        std::cout << title << std::endl;
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

// Print Matrix
void printMatrix(const std::vector<std::vector<double>>& matrix, const std::string& title = "") {
    if (!title.empty()) {
        std::cout << title << std::endl;
    }

    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout << matrix[i][j];
            if (j < matrix[i].size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}
