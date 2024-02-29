#include <iostream>
#include <vector>
#include <cmath>

// MAPE for individual double values
double calculateMAPEScalr(double actual, double forecast) {
    if (actual == 0) {
        return 0;  // Return 0 to avoid division by zero
    }

    double error = std::abs((actual - forecast) / actual);
    return error * 100.0;  // Convert to percentage
}

// MAPE: Mean Absolute Percentage Error
double calculateMAPE(const std::vector<double>& actual, const std::vector<double>& forecast) {
    double sum = 0.0;
    int count = 0;

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != 0) {  // Avoid division by zero
            sum += std::abs((actual[i] - forecast[i]) / actual[i]);
            ++count;
        }
    }

    return (count == 0) ? 0 : (sum / count) * 100.0;  // Return 0 if count is 0 to avoid division by zero
}

// MAPE for matrix
double calculateMAPEMatrix(const std::vector<std::vector<double>>& actualMatrix, const std::vector<std::vector<double>>& forecastMatrix) {
    double sum = 0.0;
    int count = 0;

    // Check if the dimensions of both matrices are the same
    if (actualMatrix.size() != forecastMatrix.size()) {
        std::cerr << "Error: Matrices have different numbers of rows." << std::endl;
        return -1; // Error condition
    }

    for (size_t i = 0; i < actualMatrix.size(); ++i) {
        // Check if the dimensions of the current row in both matrices are the same
        if (actualMatrix[i].size() != forecastMatrix[i].size()) {
            std::cerr << "Error: Matrices have different numbers of columns in row " << i << "." << std::endl;
            return -1; // Error condition
        }

        for (size_t j = 0; j < actualMatrix[i].size(); ++j) {
            if (actualMatrix[i][j] != 0) { // Avoid division by zero
                sum += std::abs((actualMatrix[i][j] - forecastMatrix[i][j]) / actualMatrix[i][j]);
                ++count;
            }
        }
    }

    // Check if count is zero to avoid division by zero
    return (count == 0) ? 0 : (sum / count) * 100.0;
}

// Evaluation of Accuracy
double evaluateAccuracy(const std::vector<double>& homResults, const std::vector<double>& plainResults) {
    return calculateMAPE(plainResults, homResults);
}

