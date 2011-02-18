#include <cstdio>
#include <cstdlib>

#include "fpga_com.h"
#include "background.h"
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <algorithm>
// #include <boost/thread/

class bg_reader {
    int m_socket;
    boost::scoped_ptr<boost::thread> m_thread;
    bool m_bstop;
    
public:    
    bg_reader( int socket ) : m_socket(socket), m_thread(new boost::thread(boost::bind(&bg_reader::run, this))), m_bstop(false) {
	
    }
    
    void run() {
	const size_t max_rxsize = 10 * 1024;
	char rxb[max_rxsize];
	int np = 0;
	int abs_last = 0;
	
	size_t nncont = 0;
	size_t npkg = 0;
	while( !m_bstop ) {
		
	    ssize_t size = recv( m_socket, rxb, max_rxsize, 0 );
	    npkg++;
	    
	    bool ncont = std::abs(std::abs(rxb[0]) - abs_last) > 1;
	    if( ncont ) {
		nncont++;
		//printf( "ncont\n" );
	    }
	    //printf( "%p %d recv: %zd %d %s\n", this, np, size, rxb[0], ncont ? "XXXXXXXXXXXXXX" : "" );
	    abs_last = std::abs(rxb[0] );
	    np++;
	    if( size < 1024 ) {
		m_bstop = true;
	    }
	}
	printf( "ncont: %zd of %zd\n", nncont, npkg );
	
    }
    
    void join() {
	m_thread->join();
	
    }
};

int main() {
    const size_t bufsize = 1024 * 1024 * 1;
    
    fpga_con_t con;
    
    fpga_con_init( &con, "131.159.28.113", 12340, 12350 );
    
    bg_reader bgr( con.s );
//     bg_reader bgr2( con.s );
    char buf[4];
    
    fpga_con_send( &con, buf, 4 );
    
    const size_t rbuf_size = 10 * 1024;
    char rbuf[rbuf_size];
//     while( true ) {
// 	
// 	
// 	//size_t s = bgr.readPacket( rbuf, rbuf_size );
// 	
// 	//printf( "recv size: %zd\n", s );
// 	
// 	if( s < 1024 ) {
// 	    break;
// 	}
// 	
//     }

    bgr.join();
//     bgr2.join();
}