configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/../data/sample_vog17.txt" 
    "${CMAKE_CURRENT_BINARY_DIR}/../data/sample_vog17.txt" 
    COPYONLY
)
add_executable(ballistics_tests  ballistics_tests.cpp)
target_link_libraries(ballistics_tests gtest gtest_main)
add_test(NAME ballistics_tests COMMAND ballistics_tests)    
set_tests_properties(ballistics_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../data")