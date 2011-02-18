subdirs( IvyMike pwosali raxml-hpc-pa_new pars_cluster parser_test test)
include_directories( IvyMike/src )
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(fpga_com)
add_subdirectory(pv_compress)
add_subdirectory(mpi_test)
add_subdirectory(ml_tools)
