#include <cstdio>

#include <boost/thread.hpp>
#include "background.h"
#include <exception>
#include <sys/socket.h>
#include <boost/random.hpp>
#include <boost/timer.hpp>
#include "fpga_com.h"
#include "ivymike/time.h"
#include "ivymike/cycle.h"

typedef boost::uniform_int<> distribution_type;
typedef boost::minstd_rand   generator_type;
typedef boost::variate_generator< generator_type,distribution_type > RandomNumberGenerator;
static RandomNumberGenerator rand_uint(generator_type(), distribution_type( 0, 2048 - 4));

// static RandomNumberGenerator r( generator_type, distribution_type);

BackgroundReader::BackgroundReader(int s, size_t size)
    : m_socket(s),
    m_size(size),
    m_stop(false),
    m_readPtr(0),
    m_writePtr(0),
    m_readCyc(0),
    m_writeCyc(0),
    m_readbarrier(0),
    m_nblocked(0),
    m_nskipped1(0),
    m_nskipped2(0)
{
    
    m_data.reset( new char[m_size] );
    
}


BackgroundReader::~BackgroundReader()
{

}
void BackgroundReader::start() {
    m_thread.reset(new boost::thread(boost::bind(&BackgroundReader::run, this)));   
}

void BackgroundReader::checkInvariant() {

    if( !( 
	(m_readPtr <= m_writePtr && m_readCyc == m_writeCyc ) ||
	(m_readPtr >= m_writePtr && m_readCyc == (m_writeCyc - 1) ) ))
    {	
	
	printf( "invartiant error: %zd %zd %zd %zd\n", m_readPtr, m_writePtr, m_readCyc, m_writeCyc );
	throw std::runtime_error( "bailing out" );
    }
    
}

static int g_xxx = 0;
void BackgroundReader::run() {
    
    
    
    
    while( !m_stop ) {
	ticks t1 = getticks();
	while( getticks() - t1 < 24000 / 1.5 );
	
	m_lock.lock();
	checkInvariant();
	
	
	
	
	if( m_writePtr + m_maxsize > m_size ) {
	    // wrap around: write to start of buffer
	    
	    // if read pointer is after writepointer (in the previous cycle), push it forward until it is
	    // in the same cycle
	    if( m_readCyc == m_writeCyc - 1 ) {
		pushReadPtrToWriterCycle();
	    }
	    
	    // write the wrap around signal if it fits
	    if( m_writePtr + 4 <= m_size ) {
		int *sizep = (int*)(&m_data[m_writePtr]);
		*sizep = 0xFFFFFFFF;
	    }
	    
	    m_writePtr = 0;
	    m_writeCyc++;
	    // write cycle must now be larger than read cycle
	    assert( m_readCyc == m_writeCyc - 1 );
	    
	    
	    
	    
	    
	}
	
	// check if the distance to the read ptr is large enough
	if( m_readCyc == m_writeCyc - 1 ) {
	    pushReadPtrPast( m_writePtr + m_maxsize );
	}
	
	
	#if 0
	ssize_t size = recv( m_socket, &m_data[m_writePtr + 4], maxsize - 4, 0 );
	
	#else
	ssize_t size = rand_uint();
	memset( &m_data[m_writePtr + 4], (uint8_t)size & 0xFF, size );
	#endif
	assert( size >= 0 );
	
	int *sizep  = (int*) &m_data[m_writePtr];
	*sizep = (123 << 24) | int(size);
	
	m_writePtr += (4 + size);
	m_readbarrier = 0;
	
	checkInvariant();
	m_lock.unlock();
    }
    
}


void BackgroundReader::pushReadPtrToWriterCycle()
{
    assert( m_readCyc == m_writeCyc - 1);
    
    // loop until the readPtr hits the end of the buffer
    while( m_readPtr + 4 <= m_size ) {
	uint32_t *sizep = (uint32_t *)&m_data[m_readPtr];
	
	// check for the wrap around siganl
	if( *sizep == 0xFFFFFFFF ) {
	    break;
	}
	
	assert( ((*sizep) & 0xFF000000) == (123 << 24) );
	
	int size = (*sizep) & 0xFFFFFF;
	
	// the last packet may not actually contain data
	// if a packet does not fit on write (wrap around) but there are still
	// 4 bytes left the writer will just write the size and wrap around
	m_readPtr += (4 + size); 
	m_nskipped1++;
    }
    
    m_readPtr = 0;
    m_readCyc++;
        
}


