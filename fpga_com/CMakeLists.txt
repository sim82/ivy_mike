cmake_minimum_required( VERSION 2.8 )

project( fpga_com )

add_executable( fpga_com test.c fpga_com.c )
# add_executable( bg background.cpp fpga_com.c )
add_executable( bg_test background.cpp fpga_com.c bg_test.cpp )
add_executable( conftest fpga_com.c conftest.cpp )

add_executable( bg_test2 background_reader.cpp  fpga_com.c )

# target_link_libraries( bg boost_thread-mt ivymike)
target_link_libraries( bg_test2 boost_thread-mt )
target_link_libraries( bg_test boost_thread-mt )
target_link_libraries( conftest boost_thread-mt )
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -std=c99")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")