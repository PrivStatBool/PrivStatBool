#include <tfhe/tfhe.h>

// converts integer to double representation of Lwe Sample
void HomIntP2C(LweSample* res, const int num, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    // Convert the integer to a two's complement representation if it's negative
    uint32_t plain = (num < 0) ? (~(-num) + 1) : num;  // Handle two's complement for negative numbers

    // Set fractional part to 0
    for (int i = 0; i < (length / 2) - 1; i++)
        bootsCONSTANT(&res[i], 0, bk);

    // Encode the integer part, adjusting for the fractional part and leaving space for the sign bit
    for (int i = 0; i < (length / 2); i++)
        bootsCONSTANT(&res[i + (length / 2) - 1], (plain >> i) & 1, bk);

    // Set the sign bit, located at the last position of the array
    bootsCONSTANT(&res[length - 1], (num < 0) ? 1 : 0, bk);  // Set the sign bit based on the number's sign
}

// converts double to double representation of Lwe Sample
void HomDoubleP2C(LweSample* res, const double num, const int length, const TFheGateBootstrappingCloudKeySet* bk) {
    // Determine the number of bits allocated for the fractional part
    int fractionalBits = length / 2 - 1;
    double scale = pow(2.0, fractionalBits);

    // Scale the number to convert the fractional part to an integer representation
    int32_t scaledNum = (int32_t)(num * scale); // Directly cast to int32_t to round down

    // Handle two's complement for negative numbers
    uint32_t encodedNum = (scaledNum < 0) ? (~(-scaledNum) + 1) : (uint32_t)scaledNum;

    // Encode both integer and fractional parts together
    for (int i = 0; i < length - 1; i++) {
        bootsCONSTANT(&res[i], (encodedNum >> i) & 1, bk);
    }

    // Set the sign bit
    int signBit = (num < 0) ? 1 : 0;
    bootsCONSTANT(&res[length - 1], signBit, bk);
}

