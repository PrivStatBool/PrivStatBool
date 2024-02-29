#Add executables
add_executable(demoSqRoot demoSqRoot.cpp)

# Link each executable to the HomStatLib library
target_link_libraries(demoSqRoot HomStatLib)

