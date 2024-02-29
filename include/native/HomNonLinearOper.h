#ifndef HOMNONLINEAROPER_H
#define HOMNONLINEAROPER_H

void bitwiseAND(LweSample* res, const LweSample* bitA, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
void bitwiseXOR(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
void HomSqRoot(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk); 

#endif 
