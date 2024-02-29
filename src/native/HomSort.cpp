#include <tfhe/tfhe.h>
#include <iostream>
#include <cmath>

#include "native/HomComp.h"
#include "native/HomMatrix.h"
#include "native/HomMinMax.h"
#include "native/HomArith.h"
#include "native/HomBitOper.h"

// Compares two ciphertexts and swaps them such that res1 contains the max and res2 contains the min
void HomCompareAndSwap(LweSample* res1, LweSample* res2, LweSample* a, LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk){
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);

    HomCompGE(&temp[0], a, b, length, bk);  // temp[0] = 1 if a >= b, else 0
    bootsNOT(&temp[1], &temp[0], bk);       // temp[1] = 1 if a < b, else 0

    for (int i = 0; i < length; i++) {
        bootsMUX(&res1[i], &temp[0], &a[i], &b[i], bk);  // res1 = max(a, b)
        bootsMUX(&res2[i], &temp[1], &a[i], &b[i], bk);  // res2 = min(a, b)
    }

    delete_gate_bootstrapping_ciphertext_array(2, temp);
}

// Bubble Sort an LweSampleVector 
LweSampleVector HomBubbleSort(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {

    int N = a.size(); 

    LweSampleVector res(N, nullptr);
    for (auto& sample : res)
        sample = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    // Perform Bubble Sort on 'a' and store in 'res'
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            // Swap to ensure res[j] <= res[j+1]
            HomCompareAndSwap(res[j], res[j + 1], a[j], a[j + 1], length, bk);
            
            // Update the original array for the next pass
            for (int k = 0; k < length; k++) {
                bootsCOPY(&a[j][k], &res[j][k], bk);      // Update a[j] to be the max of a[j] and a[j+1]
                bootsCOPY(&a[j + 1][k], &res[j + 1][k], bk);  // Update a[j+1] to be the min of a[j] and a[j+1]
            }
        }
    }
	
    return res;
}

//// Finding HomMinMax
// find min max method (1) linear: O(n)
LweSampleVector HomFindMinMax(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = a.size();   
    LweSampleVector minMax(2);

    // Initialize min and max with the first element of the array
    minMax[0] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    minMax[1] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    for (int i = 0; i < length; ++i) {
        bootsCOPY(&minMax[0][i], &a[0][i], bk);
        bootsCOPY(&minMax[1][i], &a[0][i], bk);
    }

    // Iterate over the array and update min and max
    for (int i = 1; i < N; ++i) {
        HomMin(minMax[0], minMax[0], a[i], length, bk); // Update min
        HomMax(minMax[1], minMax[1], a[i], length, bk); // Update max
    }

    return minMax; // minMax[0] is the min value, minMax[1] is the max value
}

// find min max method (2) quasi-linear: O(nlogn)
LweSampleVector HomMinMaxSort(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = a.size();
    LweSampleVector minMax(2);

    // Allocate memory for min and max
    minMax[0] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    minMax[1] = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    // Perform Bubble Sort on 'a' and store in a new sorted list
    LweSampleVector sortedList(N);
    for (int i = 0; i < N; ++i) {
        sortedList[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    }
    sortedList = HomBubbleSort(a, length, bk);

    // Assuming the sorted list is in descending order: max is the first element, min is the last element
    for (int i = 0; i < length; ++i) {
        bootsCOPY(&minMax[1][i], &sortedList[0][i], bk); // Max
        bootsCOPY(&minMax[0][i], &sortedList[N-1][i], bk); // Min
    }

    // Cleanup sortedList to avoid memory leaks
    for (int i = 0; i < N; ++i) {
        delete_gate_bootstrapping_ciphertext_array(length, sortedList[i]);
    }

    return minMax; // minMax[0] is the min value, minMax[1] is the max value
}

// wrapper function for min max
LweSampleVector HomFindMinMaxWrapper(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk, const std::string& strategy) {
    if (strategy == "linear") {
        return HomFindMinMax(a, length, bk);
    } else if (strategy == "sort") {
        return HomMinMaxSort(a, length, bk);
    } else {
        throw std::invalid_argument("Invalid strategy for finding min and max.");
    }
}

//// Range
LweSample* HomRange(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    // Find the min and max values of the vector
    LweSampleVector minMax = HomFindMinMax(a, length, bk); // Using linear strategy by default

    // Allocate memory for the range value
    LweSample* rangeVal = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    // Compute the range as max - min
    HomSubt(rangeVal, minMax[1], minMax[0], length, bk);

    // Cleanup the minMax vector to avoid memory leaks
    for (int i = 0; i < 2; ++i) {
        delete_gate_bootstrapping_ciphertext_array(length, minMax[i]);
    }

    // Return the computed range
    return rangeVal;
}

//// Median
LweSample* HomMedian(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = a.size(); 
    LweSampleVector sorted = HomBubbleSort(a, length, bk); // Sort the input vector

    LweSample* median = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    if (N % 2 == 0) {
        // For an even number of elements, average the two middle elements
        LweSample* temp = new_gate_bootstrapping_ciphertext_array(length, bk->params);
        HomAdd(temp, sorted[N / 2 - 1], sorted[N / 2], length, bk); // temp = middle element + next middle element
        HomLShift(median, temp, length, 1, bk); // median = temp / 2
        delete_gate_bootstrapping_ciphertext_array(length, temp);
    } else {
        // For an odd number of elements, select the middle element
        for (int i = 0; i < length; i++) {
            bootsCOPY(&median[i], &sorted[N / 2][i], bk);
        }
    }

    // Cleanup sorted vector
    for (auto& sample : sorted) {
        delete_gate_bootstrapping_ciphertext_array(length, sample);
    }

    return median; // Return the median value
}

LweSampleVector HomSortAscending(const LweSampleVector& sortedData, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = sortedData.size();
    LweSampleVector ascendingData(N, nullptr);

    // Allocate memory for each element in the ascendingData vector
    for (int i = 0; i < N; ++i) {
        ascendingData[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    }

    // Copy data from sortedData to ascendingData in reverse order
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < length; ++j) {
            bootsCOPY(&ascendingData[i][j], &sortedData[N - 1 - i][j], bk);
        }
    }

    return ascendingData; // Return the data in ascending order
}

