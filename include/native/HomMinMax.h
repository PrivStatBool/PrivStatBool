#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "HomComp.h"


void HomMax(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk); 
void HomMin(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk); 

