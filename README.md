# PrivStatBool: Privacy-Preserving Statistical Analysis Framework

PrivStatBool leverages the TFHE library for performing statistical analyses on encrypted data, ensuring data privacy.

## Installation Guide

## Prerequisites

Before installing PrivStatBool and TFHE, ensure your system meets the following requirements:

- **C++ Compiler:** A compiler supporting C++11 standards, such as g++ (version 5.2 or higher) or clang (version 3.8 or higher). This is necessary for compiling the core TFHE library and PrivStatBool.
  
- **OpenMP:** For parallel computation, PrivStatBool utilizes OpenMP. Ensure your C++ compiler supports OpenMP. Most modern compilers like g++ and clang come with OpenMP support. To install OpenMP on Debian-based systems, use the following command:
  ```shell
  sudo apt-get install libomp-dev

### TFHE Installation

1. **Install Compilers:**
   ```shell
   sudo apt-get update
   sudo apt-get install g++ clang

2. **Clone TFHE:**
   ```shell
   git clone https://github.com/tfhe/tfhe.git

3. **Build TFHE:**
- Navigate to the TFHE directory and build:
   ```shell
  cd tfhe
  mkdir build && cd build
  ccmake ..

4. **Set Environment Variables:**
- Add the following to ~/.bashrc (adjust TFHE_PREFIX if necessary):
   ```shell
  export C_INCLUDE_PATH=$C_INCLUDE_PATH:/usr/local/include
  export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/include
  export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

- Reload ~/.bashrc:
   ```shell
   source ~/.bashrc

### PrivStatBool Installation

1. **Clone PrivStatBool:**
   ```shell
   git clone https://github.com/PrivStatBool/PrivStatBool.git

2. **Build PrivStatBool:**
- Navigate to the PrivStatBool directory and build:
   ```shell
   cd PrivStatBool
   mkdir build && cd build
   make

### Running Demos and Tests
#### Demos
- Navigate to the respective demo directory under build/demo and execute the binary. Example for demoAdd:
   ```shell
   cd build/demo/arith
   ./demoAdd

#### Tests
- Navigate to the test directory under build/test and execute the desired test binary. Example for testHomBitOper:
   ```shell
   cd build/test/basic
   ./testHomBitOper

### Available Demos and Tests
#### Demos
- Arithmetic Operations: demoAdd, demoSub, demoMult, demoDiv
- Basic Operations: demoMinMax
- Non-Linear Operations: demoSqRoot
- Statistical Analysis: demoSum, demoMean, demoVar, demoStd, demoConfidenceInterval, demoSkewness, demoCovariance
- Sorting and Order Statistics: demoSort, demoFindMinMax, demoRange, demoMedian, demoPercentile, demoQuartile

#### Tests
- Basic Operations: testHomBitOper, testComp, timeComp, timeBitOper, timeBitOperAbsTwos, timeMinMax
- Arithmetic Operations: timeArith
- Non-Linear Operations: timeNonLinear
- Optimization: testAddPlainOpt, testAddOpt



