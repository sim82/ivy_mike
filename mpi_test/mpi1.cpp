#if 1
#include </opt/mpich2/include/mpi.h>
#endif
#include <mpi.h>
#include <iostream>

#include <boost/thread.hpp>
#include <cassert>
#include <algorithm>
#include <vector>

const int CL_SCORES = 0x666;
const int SV_MINSCORES = 0x777;
const int CL_EXIT = 0x888;
const int NSCORES = 1024 * 1024;


enum Tag {
    S_M_SCORES = 0x666,
    M_S_MINSCORES = 0x777,
    S_M_EXIT = 0x888
};

struct MpiMaster {

    struct Probe {
	Tag 	m_tag;
	size_t 	m_count;
	int	m_source;
	int 	m_type;
    };
    

    MpiMaster() {
	
	
    }
  

    Probe probe() {
	MPI_Status stat;
	int res = MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat );
    
	assert( res == MPI_SUCCESS );
	Probe p;
	p.m_tag = Tag(stat.MPI_TAG);
	p.m_count = stat.count;
	p.m_source = stat.MPI_SOURCE;
	
	return p;
   
	
    }
    
    void recv( Probe p, std::vector<uint32_t> &v ) {
	MPI_Status stat;
	int result = MPI_Recv( v.data(), v.size(), MPI_INT, p.m_source, p.m_tag, MPI_COMM_WORLD, &stat );
	
	assert( result == MPI_SUCCESS );
    }
    
   
    void send( int receiver, int tag, std::vector<uint32_t> &v ) {
	int result = MPI_Send( v.data(), v.size(), MPI_INT, receiver, tag, MPI_COMM_WORLD);
	assert( result == MPI_SUCCESS );
    }
  
};



struct MpiSlave {
    
  
    void recv( int tag, std::vector<uint32_t> &v ) {
	MPI_Status stat;
	int result = MPI_Recv( v.data(), v.size(), MPI_INT, 0, tag, MPI_COMM_WORLD, &stat );
	assert( result == MPI_SUCCESS );
    }
    
    
    void send( int tag, std::vector<uint32_t> &v ) {
	int result = MPI_Send( v.data(), v.size(), MPI_INT, 0, tag, MPI_COMM_WORLD );
	assert( result == MPI_SUCCESS );
    }
};


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank;
    int numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
    
    int numSlaves = numProcs - 1;
    if (rank == 0) {
	std::vector<uint32_t> glscores(NSCORES);
	for( int i = 0; i < NSCORES; i++ ) {
	    glscores[i] = INT_MAX;
	}


	MpiMaster m;
	while( numSlaves > 0 ) {
	    	    
	    MpiMaster::Probe p = m.probe();
	    
	    if( p.m_tag == S_M_SCORES ) {
		assert( p.m_count == NSCORES );
		std::vector<uint32_t> res(NSCORES);
		
		m.recv(p, res);
		printf( "%d recv: S_M_SCORES\n", rank );
		
		
		
		for( int i = 0; i < NSCORES; i++ ) {
		    glscores[i] = std::min( res[i], glscores[i] );
		}

		m.send( p.m_source, M_S_MINSCORES, glscores );

		printf( "%d send: M_S_MINSCORES\n", rank );
	    } else if( p.m_tag == S_M_EXIT ) {
		std::vector<uint32_t> dummy(1);
		m.recv(p, dummy);
		//int result = MPI_Recv( dummy, 1, MPI_INT, stat.MPI_SOURCE, CL_EXIT, MPI_COMM_WORLD, &stat );
		
		numSlaves--;
	    }
	    
	}
	printf( "all slaves dead\n" );
    } else if (rank > 0) {
	MpiSlave s;
	
	std::vector<uint32_t> minscores(NSCORES);
	for( int j = 0; j < 100; j++ ) {
	  //  boost::this_thread::sleep(boost::posix_time::millisec(2000));
	    std::vector<uint32_t> res(NSCORES);
	    
	    for( int i = 0; i < NSCORES; i++ ) {
		res[i] = rand() % 1000;
	    }
	    
	    s.send( S_M_SCORES, res );
	    
	    printf( "%d send: S_M_SCORES\n", rank );
	    
	    s.recv( M_S_MINSCORES, minscores );
	    
	    
	    printf( "%d recv: M_S_MINSCORES\n", rank );
	}
	
	{
	    std::vector<uint32_t> dummy;
	    dummy.push_back(0);
	    
	    s.send( S_M_EXIT, dummy );
	    printf( "slave dies: %d\n", rank );
	}
    }
    MPI_Finalize();
    return 0;
} 

