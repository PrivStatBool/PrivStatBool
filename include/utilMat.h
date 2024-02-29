#pragma once

#include <native/HomMatrix.h>

LweSampleVector EncryptVector(const std::vector<double>& dataset, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key); 
 
LweSampleMatrix EncryptDataset(const std::vector<std::vector<double>>& dataset, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key);

std::vector<double> decryptLweVector(const LweSampleVector& encryptedVector, const int length, const TFheGateBootstrappingSecretKeySet* key); 

std::vector<std::vector<double>> decryptLweMatrix(const LweSampleMatrix& encryptedMatrix, const int length, const TFheGateBootstrappingSecretKeySet* key);

void CleanupVector(LweSampleVector& vector, int length); 

void CleanupMatrix(LweSampleMatrix& matrix, int length);

void printVector(const std::vector<double>& vec, const std::string& title = ""); 

void printMatrix(const std::vector<std::vector<double>>& matrix, const std::string& title = "");

