#ifndef PLAINSTAT_H
#define PLAINSTAT_H

std::vector<double> plainSum(const std::vector<std::vector<double>>& dataset); 
std::vector<double> plainMean(const std::vector<std::vector<double>>& dataset); 
std::vector<double> plainVar(const std::vector<std::vector<double>>& dataset); 
std::vector<double> plainStd(const std::vector<std::vector<double>>& dataset); 
std::vector<std::vector<double>> plainConfidenceInterval(const std::vector<std::vector<double>>& dataset, double Z); 
std::vector<double> plainSkewness(const std::vector<std::vector<double>>& dataset); 
double plainCovariance(const std::vector<std::vector<double>>& dataset); 

#endif
