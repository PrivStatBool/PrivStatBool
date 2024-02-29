#Test CMakeLists.txt
add_executable(timeBitOperAbsTwos timeBitOperAbsTwos.cpp)
target_link_libraries(timeBitOperAbsTwos HomStatLib)

add_executable(timeBitOper timeBitOper.cpp)
target_link_libraries(timeBitOper HomStatLib)

add_executable(timeComp timeComp.cpp)
target_link_libraries(timeComp HomStatLib)

add_executable(testComp testComp.cpp)
target_link_libraries(testComp HomStatLib)

add_executable(timeMinMax timeMinMax.cpp)
target_link_libraries(timeMinMax HomStatLib)

add_executable(testHomBitOper testHomBitOper.cpp)
target_link_libraries(testHomBitOper HomStatLib)

