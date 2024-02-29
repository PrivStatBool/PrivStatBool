#Test CMakeLists.txt
add_executable(testAddOpt testAddOpt.cpp)
target_link_libraries(testAddOpt HomStatLib)

add_executable(testAddPlainOpt testAddPlainOpt.cpp)
target_link_libraries(testAddPlainOpt HomStatLib)


