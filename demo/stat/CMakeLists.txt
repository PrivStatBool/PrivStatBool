# Add executables
add_executable(demoSum demoSum.cpp)
add_executable(demoMean demoMean.cpp)
add_executable(demoVar demoVar.cpp)
add_executable(demoStd demoStd.cpp)
add_executable(demoConfidenceInterval demoConfidenceInterval.cpp)
add_executable(demoSkewness demoSkewness.cpp)
add_executable(demoCovariance demoCovariance.cpp)

# Link each executable to the HomStatLib library
target_link_libraries(demoSum HomStatLib)
target_link_libraries(demoMean HomStatLib)
target_link_libraries(demoVar HomStatLib)
target_link_libraries(demoStd HomStatLib)
target_link_libraries(demoConfidenceInterval HomStatLib)
target_link_libraries(demoSkewness HomStatLib)
target_link_libraries(demoCovariance HomStatLib)
