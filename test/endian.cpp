#include <cstdio>
#include <ivymike/time.h>
#include <ivymike/cycle.h>
#include <vector>
#include <sys/types.h>
#include <stdint.h>
#include <cstring>
#include <byteswap.h>
#include <boost/concept_check.hpp>
#include <omp.h>

inline void swap( const double &v1, double &v2 ) {
    const char *v1c = (const char *)&v1;
    char *v2c = (char *)&v2;
    
    for( int i = 0; i < 8; i++ ) {
	v2c[i] = v1c[7-i];
    }
}


inline void swap3( double &v1, double &v2 ) {
    char *v1c = (char *)&v1;
    char *v2c = (char *)&v2;
    
    for( int i = 0; i < 4; i++ ) {
	std::swap( v2c[i], v1c[7-i] );
    }
}


template<class T>
inline void swap2( const T &v1, T &v2 ) {
    unsigned char uc[sizeof v1];
    
    memcpy( uc, &v1, sizeof v1 );
    for (unsigned char *b = uc, *e = uc + sizeof(T) - 1; b < e; ++b, --e) {
	std::swap(*b, *e);
    }
    
    memcpy(&v2, uc, sizeof v1);

}




template<class T>
inline T swap3(T &v1 ) {
    char *v1c = (char *)&v1;
    T v2;
    char *v2c = (char *)&v2; 
    
    
    for( int i = 0; i < 4; i++ ) {
	std::swap( v2c[i], v1c[7-i] );
    }
    
    return v2;
}


template<class T>
inline void swap4(T &v1, T &v2 ) {
    v2 = bswap_64(v1);
}

// void swap2( const double &v1, double &v2 ) {
//     const uint64_t v1c = *((const uint64_t *)&v1);
//     uint64_t v2c = (char *)&v2;
//     
//     
//     *v2 = ((v1c >> 60) & 0xFF ) | ((v1c >> 48) & 0xFF00 ) ((v1c >> 40) & 0xFF0000 ) | ((v1c >> 40) & 0xFF000000 ) |
// 	((v1c >> 32) & 0xFF00000000 ) | ((v1c >> 48) & 0xFF00 ) ((v1c >> 40) & 0xFF0000 ) | ((v1c >> 40) & 0xFF000000 )   
// }

int main() {
    const size_t N = 1024 * 1024;
    std::vector<double> v1(N);
//     double *v1 = new double[N];
    
    std::vector<double> v2(N);
    std::fill( v1.begin(), v1.end(), 1234.0);
//     std::fill( v1, v1 + N, 1234.0);
    ticks t1 = getticks();
    
    for( int j = 0; j < 1000; j++ ) {
// 	for( auto it = v1.begin(), it2 = v2.begin(); it != v1.end(); ++it, ++it2 ) {
// 	    //swap4( *it, *it );
// 	   // *it2 = bswap_64(*it);
// 	    //*it2 = swap3(*it);
// 	    swap4( *it, *it );
// 	    //swap3( *it, *it2 );
// 	}

#pragma omp for schedule (static, N/2)
	for( size_t i = 0; i < N; i++ ) {
	    //swap4( v1[i], v1[i] );
	    v1[i] = bswap_64(v1[i]);
	    
	}
    }
    
    ticks t2 = getticks();
    
    printf( "ticks: %lld\n", t2 - t1 );
    
    double x1 = 1234.0;
    double x2 = bswap_64(x1);
    double x3 = bswap_64(x2);
    
    double x4 = 1234.0;
    x4 = bswap_64(x4);
    x4 = bswap_64(x4);
    printf( "%f %f %f %f\n", x1, x2, x3, x4 );
    
    
}
