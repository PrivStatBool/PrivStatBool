#include <tfhe/tfhe.h>
#include <iostream>

#include "native/HomArith.h"
#include "utilServer.h"
#include "native/HomMatrix.h"
#include "native/HomNonLinearOper.h"
#include "native/HomSort.h"

LweSampleVector HomSum(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int rowNum = encData.size();
    int colNum = encData[0].size();

    // Initialize the result vector with arrays of ciphertexts
    LweSampleVector res(colNum);
    for (int i = 0; i < colNum; ++i) {
        res[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        for (int j = 0; j < length; ++j) {
            bootsCOPY(&res[i][j], &encData[0][i][j], bk);  // Copy the first row's data into res
        }
    }

    // Sequential Summation; Add column-wisely
    for (int i = 0; i < colNum; ++i) {
        for (int j = 1; j < rowNum; ++j) {  // Start from the second row
            HomAdd(res[i], res[i], encData[j][i], length, bk);
        }
    }

    return res;
}

LweSampleVector HomMean(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

    int rowNum = encData.size();
    int colNum = encData[0].size();

    // Encryption of 1/N as an array of ciphertexts
    LweSample* encOneOverN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    double oneOverN = 1.0 / rowNum;
    HomDoubleP2C(encOneOverN, oneOverN, length, bk);

    // Get the sum of all elements in each column
    LweSampleVector sumVector = HomSum(encData, length, bk);

    // Initialize the mean vector and calculate the mean for each column
    LweSampleVector meanVector(colNum);
    for (int i = 0; i < colNum; ++i) {
        meanVector[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomMult(meanVector[i], encOneOverN, sumVector[i], length, bk);
    }

    // Cleanup
    delete_gate_bootstrapping_ciphertext_array(length, encOneOverN);
    for (auto& sample : sumVector) {
        delete_gate_bootstrapping_ciphertext_array(length, sample);
    }

    return meanVector;
}

LweSampleVector HomVar(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int rowNum = encData.size();
    int colNum = encData[0].size();

    // Encryption of 1/N as an array of ciphertexts
    LweSample* encOneOverN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    double oneOverN = 1.0 / rowNum;
    HomDoubleP2C(encOneOverN, oneOverN, length, bk);

    // Calculate the mean
    LweSampleVector encMu = HomMean(encData, length, bk);

    // Initialize variance vector
    LweSampleVector Var(colNum);
    for (int i = 0; i < colNum; ++i) {
        Var[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomDoubleP2C(Var[i], 0.0, length, bk); // Initializing with 0
    }

    LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    for (int i = 0; i < colNum; ++i) {
        for (int j = 0; j < rowNum; ++j) {
            HomSubt(temp, encData[j][i], encMu[i], length, bk); // (x_ij - mu_i)
            HomMult(temp, temp, temp, length, bk); // Squaring (x_ij - mu_i)^2
            HomAdd(Var[i], Var[i], temp, length, bk); // Summing up
        }
        HomMult(Var[i], Var[i], encOneOverN, length, bk); // Dividing by N
    }

    // Cleanup
    delete_gate_bootstrapping_ciphertext_array(length, encOneOverN);
    delete_gate_bootstrapping_ciphertext_array(length, temp);
    for (auto& mu : encMu) {
        delete_gate_bootstrapping_ciphertext_array(length, mu);
    }

    return Var;
}

LweSampleVector HomStd(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int colNum = encData[0].size();

    LweSampleVector stdVec(colNum, nullptr);
    for (auto &std : stdVec)
        std = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    // Find variance of encData
    stdVec = HomVar(encData, length, bk);

    // Compute the square root of each variance to get the standard deviation
    for (auto& std : stdVec)
        HomSqRoot(std, std, length, bk);

    return stdVec;
}

LweSampleMatrix HomConfInter(const LweSampleMatrix& encData, double confidenceLevel, int length, const TFheGateBootstrappingCloudKeySet* bk) {

    if (confidenceLevel != 95.0 && confidenceLevel != 99.0) {
        throw std::invalid_argument("Currently, only 95% and 99% confidence levels are supported.");
    }

    int N = encData.size();  // Assuming encData is a vector of vectors (matrix)

    // Calculate the sample mean and standard deviation
    LweSampleVector sampleMean = HomMean(encData, length, bk);
    LweSampleVector sampleStd = HomStd(encData, length, bk);

    // Set the zScore based on the confidence level
    double zScore = (confidenceLevel == 95.0) ? 1.96 : 2.576;

    LweSample* encZScore = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    HomDoubleP2C(encZScore, zScore, length, bk);

    double oneOverSqrtN = 1.0 / sqrt(N);
    LweSample* encOneOverSqrtN = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    HomDoubleP2C(encOneOverSqrtN, oneOverSqrtN, length, bk);

    LweSampleMatrix interval(sampleMean.size(), LweSampleVector(2, nullptr));
    for (size_t i = 0; i < sampleMean.size(); ++i) {
        interval[i][0] = new_gate_bootstrapping_ciphertext_array(length, bk->params);  // Lower bound
        interval[i][1] = new_gate_bootstrapping_ciphertext_array(length, bk->params);  // Upper bound

        LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomMult(temp, encOneOverSqrtN, sampleStd[i], length, bk);  // s / sqrt(N)
        HomMult(temp, temp, encZScore, length, bk);  // z * s / sqrt(N)

        HomSubt(interval[i][0], sampleMean[i], temp, length, bk);  // mean - z * s / sqrt(N)
        HomAdd(interval[i][1], sampleMean[i], temp, length, bk);  // mean + z * s / sqrt(N)

        delete_gate_bootstrapping_ciphertext_array(length, temp);
    }

    // Cleanup
    delete_gate_bootstrapping_ciphertext_array(length, encZScore);
    delete_gate_bootstrapping_ciphertext_array(length, encOneOverSqrtN);

    return interval;
}

LweSampleVector HomSkewness(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

    /*
		Two ways to measure skewness of distribution: 
			(1) Pearson's first coefficient
			(2) Pearson's second coefficient

		In (2), sk = (3 * X_bar - Md) / s where sk: skewness, X_bar: sample mean, Md: median, s: sample std 

    */


    int N = encData.size();  // Number of samples
    int colNum = encData[0].size();  // Number of columns

     // Compute sample mean and standard deviation
     LweSampleVector sampleMean(colNum, nullptr);
     for (int i = 0; i < colNum; i++) {
         sampleMean[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
     }
     sampleMean = HomMean(encData, length, bk);
	
     LweSampleVector sampleStd(colNum, nullptr);
     for (int i = 0; i < colNum; i++) {
         sampleStd[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
     }
     sampleStd = HomStd(encData, length, bk);

     // Compute sample median for each column
     LweSampleVector sampleMedian(colNum, nullptr);
     for (int i = 0; i < colNum; i++) {
         sampleMedian[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
     }

     for (int i = 0; i < colNum; ++i) {
         // Extracting the i-th column
         LweSampleVector columnData(N, nullptr);
         for (int j = 0; j < N; ++j){ 
             columnData[j] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	     columnData[j] = encData[j][i];
	 }
         sampleMedian[i] = HomMedian(columnData, length, bk);  // Compute median for the i-th column
     }
 
    // Encrypt the coefficient 3.0
    LweSample* encCoeff = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    HomDoubleP2C(encCoeff, 3.0, length, bk);

    // Initialize the result vector for skewness values
    LweSampleVector skewness(colNum, nullptr);
    for (int i = 0; i < colNum; ++i) {
        skewness[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);

        // Compute 3 * X_bar - Md
        LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomSubt(temp, sampleMean[i], sampleMedian[i], length, bk);    // X_bar - Md
        HomMult(temp, encCoeff, temp, length, bk);  // 3 * (X_bar - Md)

        // Divide by s to get skewness
        HomDiv(skewness[i], temp, sampleStd[i], length, bk);

        // Cleanup temporary variables
        delete_gate_bootstrapping_ciphertext_array(length, temp);
    }

    // Cleanup
    delete_gate_bootstrapping_ciphertext_array(length, encCoeff);
    for (auto& mean : sampleMean) {
        delete_gate_bootstrapping_ciphertext_array(length, mean);
    }
    for (auto& median : sampleMedian) {
        delete_gate_bootstrapping_ciphertext_array(length, median);
    }
    for (auto& std : sampleStd) {
        delete_gate_bootstrapping_ciphertext_array(length, std);
    }

    return skewness;  // Return the skewness for each column
}

//// Covariance
LweSample* HomCovariance(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	int N = encData.size();
	int colNum = encData[0].size();

	LweSampleVector sampleMean(colNum, nullptr);
	for (int i=0; i < colNum; i++)
		sampleMean[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	sampleMean = HomMean(encData, length, bk);
	LweSample* result = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomDoubleP2C(result, 0.0, length, bk);	// setting result to 0

	for (int i=0; i < N; i++) {
		LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		LweSample* temp2 = new_gate_bootstrapping_ciphertext_array(length, bk->params);
		HomSubt(temp, encData[i][0], sampleMean[0], length, bk); // x_i0 - mu_0
		HomSubt(temp2, encData[i][1], sampleMean[1], length, bk); // x_i1 - mu_1
		HomMult(temp, temp, temp2, length, bk); // temp = temp * temp2
		HomAdd(result, result, temp, length, bk); // result += temp
		delete_gate_bootstrapping_ciphertext_array(length, temp);
		delete_gate_bootstrapping_ciphertext_array(length, temp2);
	}

	double coeff = 1.0 / (N - 1);
	LweSample* encCoeff = new_gate_bootstrapping_ciphertext_array(length, bk->params);
	HomDoubleP2C(encCoeff, coeff, length, bk);

	HomMult(result, result, encCoeff, length, bk);

	delete_gate_bootstrapping_ciphertext_array(length, encCoeff);
	for (int i =0; i < colNum; i++)
		delete_gate_bootstrapping_ciphertext_array(length, sampleMean[i]);

	return result;
}
