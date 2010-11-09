find_package(MPI)

set(CMAKE_CXX_COMPILER mpic++)
set(CMAKE_C_COMPILER mpicc)


ADD_EXECUTABLE( mpi1 mpi1.cpp )
ADD_EXECUTABLE( mpi2 mpi2.cpp )
target_link_libraries( mpi1 boost_thread-mt )
