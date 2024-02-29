#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "native/HomComp.h"


void HomMax(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* isAGreater = new_gate_bootstrapping_ciphertext(bk->params);
    
    // Compare a and b
    HomCompGE(isAGreater, a, b, length, bk);

    // MUX(res, isAGreater, a, b): if isAGreater == 1 then res = a else res = b
    for (int i = 0; i < length; i++) {
        bootsMUX(&res[i], isAGreater, &a[i], &b[i], bk);
    }

    delete_gate_bootstrapping_ciphertext(isAGreater);
}

void HomMin(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    LweSample* isALesser = new_gate_bootstrapping_ciphertext(bk->params);
    
    // Compare a and b
    HomCompLE(isALesser, a, b, length, bk);

    // MUX(res, isALesser, a, b): if isALesser == 1 then res = a else res = b
    for (int i = 0; i < length; i++) {
        bootsMUX(&res[i], isALesser, &a[i], &b[i], bk);
    }

    delete_gate_bootstrapping_ciphertext(isALesser);
}

