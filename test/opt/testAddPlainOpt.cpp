#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

// Function to perform XOR operation
bool XOR(bool a, bool b) {
    return a != b;
}

// Function to perform AND operation
bool AND(bool a, bool b) {
    return a && b;
}

// Function to perform OR operation
bool OR(bool a, bool b) {
    return a || b;
}

// Function to add two binary numbers without parallelization
void PlainAdd(std::vector<bool>& res, const std::vector<bool>& a, const std::vector<bool>& b) {
    bool carry = false;
    for (size_t i = 0; i < a.size(); ++i) {
        bool sum = XOR(XOR(a[i], b[i]), carry);
        carry = OR(AND(a[i], b[i]), AND(carry, XOR(a[i], b[i])));
        res[i] = sum;
    }
    
    // Handling the carry for the most significant bit
    if (carry) {
        // Handle overflow or expand the result size as needed
        // For simplicity, we just print a message here
        std::cout << "Overflow occurred" << std::endl;
    }
}

// Function to add two binary numbers with parallelization
void PlainAddParallel(std::vector<bool>& res, const std::vector<bool>& a, const std::vector<bool>& b, int num_of_threads) {
    bool carry = false;
    omp_set_num_threads(num_of_threads); // Set the number of threads

    for (size_t i = 0; i < a.size(); ++i) {
        bool temp[2];

        // First set of parallel sections for XOR and AND
        #pragma omp parallel sections
        {
            #pragma omp section
            {        
                temp[0] = XOR(a[i], b[i]);
            }
            #pragma omp section
            {
                temp[1] = AND(a[i], b[i]);
            }
        }

        bool temp2;
        // Second set of parallel sections for XOR and AND
        #pragma omp parallel sections
        {
            #pragma omp section
            {        
                res[i] = XOR(temp[0], carry);
            }
            #pragma omp section
            {
                temp2 = AND(temp[0], carry);
            }
        }

        // Sequential computation for carry
        carry = OR(temp2, temp[1]);
    }
    
    // Handling the carry for the most significant bit
    if (carry) {
        // Handle overflow or expand the result size as needed
        std::cout << "Overflow occurred" << std::endl;
    }
}

int main() {

    int num_of_threads = 8;

    // Example usage
    std::vector<bool> a = {1, 0, 0, 1}; // Binary 1001
    std::vector<bool> b = {0, 1, 1, 0}; // Binary 0110
    std::vector<bool> res(4); // Result vector
    std::vector<bool> res_parallel(4); // Result vector

    auto start = std::chrono::high_resolution_clock::now();
    // Time without Parallel
    PlainAdd(res, a, b);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    auto time_wo_parallel = duration.count();
    std::cout << "Time taken by without parallel: " << time_wo_parallel << " nanoseconds" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    // Time with Parallel
    PlainAddParallel(res_parallel, a, b, num_of_threads); // Using 2 threads for parallel sections
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    auto time_with_parallel = duration.count();
    std::cout << "Time taken by with parallel: " << time_with_parallel << " nanoseconds" << std::endl;

    //// Result of Comparison
    std::cout << "\nWith parallel processing on Plain Circuit, Speed Up Factor: " << static_cast<double>(time_wo_parallel) / time_with_parallel << std::endl;
    std::cout << "In other words, the performance decreases by: " <<   static_cast<double>(time_with_parallel) / time_wo_parallel << std::endl;



    return 0;
}

