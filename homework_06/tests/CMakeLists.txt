add_subdirectory(../data data)
add_executable(ballistics_tests  ballistics_tests.cpp)
target_link_libraries(ballistics_tests gtest gtest_main)
add_test(NAME ballistics_tests COMMAND ballistics_tests)    