#ifndef __LARGEPHYLIP_H
#define __LARGEPHYLIP_H


#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

typedef unsigned char u1_t;
// namespace gnu = __gnu_cxx;

struct Rec {
    off_t name;
    int nameLen;
    int nameMax;

    off_t data;
    int dataLen;
    int dataMax;

    inline std::string getName( u1_t *base ) {
//         char *tmp = (char*)alloca(nameLen+1);
//         memcpy( tmp, base + name, nameLen );
//         tmp[nameLen] = 0;
// 
//         return std::string(tmp);

	return std::string( base + name, base + name + nameLen );
    }

    inline std::string getData( u1_t *base ) {
//         char *tmp = (char*)alloca(dataLen+1);
//         memcpy( tmp, base + data, dataLen );
//         tmp[dataLen] = 0;

        
	return std::string( base + data, base + data + dataLen );
    }

};

class LargePhylip {
    int m_fd;
    off_t      m_fileSize;

    u1_t *m_buf;

    int m_nTaxa;
    int m_seqLen;
    int m_maxNameLen;

    std::vector<Rec> m_recs;

    std::map<std::string,size_t> m_nameMap;


    void interpret( off_t line, off_t lineLen, Rec &rec ) ;



public:
    LargePhylip( const char *filename ) ;

    ~LargePhylip() ;
    void print() ;
    void map() ;

    void unmap() ;

    int getIdx( const char *name ) ;
    
    inline std::string getName( int i ) {
	assert( m_buf != 0 );
	return m_recs.at(i).getName(m_buf);
    }
    
    inline std::string getSequence( int i ) {
	assert( m_buf != 0 );
	return m_recs.at(i).getData(m_buf);
    }
    
    inline int size() {
	return m_recs.size();
    }
    
    inline int getSeqLen() {
	return m_seqLen;
    }
};


#endif