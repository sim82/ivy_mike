project( fpga_com )

add_executable( fpga_com test.c )
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -std=c99")