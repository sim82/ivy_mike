
#include <cstdio>
#include <cstdlib>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <algorithm>


#include "fpga_com.h"


// gcc -o conftest conftest.cpp fpga_com.c -I/home/nikos/src/boost_1_44_0 /home/nikos/src/boost_1_44_0/stage/lib/libboost_thread.a

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
	
	
	while( !m_bstop ) {
		
	    ssize_t size = recv( m_socket, rxb, max_rxsize, 0 );
	    printf( "recv: %zd %s\n", size, strerror(errno) );
	    for( int i = 0; i < size; i++ ) {
		if( i > 0 && (i % 16) == 0 ) {
		    
		    printf( "\n" );
		}
		printf( " %x", rxb[i] ); 
	    }
	    printf( "\n" );
	    m_bstop = true;
	}
	
	
    }
    
    void join() {
	m_thread->join();
	
    }
};


int main() {
    fpga_con_t con;
    
    fpga_con_init( &con, "131.159.28.113", 12340, 12350 );   
    
    bg_reader bgr( con.s );
    
    // send first command
    char cmd1[4] = {1,2,3,4};
    fpga_con_send(&con, cmd1, 4);
    fpga_con_send_init_packet(&con);
    
    
    char cmd2[4] = {1,2,3,4};
    fpga_con_send(&con, cmd2, 4);
    
    
    bgr.join();
    
}