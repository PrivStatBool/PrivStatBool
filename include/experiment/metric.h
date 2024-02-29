#ifndef METRIC_H  
#define METRIC_H  

double calculateMAPEScalr(double actual, double forecast); 
double calculateMAPE(const std::vector<double>& actual, const std::vector<double>& forecast); 
double calculateMAPEMatrix(const std::vector<std::vector<double>>& actualMatrix, const std::vector<std::vector<double>>& forecastMatrix); 
double evaluateAccuracy(const std::vector<double>& homResults, const std::vector<double>& plainResults); 

#endif
