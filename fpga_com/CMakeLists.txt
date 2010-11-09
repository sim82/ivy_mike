cmake_minimum_required( VERSION 2.8 )

project( fpga_com )

add_executable( fpga_com test.c fpga_com.c )
add_executable( bg background.cpp fpga_com.c )

target_link_libraries( bg boost_thread-mt ivymike)
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -std=c99")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")