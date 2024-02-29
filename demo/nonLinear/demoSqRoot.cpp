#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>

#include <iostream>
#include <time.h>
#include <cmath>

#include "native/HomNonLinearOper.h"
#include "utils.h"

using namespace std;


int main() {

	printf("============ Step1 start ============\n");
	auto params = initializeParams(128); 
	auto key = generateKeySet(params);

	double data1 = 0.3;

	int length = 16;

	// encode double to plaintext1
	int32_t plaintext1 = encodeDouble(length, data1);

	LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key); 

	cout << "########### HomSqRoot Demo ###########" << endl;
	cout << "### Your Input Data ###" << endl;
	cout << "The length of input: " << length << endl;
	cout << "Square Root of " << data1 << ": = " << sqrt(data1) << endl;

	printf("============ Step2 start ============\n");
	LweSample* answer = new_gate_bootstrapping_ciphertext_array(length, params);

	auto start = std::chrono::high_resolution_clock::now();

	HomSqRoot(answer, ciphertext1, length, &key->cloud);
	
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	double seconds = duration.count() / 1000.0; // Convert milliseconds to seconds with decimal
	std::cout << "Time taken: " << seconds << " seconds" << std::endl;


	printf("============ Step3 start ============\n");

	//decrypt and rebuild plaintext answer
	std::vector<int> plain_answer = decryptToBinaryVector(answer, length, key); 
	double double_answer = decodeDouble(plain_answer);	

	cout << "the answer is: " << double_answer << endl;

	//clean up all pointers
	delete_gate_bootstrapping_ciphertext_array(length, ciphertext1);
	
	//clean up all pointers
	delete_gate_bootstrapping_secret_keyset(key);
	delete_gate_bootstrapping_parameters(params);

}



