#include <cstdio>
#include <vector>
#include <boost/thread.hpp>
#include <boost/timer.hpp>


class spin_barrier {
public:
    int m_num;
    int *m_flags;
    
    void clear() {
	for( int i = 0; i < m_num; i++ ) {
	    m_flags[i] = 0;
	}
    }
    
    spin_barrier( int num ) : m_num(num) {
	m_flags = new int[num];
	clear();
    }
  
    void wait( int slot ) {
	
	
	m_flags[slot] = 1;
// 	printf( "wait: %d\n", slot );
// 	for( int i = 0; i < m_num; i++ ) {
// 	    printf( "%d ", m_flags[i] );
// 	}
// 	printf( "\n" );
	int sum = 0;
	do {
	    sum = 0;
	    
	    for( int i = 0; i < m_num; i++ ) {
		sum += m_flags[i];
	    }
// 	    printf( "sum: %d %d\n", slot, sum );
	} while( sum != 0 && sum != m_num );
	
	if( sum == m_num ) {
	    clear();
	}
    }
};

class bxbarrier : public boost::barrier {
public:
    bxbarrier( int num ) : boost::barrier(num) {}
    void wait( int slot ) {
	boost::barrier::wait();
    }
};
#if 0
typedef bxbarrier my_barrier;
#else
typedef spin_barrier my_barrier;
#endif
class worker {
    boost::shared_ptr<boost::thread> m_thread;
    my_barrier *m_barrier;
    my_barrier *m_barrier2;
    
    std::vector<worker> *m_others;
    bool m_master;
    int m_slot;
public:
    
    worker() 
    : m_barrier(0),
    m_barrier2(0),
    m_others(0)
    
    {}
    
    worker( my_barrier *barrier, my_barrier *barrier2,std::vector<worker> *others, int slot, bool master = false ) 
     : m_barrier(barrier),
     m_barrier2(barrier2),
     m_others( others ),
     m_master( master ),
     m_slot(slot)
    {}
    
    
    volatile int m_res;
    void start() {
	m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&worker::run, this)));
	//printf( "start\n" );
    }

    void run() {
	
	for( int j = 0; j < 10000; j++ ) {
	    m_barrier2->wait(m_slot);

	    m_res = 0;
	    
	    for( int i = 0; i < 100000; i++ ) {
 		if( i % 1000 == 0 ) {
// 		    printf( "%p: %d\n", m_thread.get(), i ); 
		  //  boost::thread::yield();
 		}
		m_res++;
	    }
	    //m_res = 666;
	   // printf( "call wait\n" );
	    m_barrier->wait(m_slot);
	    
	    if( m_master ) {
		
		//printf( "round %d\n", j );
		
		for( std::vector<worker>::iterator it = m_others->begin(); it != m_others->end(); ++it ) {
		    
		    if( get_res() != it->get_res() ) {
			printf( "meeeep: %p %d != %d\n", it, get_res(), it->get_res() );
		    }
		}
	    
	    }
	    
	    
	}
    }
    void join() {
	m_thread->join();
    }
    
    int get_res() {
	return m_res;
    }
    
};
int main() {
    
    const int N_THR = 32;
    
    my_barrier barrier(N_THR);
    my_barrier barrier2(N_THR);
    std::vector<worker> ws;
    //ws.reserve(8);
    
    for( int i = 0; i < N_THR; i++ ) {
	bool is_master = (i == 0);
	ws.push_back( worker( &barrier, &barrier2, &ws, i, is_master ) );
	
    }
    
    for( std::vector<worker>::iterator it = ws.begin(); it != ws.end(); ++it ) {
	
	it->start();
 
    }
    
    //barrier.wait();
    
    for( std::vector<worker>::iterator it = ws.begin(); it != ws.end(); ++it ) {
	it->join();
 
    }
}