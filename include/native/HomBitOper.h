#ifndef HOMBITOPER_H
#define HOMBITOPER_H


void HomLShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk); 
void HomRShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk); 
void HomTwosComplement(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
void HomAbs(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk); 


#endif

