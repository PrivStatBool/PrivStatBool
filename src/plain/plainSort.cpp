#include <vector>
#include <algorithm> 
#include <iostream>
#include <cmath>

// Sort in the ascending order
std::vector<double> plainSortAscending(const std::vector<double>& data) {
    std::vector<double> sortedData = data; // Make a copy of the input data
    int N = sortedData.size(); // Number of elements in the vector

    // Perform Bubble Sort on 'sortedData'
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            // If the current element is less than the next element, swap them
            if (sortedData[j] > sortedData[j + 1]) {
                std::swap(sortedData[j+1], sortedData[j]);
            }
        }
    }

    return sortedData; // Return the sorted vector
}

// Sort in the descending order
std::vector<double> plainSort(const std::vector<double>& data) {
    std::vector<double> sortedData = data; // Make a copy of the input data
    int N = sortedData.size(); // Number of elements in the vector

    // Perform Bubble Sort on 'sortedData'
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            // If the current element is less than the next element, swap them
            if (sortedData[j] < sortedData[j + 1]) { // Change the comparison here
                std::swap(sortedData[j], sortedData[j + 1]);
            }
        }
    }

    return sortedData; // Return the sorted vector in descending order
}

//// minMax
std::vector<double> plainMinMax(const std::vector<double>& data) {
    if (data.empty()) return {}; // Handle empty data case

    double min_val = data[0];
    double max_val = data[0];

    // Start from the second element since the first is already considered
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] < min_val) {
            min_val = data[i];
        }
        if (data[i] > max_val) {
            max_val = data[i];
        }
    }

    return {min_val, max_val};
}

//// range
double plainRange(const std::vector<double>& data) {

    std::vector<double> minMax = plainMinMax(data); // Get min and max values

    if (minMax.empty()) return 0; // Handle empty data case

    return minMax[1] - minMax[0]; // Range = max - min
}

//// median
double plainMedian(std::vector<double> data) {
    int N = data.size();
    
    // Handle empty data case
    if (N == 0) {
        throw std::invalid_argument("Cannot compute median of an empty vector.");
    }

    // Sort the data
    std::sort(data.begin(), data.end());

    // Check if the number of elements is even or odd
    if (N % 2 == 0) {
        // Even number of elements: return the average of the two middle elements
        return (data[N / 2 - 1] + data[N / 2]) / 2.0;
    } else {
        // Odd number of elements: return the middle element
        return data[N / 2];
    }
}

//// percentile
double plainPercentile(const std::vector<double>& data, double p_th) {
    int N = data.size();

    // Handle empty input vector
    if (N == 0) {
        throw std::invalid_argument("Cannot compute percentile of an empty vector.");
    }

    std::vector<double> sortedData = plainSortAscending(data); // Sort the data

    // Calculate the index for the p-th percentile
    int index = static_cast<int>(std::ceil((p_th / 100.0) * N)) - 1;
    index = std::max(0, std::min(index, N - 1)); // Ensure index is within bounds

    // Return the value at the calculated index
    return sortedData[index];
}

std::vector<double> plainQuartile(const std::vector<double>& data) {
    int N = data.size();
    std::vector<double> result(3); // To store Q1, Median (Q2), and Q3

    // Make a copy of the data and sort it in ascending order
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    // Calculate the positions for Q1, Q2 (median), and Q3
    int posQ1 = (N + 1) / 4 - 1; // Adjusted for 0-based indexing
    int posQ2 = (N + 1) / 2 - 1; // Adjusted for 0-based indexing
    int posQ3 = 3 * (N + 1) / 4 - 1; // Adjusted for 0-based indexing

    // Handle special case for even number of elements where quartiles may fall between two elements
    if (N % 2 == 0) {
        // For Q1 and Q3, check if position is not an integer, then average two nearby elements
        if ((N + 1) % 4 != 0) {
            result[0] = (sortedData[posQ1] + sortedData[posQ1 + 1]) / 2.0; // Average for Q1
            result[2] = (sortedData[posQ3] + sortedData[posQ3 + 1]) / 2.0; // Average for Q3
        } else {
            // Copy directly if position is an integer
            result[0] = sortedData[posQ1]; // Q1
            result[2] = sortedData[posQ3]; // Q3
        }

        // For Q2 (median), always average two middle elements for even N
        result[1] = (sortedData[posQ2] + sortedData[posQ2 + 1]) / 2.0; // Median

    } else {
        // For odd N, quartiles fall directly on elements
        result[0] = sortedData[posQ1]; // Q1
        result[1] = sortedData[posQ2]; // Q2 (median)
        result[2] = sortedData[posQ3]; // Q3
    }

    return result; // Contains Q1, Q2 (median), and Q3
}

