#include <tfhe/tfhe.h>
#include "native/HomBitOper.h"
#include "native/HomArith.h"
#include "native/HomComp.h"
#include "utilServer.h"

// Function to perform bitwise AND operation on encrypted bits
void bitwiseAND(LweSample* res, const LweSample* bitA, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    for (int i = 0; i < length; i++) {
        bootsAND(&res[i], bitA, &b[i], bk);
    }
}

// Function to perform bitwise XOR operation on encrypted bits
void bitwiseXOR(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    for (int i = 0; i < length; i++) {
        bootsXOR(&res[i], &a[i], &b[i], bk);
    }
}

void HomSqRoot(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* smallResult = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* largeResult = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* decisionBit = new_gate_bootstrapping_ciphertext_array(1, bk->params);
    LweSample* normalizedResult = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* xorSum = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    HomDoubleP2C(xorSum, 0.0, length, bk); // Initialize xorSum with 0.0

    LweSample* normEvenConst = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    HomDoubleP2C(normEvenConst, 0.5, length, bk);  // 0.5 for even exponent
    LweSample* normOddConst = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* normOddConstFrac = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    double OddConst = 0.5 * sqrt(2); // 0.5 * sqrt(2) for odd exponent approximation
    HomDoubleP2C(normOddConst, OddConst, length, bk);
    double OddConstFrac = 0.5 / sqrt(2);
    HomDoubleP2C(normOddConstFrac, OddConstFrac, length, bk);


    for (int i = 0; i < length - 2; i++) {
        double leftVal = pow(2, 1 - length / 2 + i);
        double rightVal = pow(2, 1 - length / 2 + i + 1);

        LweSample* encLeft = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomDoubleP2C(encLeft, leftVal, length, bk); // Encrypt leftVal
        LweSample* encRight = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomDoubleP2C(encRight, rightVal, length, bk); // Encrypt rightVal

        HomCompGE(smallResult, a, encLeft, length, bk); // a >= encLeft
        HomCompL(largeResult, a, encRight, length, bk); // a < encRight
        bootsAND(decisionBit, smallResult, largeResult, bk); // smallResult AND largeResult

        LweSample* approxResult = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        LweSample* approxResultTemp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        if (0 <= i && i < length / 2 - 1) {
            HomRShift(normalizedResult, a, length, length / 2 - 2 - i, bk); // Right shift for normalization
            if ((length / 2 - 2 - i) % 2 == 0) { // Even exponent case
                HomLShift(approxResult, normalizedResult, length, 1, bk); // 0.5 * a
                HomAdd(approxResult, approxResult, normEvenConst, length, bk); // 0.5 + 0.5 * a = a
                HomLShift(approxResult, approxResult, length, (length / 2 - 2 - i) / 2, bk); // Shift back
            } else { // Odd exponent case
                HomLShift(approxResultTemp, normalizedResult, length, 2, bk); // 0.25 * a
                HomLShift(approxResult, normalizedResult, length, 3, bk); // 0.125 * a
                HomAdd(approxResult, approxResult, approxResultTemp, length, bk); // 0.375 * a
                HomAdd(approxResult, approxResult, normOddConstFrac, length, bk); // 0.375 * a + 0.5 / sqrt(2)
                HomLShift(approxResult, approxResult, length, (length / 2 - 2 - i) / 2, bk); // Shift back
            }
        }
	
	 else {
            HomLShift(normalizedResult, a, length, i - length / 2 + 1, bk); // Left shift for normalization
            if ((i - length / 2 + 1) % 2 == 0) { // Even exponent case
                HomLShift(approxResult, normalizedResult, length, 1, bk); // 0.5 * a
                HomAdd(approxResult, approxResult, normEvenConst, length, bk); // 0.5 + 0.5 * a
                HomRShift(approxResult, approxResult, length, (i - length / 2 + 1) / 2, bk); // Shift back
            } else { // Odd exponent case
                HomLShift(approxResultTemp, normalizedResult, length, 1, bk); // 0.5 * a
                HomLShift(approxResult, normalizedResult, length, 2, bk); // 0.25 * a
                HomAdd(approxResult, approxResult, approxResultTemp, length, bk); // 0.75 * a
                HomAdd(approxResult, approxResult, normOddConst, length, bk); // 0.75 * a + 0.5 * sqrt(2)
                HomRShift(approxResult, approxResult, length, (i - length / 2 + 1) / 2, bk); // Shift back
            }
        }

        bitwiseAND(approxResult, decisionBit, approxResult, length, bk); // Apply AND with decisionBit
        bitwiseXOR(xorSum, xorSum, approxResult, length, bk); // Accumulate result in xorSum

        delete_gate_bootstrapping_ciphertext_array(length, encLeft);
        delete_gate_bootstrapping_ciphertext_array(length, encRight);
        delete_gate_bootstrapping_ciphertext_array(length, approxResult);
        delete_gate_bootstrapping_ciphertext_array(length, approxResultTemp);
    }

    for (int i = 0; i < length; i++) {
        bootsCOPY(&res[i], &xorSum[i], bk); // Copy result to res
    }

    // Free the allocated memory
    delete_gate_bootstrapping_ciphertext_array(1, smallResult);
    delete_gate_bootstrapping_ciphertext_array(1, largeResult);
    delete_gate_bootstrapping_ciphertext_array(1, decisionBit);
    delete_gate_bootstrapping_ciphertext_array(length, normalizedResult);
    delete_gate_bootstrapping_ciphertext_array(length, xorSum);
    delete_gate_bootstrapping_ciphertext_array(length, normEvenConst);
    delete_gate_bootstrapping_ciphertext_array(length, normOddConst);
    delete_gate_bootstrapping_ciphertext_array(length, normOddConstFrac);
}

