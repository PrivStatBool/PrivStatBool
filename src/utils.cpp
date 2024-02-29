#include <iostream>
#include <math.h>
#include <chrono>
#include <functional>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>


// initialization
TFheGateBootstrappingParameterSet* initializeParams(int minimum_lambda) {

	return new_default_gate_bootstrapping_parameters(minimum_lambda);

}

TFheGateBootstrappingSecretKeySet* generateKeySet(TFheGateBootstrappingParameterSet* params) {

	uint32_t seed[] = { 314, 1592, 657 };
	tfhe_random_generator_setSeed(seed,3);
	return new_random_gate_bootstrapping_secret_keyset(params);
}

int32_t encodeDouble(int length, double data) {
    if (length % 2 != 0) {
        std::cerr << "Length must be even for the specified bit allocation." << std::endl;
        return 0;
    }

    // Determine the number of bits allocated for the fractional part
    int fractionalBits = length / 2 - 1;
    double scale = pow(2.0, fractionalBits);

    // Scale and round the data to fit the fractional part
    double scaledData = round(data * scale);

    // Convert the scaled data to a fixed-point representation
    int32_t encodedData;
    if (scaledData >= 0) {
        encodedData = static_cast<int32_t>(scaledData);
    } else {
        // For negative numbers, use two's complement within the allocated bits excluding the sign bit
        encodedData = static_cast<int32_t>(pow(2, length - 1) + scaledData);
    }

    // Ensure the encoded data fits within the allocated length (including the sign bit)
    encodedData &= (1 << (length - 1)) - 1;

    // Set the sign bit if the original data was negative
    if (data < 0) {
        encodedData |= 1 << (length - 1);
    }

    return encodedData;
}

// encode plain to plaintext not ciphertext 
LweSample* encodeDoublePlain(int length, double data, const TFheGateBootstrappingCloudKeySet* bk) {
    if (length % 2 != 0) {
        std::cerr << "Length must be even for the specified bit allocation." << std::endl;
        return nullptr;
    }

    // Determine the number of bits allocated for the fractional part
    int fractionalBits = length / 2 - 1;
    double scale = pow(2.0, fractionalBits);

    // Scale and round the data to fit the fractional part
    double scaledData = round(data * scale);

    // Convert the scaled data to a fixed-point representation
    int32_t encodedData;
    if (scaledData >= 0) {
        encodedData = static_cast<int32_t>(scaledData);
    } else {
        // For negative numbers, use two's complement within the allocated bits excluding the sign bit
        encodedData = static_cast<int32_t>(pow(2, length - 1) + scaledData);
    }

    // Ensure the encoded data fits within the allocated length (including the sign bit)
    encodedData &= (1 << (length - 1)) - 1;

    // Set the sign bit if the original data was negative
    if (data < 0) {
        encodedData |= 1 << (length - 1);
    }

    // Initialize an array of LweSamples
    LweSample* noiselessEncoding = new_gate_bootstrapping_ciphertext_array(length, bk->params);
    for (int i = 0; i < length; i++) {
        // Get the i-th bit of encodedData and convert it to Torus32
        int bit = (encodedData >> i) & 1;
        Torus32 torusBit = bit ? modSwitchToTorus32(1, 8) : modSwitchToTorus32(0, 8);

        // Use lweNoiselessTrivial to set the value of the i-th LweSample
        lweNoiselessTrivial(&noiselessEncoding[i], torusBit, bk->params->in_out_params);
    }

    return noiselessEncoding;
}

double decodeDouble(const std::vector<int>& binaryVector) {
    int length = binaryVector.size();
    bool isNegative = binaryVector[length - 1] == 1;  // Check sign bit
    int32_t value = 0;
    double result;

    // Two's complement conversion if negative
    if (isNegative) {
        // Convert from two's complement to magnitude
        for (int i = 0; i < length - 1; i++) {  // Skip sign bit
            value += (!binaryVector[i]) << i;  // Invert bits and accumulate
        }
        value = (value + 1) & ((1 << (length - 1)) - 1);  // Add 1 and mask to length - 1 bits
    } else {
        // Directly accumulate value for positive numbers
        for (int i = 0; i < length - 1; i++) {  // Skip sign bit
            value += binaryVector[i] << i;
        }
    }

    // Scale back and apply sign
    result = value / pow(2.0, (length / 2) - 1);  // Assuming (length / 2) - 1 fractional bits
    if (isNegative) {
        result = -result;
    }

    return result;
}