void BackgroundReader::pushReadPtrPast(size_t goalPtr)
{

    // there are two ways to move the read ptr out of the way
    // 1. it is one cycle behind, but past goalPtr
    // 2. it is in the same cycle, but before the write ptr
    
    while( m_readPtr < goalPtr ) {
	uint32_t *sizep = (uint32_t *)&m_data[m_readPtr];
	
	
	
	// check for the wrap around siganl
	if( *sizep == 0xFFFFFFFF ) {
	    m_readPtr = 0;
	    m_readCyc++;
	    
	    assert( m_readCyc == m_writeCyc );
	    break;
	}
// 	printf( "sizep: %x\n", *sizep );
	assert( ((*sizep) & 0xFF000000) == (123 << 24) );
	
	int size = (*sizep) & 0xFFFFFF;
	
	
	m_readPtr += (4 + size);
	m_nskipped2++;
    }
}



ssize_t BackgroundReader::readPacket( void* buf, size_t size )
{
    m_lock.lock();
    
    checkInvariant();
    
    // check for wrap around of read cycle
    if( m_readCyc == m_writeCyc - 1 ) {
	if( m_readPtr + 4 > m_size ) {
	    m_readPtr = 0;
	    m_readCyc++;
	} else {
	    uint32_t *sizep = (uint32_t *)&m_data[m_readPtr];
	    
	    if( *sizep == 0xFFFFFFFF ) {
		m_readPtr = 0;
		m_readCyc++;
	    }
	}
    }
    
    
    
    // if no packet can be read (read ptr == write ptr), release the lock and wait on read barrier
    if( m_readPtr == m_writePtr && m_readCyc == m_writeCyc ) {
	
	m_readbarrier = 1;
	
	checkInvariant();
	m_lock.unlock();
	m_nblocked++;
	while( m_readbarrier != 0 );
	
	m_lock.lock();
	checkInvariant();
	if( m_readCyc == m_writeCyc - 1 ) {
	    if( m_readPtr + 4 > m_size ) {
		m_readPtr = 0;
		m_readCyc++;
	    } else {
		uint32_t *sizep = (uint32_t *)&m_data[m_readPtr];
		
		if( *sizep == 0xFFFFFFFF ) {
		    m_readPtr = 0;
		    m_readCyc++;
		}
	    }
	}
	
    } 
    
    {
	
	uint32_t *sizep = (uint32_t *)&m_data[m_readPtr];
	
	assert( ((*sizep) & 0xFF000000) == (123 << 24) );
	
	
	
	ssize_t rsize = (*sizep) & 0xFFFFFF;
	
	
	
	if( size >= rsize ) {
	    size = rsize;
	}
	
	memcpy( buf, &m_data[m_readPtr + 4], size );
// 	printf( "size: %zd\n", rsize );
	m_readPtr += ( 4 + rsize );
    }
    checkInvariant();
    m_lock.unlock();
    
    return size;
}


void BackgroundReader::join()
{
    m_stop = true;
    m_thread->join();
    
    printf( "nblocked: %zd\n", m_nblocked );
    printf( "nskipped: %d (%d + %d)\n", m_nskipped1 + m_nskipped2, m_nskipped1, m_nskipped2 );
}


int main() {
    const size_t bufsize = 1024 * 1024 * 1;
    
    fpga_con_t con;
    
    fpga_con_init( &con, "127.0.0.1", 1234, 1235 );
    
    BackgroundReader bgr( con.s, bufsize );
    
    
    size_t nrec = 0;
    size_t bread = 0;
    
    double t1 = ivy_mike::gettime();
    bgr.start();
    while( nrec < 1000000 ) {
	uint8_t buf[bgr.getMaxsize()];
	
	
	ssize_t size;
	
	while( (size = bgr.readPacket( buf, bufsize )) < 0 );
//  	printf( "size: %d\n", size );
	if( size >= 0 ) {
	    
	    for( int i = 0; i < size; i++ ) {
// 		fprintf( stderr, "%d %d\n", buf[i], size  );
		assert( buf[i] == (size & 0xFF) );
	    }
	    bread += size;
	    nrec++;
	}
	
	
	
    }
    
    double dt = ivy_mike::gettime() - t1;
    
    printf( "time: %f s %f Mb/s %f p/s (%zd %zd)\n", dt, bread / (dt * 1024 * 1024), nrec / dt, bread, nrec );
    
    bgr.join();
//     for( int i = 0; i < 1000; i++ ) {
// 	printf( "%d ", rand_uint() );
//     }
}
