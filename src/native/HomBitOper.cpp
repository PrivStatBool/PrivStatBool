#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>


void HomLShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk) {

	for(int i = 0; i < length - k; i++){
		bootsCOPY(&res[i], &a[i+k], bk);}
	for(int i = length-k; i < length; i++){
		bootsCOPY(&res[i], &a[length-1], bk);}
}


void HomRShift(LweSample* res, const LweSample* a, const int length, const int k, const TFheGateBootstrappingCloudKeySet* bk) {

    bootsCOPY(&res[length-1], &a[length-1], bk);
    for(int i = length-2; i > k-1; i--){
        bootsCOPY(&res[i], &a[i-k], bk);}
    for(int i = 0; i < k; i++){
        bootsCONSTANT(&res[i], 0, bk);}
} 


void HomTwosComplement(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
	LweSample* temp = new_gate_bootstrapping_ciphertext_array(1, bk->params);	    
	LweSample* b = new_gate_bootstrapping_ciphertext_array(length, bk->params);	    

	bootsCONSTANT(&b[0], 1, bk);
    

	for(int i = 0; i < length - 2; i++){
		bootsNOT(&temp[0], &a[i], bk);
		bootsXOR(&res[i], &temp[0], &b[i], bk);
		bootsAND(&b[i+1], &temp[0], &b[i], bk);}

	bootsNOT(&temp[0], &a[length-2], bk);
	bootsXOR(&res[length-2], &temp[0], &b[length-2], bk);

	bootsNOT(&res[length-1], &a[length-1], bk);
	

	delete_gate_bootstrapping_ciphertext_array(length, b);    
	delete_gate_bootstrapping_ciphertext_array(1, temp);    
}


void HomAbs(LweSample* res, const LweSample* a, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

	LweSample* na = new_gate_bootstrapping_ciphertext_array(length, bk->params);

	HomTwosComplement(na, a, length, bk);
	
	for(int i = 0; i < length; i++){
		bootsMUX(&res[i], &a[length-1], &na[i], &a[i], bk);}

	delete_gate_bootstrapping_ciphertext_array(length, na);
}