// decode plaintext to double
double decodeDoublePlain(const LweSample* noiselessEncoding, int length) {
    std::vector<int> binaryVector(length);
    double scale = pow(2.0, (length / 2) - 1);  // Scale factor for decoding

    // Convert the LweSamples back to binary representation
    for (int i = 0; i < length; i++) {
        Torus32 sampleValue = noiselessEncoding[i].b;  // Extract the Torus32 value
        // Map Torus32 value to binary. We assume 1/8 represents 1 and -1/8 represents 0.
        binaryVector[i] = (sampleValue > 0) ? 1 : 0;
    }

    // Decode the binary vector to an integer value, considering two's complement for negative numbers
    int32_t value = 0;
    bool isNegative = binaryVector[length - 1] == 1;  // Check the sign bit

    if (isNegative) {
        // Convert from two's complement to magnitude for negative numbers
        for (int i = 0; i < length - 1; i++) {  // Skip sign bit
            value += (!binaryVector[i]) << i;  // Invert bits and accumulate
        }
        value = -(1 << (length - 1)) + value;  // Convert to negative value
    } else {
        // Accumulate value for positive numbers
        for (int i = 0; i < length - 1; i++) {  // Skip sign bit
            value += binaryVector[i] << i;
        }
    }

    // Scale back the integer value to a double
    double result = value / scale;
    return result;
}

//// encryption
LweSample* encryptBoolean(int32_t plaintext, int length, const TFheGateBootstrappingParameterSet* params, const TFheGateBootstrappingSecretKeySet* key) {

	LweSample* ciphertext = new_gate_bootstrapping_ciphertext_array(length, params);
	for (int i = 0; i < length; i++) 
		bootsSymEncrypt(&ciphertext[i], (plaintext>>i)&1, key);
	return ciphertext;

}


//// decryption
// decrypt to binary vector
std::vector<int> decryptToBinaryVector(const LweSample* ciphertext, int length, const TFheGateBootstrappingSecretKeySet* key) {
    std::vector<int> binaryVector(length);
    for(int i = 0; i < length; i++) {
        binaryVector[i] = bootsSymDecrypt(&ciphertext[i], key);
    }
    return binaryVector;

}

//// print out
void print_double_data(double * data_double, int num_data){

	std::cout << "( ";
	for(int i=0; i<num_data; i++)
	std::cout << data_double[i] << " ";
	std::cout << ")" << std::endl;

}

//// load data
std::vector<double> loadDataFromCSV(const std::string& filename) {
    std::vector<double> data;
    std::string line;
    std::ifstream file(filename);

    if (file.is_open()) {
        while (std::getline(file, line)) {
            std::stringstream linestream(line);
            double value;
            if (linestream >> value) {
                data.push_back(value);
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return data;
}

// load partial data taking a matrix
std::vector<std::vector<double>> loadPartialDataFromCSV(const std::string& filename, int rowNum, int colNum) {
    std::vector<std::vector<double>> data;
    std::string line;
    std::ifstream file(filename);
    int rowCount = 0;

    if (file.is_open()) {
        while (rowCount < rowNum && std::getline(file, line)) {
            std::stringstream linestream(line);
            std::vector<double> rowData;
            std::string cell;
            int colCount = 0;

            while (colCount < colNum && std::getline(linestream, cell, ',')) {
                try {
                    double value = std::stod(cell);
                    rowData.push_back(value);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Invalid argument: " << e.what() << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Out of range error: " << e.what() << std::endl;
                }
                colCount++;
            }

            data.push_back(rowData);
            rowCount++;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return data;
}

//// random number generator
double generateRandomDouble(double min, double max) {
    // Create a random device and use it to seed a Mersenne Twister engine.
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    // Create a distribution in the range [min, max].
    std::uniform_real_distribution<> dis(min, max);

    // Use the distribution and the generator to produce a random value.
    return dis(gen);
}
