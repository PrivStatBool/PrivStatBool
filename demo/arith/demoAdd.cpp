#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include <iostream>
#include <time.h>

#include "native/HomArith.h"
#include "utils.h"

using namespace std;


void testAdd(int length) {

	printf("============ Step1 start ============\n");
	auto params = initializeParams(128); 
	auto key = generateKeySet(params);

	double data1 = 1.1, data2 = 3.4;

	// encode double to plaintext1
	int32_t plaintext1 = encodeDouble(length, data1);
	int32_t plaintext2 = encodeDouble(length, data2);

	LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key); 
	LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key); 

	cout << "########### HomAdd Demo ###########" << endl;
	cout << "### Your Input Data ###" << endl;
	cout << "Add " << data1 << " with " << data2 << endl;

	printf("============ Step2 start ============\n");
	LweSample* answer = new_gate_bootstrapping_ciphertext_array(length, params);

	std::cout << "Time for HomAdd: " << measureTime(HomAdd, answer, ciphertext1, ciphertext2, length, &key->cloud) << " seconds" << std::endl;

	printf("============ Step3 start ============\n");

	//decrypt and rebuild plaintext answer
	std::vector<int> plain_answer = decryptToBinaryVector(answer, length, key); 
	double double_answer = decodeDouble(plain_answer);	

	cout << "the answer is: " << double_answer << endl;

	//clean up all pointers
	delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
	delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
	
	//clean up all pointers
	delete_gate_bootstrapping_secret_keyset(key);
	delete_gate_bootstrapping_parameters(params);

}


int main() {
	
	// tests based on different input lengths
	testAdd(16);
	return 0;
}
