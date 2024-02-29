#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include "utils.h"
#include "optimized/HomArithOPT.h"
#include "native/HomArith.h"
#include <iostream>

using namespace std;

int main() {

	printf("============ Step1 start ============\n");
	auto params = initializeParams(128); 
	auto key = generateKeySet(params);
	int length = 16;
	int num_of_threads = 8;

	double data1 = 1.1, data2 = 3.4;

	// encode double to plaintext1
	int32_t plaintext1 = encodeDouble(length, data1);
	int32_t plaintext2 = encodeDouble(length, data2);

	LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key); 
	LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key); 

	cout << "########### HomAdd Parallel Test ###########" << endl;
	cout << "### Your Input Data ###" << endl;
	cout << "Add " << data1 << " with " << data2 << endl;
	cout << "Parallel Compuation on HomAddOPT" << endl;

	printf("============ Step2 start ============\n");
	LweSample* answer = new_gate_bootstrapping_ciphertext_array(length, params);
	LweSample* answer2 = new_gate_bootstrapping_ciphertext_array(length, params);
	
	std::cout << "Time for HomAddOPT: " << measureTime(HomAddOPT, answer, ciphertext1, ciphertext2, length, &key->cloud, num_of_threads) << " seconds" << std::endl;
	std::cout << "Time for HomAdd: " << measureTime(HomAdd, answer2, ciphertext1, ciphertext2, length, &key->cloud) << " seconds" << std::endl;
	
	printf("============ Step3 start ============\n");

	//decrypt and rebuild plaintext answer
	std::vector<int> plain_answer = decryptToBinaryVector(answer, length, key); 
	double double_answer = decodeDouble(plain_answer);	

	cout << "the answer for HomAddOPT is: " << double_answer << endl;

	//decrypt and rebuild plaintext answer
	std::vector<int> plain_answer2 = decryptToBinaryVector(answer2, length, key); 
	double double_answer2 = decodeDouble(plain_answer2);	

	cout << "the answer for HomAdd is: " << double_answer2 << endl;


	//clean up all pointers
	delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
	delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
	delete_gate_bootstrapping_ciphertext_array(length, answer);
	delete_gate_bootstrapping_ciphertext_array(length, answer2);
	
	//clean up all pointers
	delete_gate_bootstrapping_secret_keyset(key);
	delete_gate_bootstrapping_parameters(params);

	return 0;

}

