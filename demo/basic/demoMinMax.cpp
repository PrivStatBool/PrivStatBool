#include <iostream>
#include <tfhe/tfhe.h>
#include <vector>
#include "utils.h"
#include "native/HomMinMax.h"

int main() {
    std::cout << "Demo: Min/Max Correctness Test" << std::endl;

    auto params = initializeParams(128);
    auto key = generateKeySet(params);
    const TFheGateBootstrappingCloudKeySet* bk = &key->cloud;

    int length = 16;

    // Define your test cases
    std::vector<std::pair<double, double>> testCases = {
        {1.2, 3.1},
        {2.3, -1.2},
        {-1.2, 2.3},
        {-1.5, -2.6}
    };

    for (const auto& testCase : testCases) {
        double data1 = testCase.first;
        double data2 = testCase.second;

        int plaintext1 = encodeDouble(length, data1);
        int plaintext2 = encodeDouble(length, data2);

        LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key);
        LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key);

        LweSample* ctxMin = new_gate_bootstrapping_ciphertext_array(length, params);
        LweSample* ctxMax = new_gate_bootstrapping_ciphertext_array(length, params);

        HomMin(ctxMin, ciphertext1, ciphertext2, length, bk);
        HomMax(ctxMax, ciphertext1, ciphertext2, length, bk);

        // Decrypt and decode
        std::vector<int> plain_answer1 = decryptToBinaryVector(ctxMin, length, key);
        std::vector<int> plain_answer2 = decryptToBinaryVector(ctxMax, length, key);

        double answer1 = decodeDouble(plain_answer1);
        double answer2 = decodeDouble(plain_answer2);

        std::cout << "length: " << length << "-bit" << std::endl;
        std::cout << "Min/Max Comparing data1: " << data1 << " and data2: " << data2 << std::endl;
        std::cout << "Min: " << answer1 << " Max: " << answer2 << std::endl;

        // Clean up all pointers
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
        delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
        delete_gate_bootstrapping_ciphertext_array(length, ctxMin);
        delete_gate_bootstrapping_ciphertext_array(length, ctxMax);
    }

    // Clean up key and parameters
    delete_gate_bootstrapping_secret_keyset(key);
    delete_gate_bootstrapping_parameters(params);

    return 0;
}

