# Add executables
add_executable(demoMinMax demoMinMax.cpp)

# Link each executable to the HomStatLib library
target_link_libraries(demoMinMax HomStatLib)

