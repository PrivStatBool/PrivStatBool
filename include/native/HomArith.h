// HomArith.h
#ifndef HOM_ARITH_H
#define HOM_ARITH_H


// Arithmetic Operations Declarations
void HomAdd(LweSample* result, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk);
void HomSubt(LweSample* result, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk);
void HomMult(LweSample* result, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk);
void HomDiv(LweSample* result, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk);


#endif 