// int main(int argc, char* argv[])
// {
//     MPI_Init(&argc, &argv);
// 
//     int rank;
//     int numProcs;
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//     MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
//     
//     int numSlaves = numProcs - 1;
//     if (rank == 0) {
// 	std::vector<uint32_t> glscores(NSCORES);
// 	for( int i = 0; i < NSCORES; i++ ) {
// 	    glscores[i] = INT_MAX;
// 	}
// 	
// 	while( numSlaves > 0 ) {
// 	    
// 	    
// 	    
// 	    MPI_Status stat;
// 	    MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat );
// 	    
// 	    if( stat.MPI_TAG == CL_SCORES ) {
// 		assert( stat.count == NSCORES );
// 		std::vector<uint32_t> res(NSCORES);
// 		
// 		int sender = stat.MPI_SOURCE;
// 		
// 		int result = MPI_Recv( res.data(), NSCORES, MPI_INT, stat.MPI_SOURCE, CL_SCORES, MPI_COMM_WORLD, &stat );
// 		printf( "%d recv: CL_SCORES\n", rank );
// 		assert( result == MPI_SUCCESS );
// 		
// 		
// 		for( int i = 0; i < NSCORES; i++ ) {
// 		    glscores[i] = std::min( res[i], glscores[i] );
// 		}
// 		
// 		MPI_Send( glscores.data(), NSCORES, MPI_INT, sender, SV_MINSCORES, MPI_COMM_WORLD);
// 		printf( "%d send: SV_MINSCORES\n", rank );
// 	    } else if( stat.MPI_TAG == CL_EXIT ) {
// 		int dummy[1];
// 		int result = MPI_Recv( dummy, 1, MPI_INT, stat.MPI_SOURCE, CL_EXIT, MPI_COMM_WORLD, &stat );
// 		
// 		numSlaves--;
// 	    }
// 	    
// 	}
// 	printf( "all slaves dead\n" );
//     } else if (rank > 0) {
// 	
// 	std::vector<uint32_t> minscores(NSCORES);
// 	for( int j = 0; j < 100; j++ ) {
// 	  //  boost::this_thread::sleep(boost::posix_time::millisec(2000));
// 	    std::vector<uint32_t> res(NSCORES);
// 	    
// 	    for( int i = 0; i < NSCORES; i++ ) {
// 		res[i] = rand() % 1000;
// 	    }
// 	    int result = MPI_Send( res.data(), NSCORES, MPI_INT, 0, CL_SCORES, MPI_COMM_WORLD );
// 	    printf( "%d send: CL_SCORES\n", rank );
// 	    assert( result == MPI_SUCCESS );
// 	    
// 	    MPI_Status stat;
// 	    result = MPI_Recv( minscores.data(), NSCORES, MPI_INT, 0, SV_MINSCORES, MPI_COMM_WORLD, &stat );
// 	    printf( "%d recv: SV_MINSCORES\n", rank );
// 	}
// 	
// 	{
// 	    int dummy[1] = {0};
// 	    int result = MPI_Send( dummy, 1, MPI_INT, 0, CL_EXIT, MPI_COMM_WORLD );
// 	    printf( "slave dies: %d\n", rank );
// 	}
//     }
//     MPI_Finalize();
//     return 0;
// } 


// int main(int argc, char* argv[])
// {
//   MPI_Init(&argc, &argv);
// 
//   int rank;
//   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//   if (rank == 0) {
//     int value = 17;
//     int result = MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
//     if (result == MPI_SUCCESS)
//       std::cout << "Rank 0 OK!" << std::endl;
//   } else if (rank > 0) {
//     int value;
//     int result = MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
// 			  MPI_STATUS_IGNORE);
//     if (result == MPI_SUCCESS && value == 17)
//       std::cout << "Rank 1 OK!" << std::endl;
//   }
//   MPI_Finalize();
//   return 0;
// } 



// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// 
// #include <mpi.h>
// 
// int main (int iArgC, char *apArgV [])
// {
//   int iRank;
//   int iLength;
//   char *pMessage;
//   char acMessage [] = "Hello World !";
// 
//   MPI_Init (&iArgC, &apArgV);
// 
//   MPI_Comm_rank (MPI_COMM_WORLD, &iRank);
// 
//   if (iRank == 0)
//   {
//     iLength = sizeof (acMessage);
//     MPI_Bcast (&iLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     MPI_Bcast (acMessage, iLength, MPI_CHAR, 0, MPI_COMM_WORLD);
//     printf ("Process 0: Message sent\n");
//   }
//   else
//   {
//     MPI_Bcast (&iLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
//     pMessage = (char *) malloc (iLength);
//     MPI_Bcast (pMessage, iLength, MPI_CHAR, 0, MPI_COMM_WORLD);
//     printf ("Process %d: %s\n", iRank, pMessage);
//   }
// 
//   MPI_Finalize ();
// 
//   return (0);
// }