//// Percentile 
LweSample* HomPercentile(const LweSampleVector& a, double p_th, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = a.size();
    
    // Handle empty input vector
    if (N == 0) {
        std::cerr << "Error: Input vector is empty." << std::endl;
        return nullptr;
    }
    
    // Sort the data in descending order
    LweSampleVector sortedDataDescending = HomBubbleSort(a, length, bk);

    // Convert the sorted data to ascending order
    LweSampleVector sortedData = HomSortAscending(sortedDataDescending, length, bk);

    // Calculate the index for the p-th percentile
    int index = static_cast<int>(ceil((p_th / 100.0) * N)) - 1;
    index = std::max(0, std::min(index, N - 1)); // Ensure index is within bounds

    // Allocate memory for the result
    LweSample* result = new_gate_bootstrapping_ciphertext_array(length, bk->params);

    // Copy the value at the calculated index to the result
    for (int i = 0; i < length; ++i) {
        bootsCOPY(&result[i], &sortedData[index][i], bk);
    }

    // Cleanup sortedData and sortedDataDescending vectors
    for (auto& sample : sortedData) {
        delete_gate_bootstrapping_ciphertext_array(length, sample);
    }
    for (auto& sample : sortedDataDescending) {
        delete_gate_bootstrapping_ciphertext_array(length, sample);
    }

    return result; // Return the p-th percentile value
}

//// Quartile
LweSampleVector HomQuartile(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk) {
    int N = a.size();
    LweSampleVector result(3, nullptr);
    // Allocate memory for quartiles
    for (int i = 0; i < 3; ++i) {
        result[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    }

    // Sort the input data in ascending order 
    LweSampleVector sortedData(N, nullptr);
    for (int i = 0; i < N; ++i) {
        sortedData[i] = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    }
    sortedData = HomBubbleSort(a, length, bk);
    sortedData = HomSortAscending(sortedData, length, bk);

    // Calculate the positions for Q1, Q2 (median), and Q3
    int posQ1 = (N + 1) / 4 - 1;  // Adjusted for 0-based indexing
    int posQ2 = (N + 1) / 2 - 1;  // Adjusted for 0-based indexing
    int posQ3 = 3 * (N + 1) / 4 - 1;  // Adjusted for 0-based indexing

    // Handle special case for even number of elements where quartiles may fall between two elements
    if (N % 2 == 0) {
        // For Q1 and Q3, check if position is not an integer, then average two nearby elements
        if ((N + 1) % 4 != 0) {
            HomAdd(result[0], sortedData[posQ1], sortedData[posQ1 + 1], length, bk);  // Average for Q1
            HomLShift(result[0], result[0], length, 1, bk);  // Divide by 2

            HomAdd(result[2], sortedData[posQ3], sortedData[posQ3 + 1], length, bk);  // Average for Q3
            HomLShift(result[2], result[2], length, 1, bk);  // Divide by 2
        } else {
            // Copy directly if position is an integer
            for (int i = 0; i < length; ++i) {
                bootsCOPY(&result[0][i], &sortedData[posQ1][i], bk);  // Q1
                bootsCOPY(&result[2][i], &sortedData[posQ3][i], bk);  // Q3
            }
        }

        // For Q2 (median), always average two middle elements for even N
        HomAdd(result[1], sortedData[posQ2], sortedData[posQ2 + 1], length, bk);
        HomLShift(result[1], result[1], length, 1, bk);  // Divide by 2

    } else {
        // For odd N, quartiles fall directly on elements
        for (int i = 0; i < length; ++i) {
            bootsCOPY(&result[0][i], &sortedData[posQ1][i], bk);  // Q1
            bootsCOPY(&result[1][i], &sortedData[posQ2][i], bk);  // Q2 (median)
            bootsCOPY(&result[2][i], &sortedData[posQ3][i], bk);  // Q3
        }
    }

    // Cleanup sorted data
    for (auto& sample : sortedData) {
        delete_gate_bootstrapping_ciphertext_array(length, sample);
    }

    return result;  // Contains Q1, Q2 (median), and Q3
}

