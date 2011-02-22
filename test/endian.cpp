#if 1
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdint.h>
#include <endian.h>


template<typename T,size_t N>
struct swap_endian {
    inline T operator()( T &v ) {
        
        T vo = v;
        uint8_t *vb = (uint8_t*) &vo;
        
        for( size_t i = 0; i < sizeof(T) / 2; i++ ) {
            std::swap(vb[i], vb[sizeof(T) - i - 1]);
        }
        return vo;
    }
};


template<typename T>
struct swap_endian<T,2> {
    inline T operator()( T &v ) {
        uint16_t t = __bswap_16(*((uint16_t*)&v));
        
        T* r = (T*)&t; // we don't want to dereference a type-punned pointer, do we?
        return *r;
        
    }
};


template<typename T>
struct swap_endian<T,4> {
    inline T operator()( T &v ) {
        uint32_t t = __bswap_32(*((uint32_t*)&v));
        
        T* r = (T*)&t;
        return *r;
    }
};

template<typename T>
struct swap_endian<T,8> {
    inline T operator()( T &v ) {
        uint64_t t = __bswap_64(*((uint64_t*)&v));
        
        T* r = (T*)&t;
        return *r;
    }
};





template<typename T,size_t N>
struct swappy_au {
        
    swap_endian<T,N> swap;
    inline void operator()( void * dest, void * src, size_t n ) {
        std::copy( (char *)src, ((char *)src) + n * N, (char *)dest );
        //std::memcpy( dest, src, n * N );
        
        T * ptr = (T *)dest;
        T * end = ptr + n;
        
        // do the endian swapping in place in the aligned buffer
        while( ptr < end ) {
            *ptr = swap( *ptr );
            ptr++;
        }
        
    }

};

template<typename T,size_t N>
struct swappy_aa {
        
    swap_endian<T,N> swap;
    inline void operator()( void * dest, void * src, size_t n ) {
        
        
        T * sptr = (T *)src;
        T * ptr = (T *)dest;
        T * end = ptr + n;
        
        // do the endian swapping in place in the aligned buffer
        while( ptr < end ) {
            *ptr = swap( *sptr );
            ptr++;
            sptr++;
        }
        
    }

};

template<typename T,size_t N>
struct xerox_plain {
    inline void operator()( void * dest, void * src, size_t n ) {
        std::copy( (char*)src, ((char*)src) + n * N, (char*)dest ); // using char* here to prevent std;:copy form making any assumptions about th alignment of src/dest
        //std::copy( (T*)src, ((T*)src) + n, (T*)dest );
    }
};


int main() {
    std::vector<char> vbuf( 1024 * 9 + 1);
    std::vector<char> vbuf2( 1024 * 9 );
    
    char *buf2 = vbuf2.data();
    
    std::fill( vbuf.begin(), vbuf.end(), 16 );
    
    char *buf = vbuf.data();
    char *end = buf + vbuf.size();
    buf[8] = 17;
    
    swappy_aa<double,8> swappy;
//    xerox_plain<double,8> swappy;
    for( int i = 0; i < 400000; i++ ) {
        
        char * ptr = buf + 1;
        ssize_t to_copy = ssize_t(end-ptr) / 8;
        
        
        swappy( buf2, ptr, to_copy ); 
    }
    
//     for( int i = 0; i < 200000; i++ ) {
//         char * ptr = buf + 1;
//         ssize_t to_copy = ssize_t(end-ptr);
//         
//         
// #if 1
//         for( char *ptr = buf + 0, *ptr2 = buf2; ptr < end; ptr += 8, ptr2 += 8 ) {
//             double *dptr = (double *)ptr;
//             double *dptr2 = (double *)ptr2;
//             
//             *((uint64_t*)dptr2) = __bswap_64( *((uint64_t*)dptr) );
//             
//             
//         }
// #else
//         
//         
//         
//         std::copy( ptr, end, buf2 );
//         for( char *ptr2 = buf2; ptr2 < buf2 + to_copy; ptr2 += 8 ) {
//             double *dptr = (double *)ptr2;
//             
//             *((uint64_t*)dptr) = __bswap_64( *((uint64_t*)dptr) );
//         }
//         //std::copy( buf2, buf2 + to_copy, ptr );
//         
// #endif
//     }
    
    printf( "%d %d %d %d\n", buf2[0], buf2[1], buf2[2], buf2[3] );
}


#else
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
#endif