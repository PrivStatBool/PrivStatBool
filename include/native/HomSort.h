#ifndef HOMSORT_H
#define HOMSORT_H

#include "native/HomMatrix.h"

void HomCompareAndSwap(LweSample* res1, LweSample* res2, LweSample* a, LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk);
LweSampleVector HomBubbleSort(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk); 

LweSampleVector HomFindMinMax(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomMinMaxSort(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomFindMinMaxWrapper(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk, const std::string& strategy); 

LweSample* HomRange(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSample* HomMedian(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk);
LweSample* HomPercentile(const LweSampleVector& a, double p_th, int length, const TFheGateBootstrappingCloudKeySet* bk); 
LweSampleVector HomQuartile(const LweSampleVector& a, int length, const TFheGateBootstrappingCloudKeySet* bk); 

 
#endif
