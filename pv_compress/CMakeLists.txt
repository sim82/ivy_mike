
cmake_minimum_required( VERSION 2.8 )

project( pv_compress )
# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++0x")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")
add_executable( pv_compress pv_compress.cpp )

target_link_libraries( pv_compress lzo2 )