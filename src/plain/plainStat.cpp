#include <vector>
#include <cmath>
#include "plain/plainSort.h"
#include <iostream>


std::vector<double> plainSum(const std::vector<std::vector<double>>& dataset) {
    if (dataset.empty()) return {};

    int rowNum = dataset.size();
    int colNum = dataset[0].size();
    std::vector<double> sum(colNum, 0.0);

    for (int i = 0; i < colNum; i++) {
        for (int j = 0; j < rowNum; j++) {
            sum[i] += dataset[j][i];
        }
    }

    return sum;
}

std::vector<double> plainMean(const std::vector<std::vector<double>>& dataset) {
    std::vector<double> sum = plainSum(dataset); // Reuse plainSum to calculate column sums

    if (dataset.empty()) return {};

    int rowNum = dataset.size(); 

    // Multiply each sum by the inverse of the number of rows to get the mean
    for (double& value : sum) {
        value /= rowNum;
    }

    return sum; 
}

// Function to calculate the variance of elements in each column
std::vector<double> plainVar(const std::vector<std::vector<double>>& dataset) {
    if (dataset.empty()) return {};

    int rowNum = dataset.size();
    int colNum = dataset[0].size();

    std::vector<double> mean = plainMean(dataset); // Reuse plainMean to calculate column means
    std::vector<double> var(colNum, 0.0);

    for (int i = 0; i < colNum; ++i) {
        for (int j = 0; j < rowNum; ++j) {
            double diff = dataset[j][i] - mean[i]; // Difference from the mean
            var[i] += diff * diff; // Squaring the difference and summing up
        }
        var[i] /= rowNum; // Dividing by N to get the variance
    }

    return var;
}

std::vector<double> plainStd(const std::vector<std::vector<double>>& dataset) {

    if (dataset.empty()) return {};
    
    int colNum = dataset[0].size();
    std::vector<double> std(colNum, 0.0);
    std = plainVar(dataset);
    
    for (auto& stdElt : std)
        stdElt = sqrt(stdElt);
    
    return std;
}

std::vector<std::vector<double>> plainConfidenceInterval(const std::vector<std::vector<double>>& dataset, double Z) {
    if (dataset.empty()) return {};

    int colNum = dataset[0].size();
    int rowNum = dataset.size();
    std::vector<double> mean = plainMean(dataset);
    std::vector<double> stdDev = plainStd(dataset);

    std::vector<std::vector<double>> confidenceInterval(colNum, std::vector<double>(2, 0.0)); // 2 for lower and upper bounds

    // Set the zScore based on the confidence level
    double zScore = (Z == 95.0) ? 1.96 : 2.576;

    for (int i = 0; i < colNum; ++i) {
        double marginError = zScore * (stdDev[i] / sqrt(rowNum));
        confidenceInterval[i][0] = mean[i] - marginError; // Lower bound
        confidenceInterval[i][1] = mean[i] + marginError; // Upper bound
    }

    return confidenceInterval;

}

std::vector<double> plainSkewness(const std::vector<std::vector<double>>& dataset) {
     std::vector<double> mean = plainMean(dataset);
     std::vector<double> stdDev = plainStd(dataset);  

     int colNum = dataset[0].size();
     std::vector<double> skewness(colNum, 0.0);
 
     for (int i = 0; i < colNum; ++i) {
         std::vector<double> columnData;  // Extract column data
         for (const auto& row : dataset) {
             columnData.push_back(row[i]);
         }
         double median = plainMedian(columnData);  // Calculate median for the current column	
         skewness[i] = 3.0 * (mean[i] - median) / stdDev[i];
     }
 
    return skewness;
}

double plainCovariance(const std::vector<std::vector<double>>& dataset) {

    int N = dataset.size();
    std::vector<double> mean = plainMean(dataset);

    double cov = 0.0;
    for (size_t i = 0; i < N; ++i) {
        cov += (dataset[i][0] - mean[0]) * (dataset[i][1] - mean[1]);
    }
    return cov / (dataset.size() - 1);
}

