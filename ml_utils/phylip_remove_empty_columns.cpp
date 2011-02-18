
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <vector>
#include <algorithm>
#include "ivymike/LargePhylip.h"
#include <boost/dynamic_bitset.hpp>


static inline bool is_gap( char c ) {
    return c == '-' || c == 'N' || c == '?' || c == 'O' || c == 'X';
}

static inline char removeStupidCharacters(char b) {
    if( b == 'N' || b == 'n' || b == '?' ) {
        return '-';
    } else {
        return b;
    }
        
}

int main( int argc, char *argv[] ) {
    if( argc != 2 ) {
        throw std::runtime_error( "argc != 2" );
        
    }
    const char *phyname = argv[1];
    
    LargePhylip lp(phyname);
    
//     lp.map();
    
    typedef boost::dynamic_bitset<> bs_t;
    
    bs_t bs(lp.getSeqLen());
    
    
    bs_t bsc( lp.getSeqLen() );
    
    
    size_t max_name_len = 0;
    
    for( int i = 0; i < lp.size(); i++ ) {
        max_name_len = std::max( max_name_len, lp.getNameLen(i) );
    
        u1_t *it = lp.getSequenceBegin(i);    
        u1_t *end = lp.getSequenceEnd(i);
//     size_t ct = 0;    
        for( int i = 0; it != end; ++it, ++i ) {
            bsc[i] = !is_gap( *it );
//             ct++;
        }
//     printf( "ct: %zd %zd\n", ct, lp.getSeqLen() );
        bs |= bsc;
        
    }

    
    std::vector<int>map;
    
    size_t nbits = bs.count();
    map.reserve(nbits);
    
    for( int i = bs.find_first(); i != bs_t::npos; i = bs.find_next(i)) {
        map.push_back(i);
    }
    
    assert( nbits == map.size() );
    
    printf( "%d %d\n", lp.size(), nbits );
    for( int i = 0; i < lp.size(); i++ ) {
        std::string name = lp.getName(i);
        u1_t *data = lp.getSequenceBegin(i);
        
        for( int j = 0; j < /*maxNameLen + 1*/ name.size() + 1; j++ ) {
                
            if( j < name.size() ) {
                putc( name[j], stdout );
            } else {
                putc( ' ', stdout );
            }
        }
//                      FileOutputStream fsdfds;
        for( int j = 0; j < map.size(); j++ ) {
            putc( removeStupidCharacters( data[map[j]]), stdout);
                    
        }
        putc( '\n', stdout );
        
    }
    
    
}