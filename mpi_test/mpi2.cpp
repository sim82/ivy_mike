#undef NDEBUG


#if 1
#include </opt/mpich2/include/mpi.h>
#endif
#include <mpi.h>
#include <iostream>



#include <boost/thread.hpp>
#include <cassert>
#include <algorithm>
#include <vector>

int main(int argc, char* argv[])
{
    
    
    MPI_Init(&argc, &argv);

    int rank;
    int numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    
    std::vector<uint8_t>rm(1024);
    
    
    

    MPI::Win win2 = MPI::Win::Create(rm.data(), rm.size(), 1, MPI_INFO_NULL, MPI_COMM_WORLD );
    if( rank == 0 ) {
	win2.Lock(MPI_LOCK_EXCLUSIVE, 0, 0);
	std::fill(rm.begin(), rm.end(), 255 );
	win2.Unlock(0);
    }
    
    win2.Fence(0);
    printf( "fence: %d\n", rank );
    {
	
	std::vector<uint8_t>lb(1024);
	std::fill( lb.begin(), lb.end(), 100 );
	
	win2.Lock(MPI_LOCK_EXCLUSIVE, 0, 0);
	printf( "after lock: %d\n", rank );
	
	win2.Get(lb.data(), lb.size(), MPI_CHAR, 0, 0, lb.size(), MPI_CHAR );
	printf( "after get: %d\n", rank );
	win2.Unlock(0);
	for( int i = 0; i < lb.size(); i++ ) {
	    if( (i % numProcs) == rank ) {
		//{
		//assert( lb[i] == 255 );
// 		if( lb[i] != 255 ) {
// 		    printf( "bad: %d %d %d\n", rank, i, lb[i] );
// 		}
		assert( lb[i] == 255 );
		lb[i] = rank;
	    }
	}
	win2.Lock(MPI_LOCK_EXCLUSIVE, 0, 0);
	win2.Put(lb.data(), lb.size(), MPI_CHAR, 0, 0, lb.size(), MPI_CHAR );
	printf( "after put: %d\n", rank );
	win2.Unlock(0);
	printf( "after unlock: %d\n", rank );
	
    }
    
    
    win2.Fence(0);
    
    
    if( rank == 0 ) {
	win2.Lock(MPI_LOCK_EXCLUSIVE, 0, 0);
	
	//std::vector<uint8_t>lb(1024);
	//win2.Get(lb.data(), lb.size(), MPI_CHAR, 0, 0, lb.size(), MPI_CHAR );
	
	for( int i = 0; i < rm.size(); i++ ) {
	    printf( "%d: %d\n", i, rm[i] );
	}
	
	win2.Unlock(0);
	
    }
    MPI_Finalize();
   
}