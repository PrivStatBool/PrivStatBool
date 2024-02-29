#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "native/HomBitOper.h"
#include "native/HomComp.h"


// Implement HomAdd
void HomAdd(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

    LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);     
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);       

    bootsCONSTANT(&c[0], 0, bk);
    
    for(int i = 0; i < length -1; i++){
        bootsXOR(&temp[0], &a[i], &b[i], bk);
        bootsAND(&temp[1], &a[i], &b[i], bk);
        bootsXOR(&res[i], &temp[0], &c[i], bk);
        bootsAND(&temp[0], &temp[0], &c[i], bk);
        bootsOR(&c[i+1], &temp[0], &temp[1], bk);
    }

    bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
    bootsXOR(&res[length-1], &temp[0], &c[length-1], bk);

    delete_gate_bootstrapping_ciphertext_array(length, c);    
    delete_gate_bootstrapping_ciphertext_array(2, temp);    
}

// Implement HomSubt
void HomSubt(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {

    LweSample* c = new_gate_bootstrapping_ciphertext_array(length, bk->params);       
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);       

    bootsCONSTANT(&c[0], 0, bk);
    
    for(int i = 0; i < length -1; i++){
        bootsXOR(&temp[0], &a[i], &b[i], bk);
        bootsANDNY(&temp[1], &a[i], &b[i], bk);
        bootsXOR(&res[i], &temp[0], &c[i], bk);
        bootsANDNY(&temp[0], &temp[0], &c[i], bk);
        bootsOR(&c[i+1], &temp[1], &temp[0], bk);}

    bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
    bootsXOR(&res[length-1], &temp[0], &c[length-1], bk);

    delete_gate_bootstrapping_ciphertext_array(length, c);   
    delete_gate_bootstrapping_ciphertext_array(2, temp);   
} 

// Implement HomMult
void HomMult(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    
    LweSample* signPositiveXOR = new_gate_bootstrapping_ciphertext(bk->params);
    LweSample* signNegativeXOR = new_gate_bootstrapping_ciphertext(bk->params);
    LweSample* absA = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* interSum = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* absB = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* shiftedAbsA = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* bitwiseAndResult = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* positiveResult = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* negativeResult = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    
    HomAbs(absA, a, length, bk);
    HomAbs(absB, b, length, bk);
    for(int i = 0; i < length; i++) {
        bootsAND(&interSum[i], &absA[i], &absB[0], bk);
    }
    HomLShift(interSum, interSum, length, length/2, bk);    

    for(int i = 1; i < length-1; i++) {
        if(i < length/2 - 1) {
            HomLShift(shiftedAbsA, absA, length, length/2-1-i, bk);

            for(int j = 0; j < length; j++) {
                bootsAND(&bitwiseAndResult[j], &shiftedAbsA[j], &absB[i], bk);
            }

            HomAdd(interSum, interSum, bitwiseAndResult, length, bk);
        }
        else if(i == length/2 - 1) {
            for(int j = 0; j < length; j++) {
                bootsAND(&bitwiseAndResult[j], &absA[j], &absB[i], bk);
            }

            HomAdd(interSum, interSum, bitwiseAndResult, length, bk);
        }
        else {
            HomRShift(shiftedAbsA, absA, length, i-length/2+1, bk);
            for(int j = 0; j < length; j++) {
                bootsAND(&bitwiseAndResult[j], &shiftedAbsA[j], &absB[i], bk);
            }

            HomAdd(interSum, interSum, bitwiseAndResult, length, bk);
        }
    }
    bootsCONSTANT(&interSum[length-1], 0, bk);

    HomTwosComplement(negativeResult, interSum, length, bk);
    
    bootsXOR(signNegativeXOR, &a[length-1], &b[length-1], bk);
    bootsNOT(signPositiveXOR, signNegativeXOR, bk);

    for(int i = 0; i < length; i++) {
        bootsAND(&positiveResult[i], &interSum[i], signPositiveXOR, bk);
        bootsAND(&negativeResult[i], &negativeResult[i], signNegativeXOR, bk);
    }

    HomAdd(res, positiveResult, negativeResult, length, bk);

    // Cleanup
    delete_gate_bootstrapping_ciphertext(signPositiveXOR);
    delete_gate_bootstrapping_ciphertext(signNegativeXOR);
    delete_gate_bootstrapping_ciphertext_array(length, absA);
    delete_gate_bootstrapping_ciphertext_array(length, interSum);
    delete_gate_bootstrapping_ciphertext_array(length, absB);
    delete_gate_bootstrapping_ciphertext_array(length, shiftedAbsA);
    delete_gate_bootstrapping_ciphertext_array(length, bitwiseAndResult);
    delete_gate_bootstrapping_ciphertext_array(length, positiveResult);
    delete_gate_bootstrapping_ciphertext_array(length, negativeResult);
}

