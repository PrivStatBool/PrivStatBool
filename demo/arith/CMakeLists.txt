#Add executables
add_executable(demoAdd demoAdd.cpp)
add_executable(demoSub demoSub.cpp)
add_executable(demoMult demoMult.cpp)
add_executable(demoDiv demoDiv.cpp)

# Link each executable to the HomStatLib library
target_link_libraries(demoAdd HomStatLib)
target_link_libraries(demoSub HomStatLib)
target_link_libraries(demoMult HomStatLib)
target_link_libraries(demoDiv HomStatLib)


