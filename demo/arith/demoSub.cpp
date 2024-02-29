#include <tfhe/tfhe.h>
#include <iostream>
#include "native/HomArith.h"
#include "utils.h"

using namespace std;


int main() {

  printf("============ Step1 start ============\n");
  auto params = initializeParams(128); 
  auto key = generateKeySet(params);
  
  double data1 = 1.1, data2 = 3.4;
  
  int length = 16;
  
  // encode double to plaintext1
  int32_t plaintext1 = encodeDouble(length, data1);
  int32_t plaintext2 = encodeDouble(length, data2);
  
  LweSample* ciphertext1 = encryptBoolean(plaintext1, length, params, key); 
  LweSample* ciphertext2 = encryptBoolean(plaintext2, length, params, key); 
  
  cout << "########### HomSubt Demo ###########" << endl;
  cout << "### Your Input Data ###" << endl;
  cout << "The length of input: " << length << endl;
  cout << "Subt: " << data1 << " - " << data2 << " = " << data1 - data2 << endl;
  
  printf("============ Step2 start ============\n");
  LweSample* answer = new_gate_bootstrapping_ciphertext_array(length, params);
  
  auto start = std::chrono::high_resolution_clock::now();
  
  HomSubt(answer, ciphertext1, ciphertext2, length, &key->cloud);
  
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
  delete_gate_bootstrapping_ciphertext_array(length, ciphertext2);
  
  //clean up all pointers
  delete_gate_bootstrapping_secret_keyset(key);
  delete_gate_bootstrapping_parameters(params);

}



