cmake_minimum_required(VERSION 3.10) # Specify the minimum version of CMake

project(HomStatProject) # Name your project

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add the library with all related source files
add_library(HomStatLib src/experiment/metric.cpp src/experiment/experiFunc.cpp  src/plain/plainStat.cpp src/plain/plainSort.cpp src/utils.cpp src/utilServer.cpp src/utilMat.cpp src/native/HomArith.cpp src/native/HomBitOper.cpp src/native/HomComp.cpp src/native/HomMinMax.cpp src/native/HomNonLinearOper.cpp src/native/HomStatBasic.cpp src/native/HomSort.cpp src/optimized/HomArithOPT.cpp)

# Find the TFHE library
find_library(TFHE_LIB tfhe-spqlios-fma)

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    # Link the library with TFHE and OpenMP
    target_link_libraries(HomStatLib PUBLIC ${TFHE_LIB} OpenMP::OpenMP_CXX)
else()
    # Link the library with only TFHE if OpenMP not found
    target_link_libraries(HomStatLib PUBLIC ${TFHE_LIB})
endif()

# Add subdirectories for tests or demos
add_subdirectory(test)
add_subdirectory(demo)
