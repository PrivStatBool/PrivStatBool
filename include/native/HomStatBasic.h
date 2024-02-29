#ifndef HOMSTATBASIC_H
#define HOMSTATBASIC_H

LweSampleVector HomSum(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomMean(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomVar(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomStd(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleMatrix HomConfInter(const LweSampleMatrix& encData, double confidenceLevel, int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomSkewness(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSample* HomCovariance(const LweSampleMatrix& encData, const int length, const TFheGateBootstrappingCloudKeySet* bk); 

#endif
