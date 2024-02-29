#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "native/HomBitOper.h"
#include "native/HomComp.h"

#include <omp.h>

// Implement HomAdd
void HomAddOPT(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk, int num_of_threads) {

    LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);     
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);       

    bootsCONSTANT(&c[0], 0, bk);

    omp_set_num_threads(num_of_threads); // Set the number of threads
    
    for(int i = 0; i < length -1; i++){
	#pragma omp parallel sections
	{
		#pragma omp section
		{        
			bootsXOR(&temp[0], &a[i], &b[i], bk);
		}
		#pragma omp section
		{
		        bootsAND(&temp[1], &a[i], &b[i], bk);
		}
	}
       	#pragma omp parallel sections
	{
		#pragma omp section
		{        
			bootsXOR(&res[i], &temp[0], &c[i], bk);
		}
		#pragma omp section
		{
		        bootsAND(&temp[0], &temp[0], &c[i], bk);
		}
	}
        bootsOR(&c[i+1], &temp[0], &temp[1], bk);
    }
    bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
    bootsXOR(&res[length-1], &temp[0], &c[length-1], bk);

    delete_gate_bootstrapping_ciphertext_array(length, c);    
    delete_gate_bootstrapping_ciphertext_array(2, temp);    
}