// Implement HomDiv
void HomDiv(LweSample* res, const LweSample* a, const LweSample* b, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    
    LweSample* temp = new_gate_bootstrapping_ciphertext_array(2, bk->params);
    LweSample* absA = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* absB = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* quotRem = new_gate_bootstrapping_ciphertext_array(2*length, bk->params);
    LweSample* divisorCopy = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* twosCompQuot = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* quotient = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* tempDivisor = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    LweSample* remainder = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    
    HomAbs(absA, a, length, bk);
    HomAbs(absB, b, length, bk);

    for(int i = 0; i < length; i++) {
        bootsCOPY(&quotRem[i], &absA[i], bk);
        bootsCOPY(&divisorCopy[i], &absB[i], bk);
        bootsCONSTANT(&quotRem[length + i], 0, bk);
    }
    
    HomRShift(quotRem, quotRem, 2*length, 1, bk);

    for(int s = 1; s < length; s++){
        HomRShift(quotRem, quotRem, 2*length, 1, bk);

        for(int i = 0; i < length; i++)
            bootsCOPY(&remainder[i], &quotRem[length+i], bk);

        HomCompLE(&temp[0], remainder, divisorCopy, length, bk);
        bootsNOT(&temp[1], &temp[0], bk);
        bootsCOPY(&quotRem[0], &temp[1], bk);

        for(int i = 0; i < length; i++){
            bootsAND(&tempDivisor[i], &divisorCopy[i], &temp[1], bk);}

        HomSubt(remainder, remainder, tempDivisor, length, bk);

        for(int i = 0; i < length; i++)
            bootsCOPY(&quotRem[length+i], &remainder[i], bk);
        
    }
    

    for(int s = length; s < length*3/2-1; s++){
        HomRShift(quotRem, quotRem, 2*length, 1, bk);
        HomRShift(remainder, remainder, length, 1, bk);

        HomCompLE(&temp[0], remainder, divisorCopy, length, bk);
        bootsNOT(&temp[1], &temp[0], bk);
        bootsCOPY(&quotRem[0], &temp[1], bk);

        for(int i = 0; i < length; i++){
            bootsAND(&tempDivisor[i], &divisorCopy[i], &temp[1], bk);}

        HomSubt(remainder, remainder, tempDivisor, length, bk);

    }


    for(int i = 0; i < length; i++)
        bootsCOPY(&quotient[i], &quotRem[i], bk);


    HomTwosComplement(twosCompQuot, quotient, length, bk);
    
    bootsXOR(&temp[0], &a[length-1], &b[length-1], bk);
    for (int i = 0; i < length; i++){
        bootsMUX(&res[i], &temp[0], &twosCompQuot[i], &quotient[i], bk);
    }
    
    
    delete_gate_bootstrapping_ciphertext_array(3, temp);
    delete_gate_bootstrapping_ciphertext_array(length, absA);
    delete_gate_bootstrapping_ciphertext_array(length, absB);
    delete_gate_bootstrapping_ciphertext_array(2*length, quotRem);
    delete_gate_bootstrapping_ciphertext_array(length, divisorCopy);
    delete_gate_bootstrapping_ciphertext_array(length, twosCompQuot);
    delete_gate_bootstrapping_ciphertext_array(length, quotient);
    delete_gate_bootstrapping_ciphertext_array(length, tempDivisor);
    delete_gate_bootstrapping_ciphertext_array(length, remainder);
}

