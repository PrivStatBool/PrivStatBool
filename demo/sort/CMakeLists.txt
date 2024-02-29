# Add executables
add_executable(demoSort demoSort.cpp)
add_executable(demoFindMinMax demoFindMinMax.cpp)
add_executable(demoRange demoRange.cpp)
add_executable(demoMedian demoMedian.cpp)
add_executable(demoPercentile demoPercentile.cpp)
add_executable(demoQuartile demoQuartile.cpp)

# Link each executable to the HomStatLib library
target_link_libraries(demoSort HomStatLib)
target_link_libraries(demoFindMinMax HomStatLib)
target_link_libraries(demoRange HomStatLib)
target_link_libraries(demoMedian HomStatLib)
target_link_libraries(demoPercentile HomStatLib)
target_link_libraries(demoQuartile HomStatLib)

