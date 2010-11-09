#include <cstdio>
#include <malloc.h>
#include <vector>
#include <cxxabi.h>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <boost/array.hpp>
#include <sys/mman.h>

#if HAVE_LIB_IM // quick hack, to get this to build without dependencies...
#include "ivymike/time.h"
#else
namespace ivy_mike {
    double gettime(void);
}

double ivy_mike::gettime(void )
{
#ifdef WIN32
  time_t tp;
  struct tm localtm;
  tp = time(NULL);
  localtm = *localtime(&tp);
  return 60.0*localtm.tm_min + localtm.tm_sec;
#else
  struct timeval ttime;
  gettimeofday(&ttime , 0);
  return ttime.tv_sec + ttime.tv_usec * 0.000001;
#endif
}
#endif


#define BAR_NOTICK

#ifndef BAR_NOTICK
#include "ivymike/cycle.h"
#else
typedef unsigned long long ticks;
static inline ticks getticks() {
	return 0;
}
#endif

double randum (long  *seed)
{
  long  sum, mult0, mult1, seed0, seed1, seed2, newseed0, newseed1, newseed2;
  double res;

  mult0 = 1549;
  seed0 = *seed & 4095;
  sum  = mult0 * seed0;
  newseed0 = sum & 4095;
  sum >>= 12;
  seed1 = (*seed >> 12) & 4095;
  mult1 =  406;
  sum += mult0 * seed1 + mult1 * seed0;
  newseed1 = sum & 4095;
  sum >>= 12;
  seed2 = (*seed >> 24) & 255;
  sum += mult0 * seed2 + mult1 * seed1;
  newseed2 = sum & 255;

  *seed = newseed2 << 24 | newseed1 << 12 | newseed0;
  res = 0.00390625 * (newseed2 + 0.000244140625 * (newseed1 + 0.000244140625 * newseed0));

  return res;
}


static inline size_t roundpage( size_t s ) {
    if( (s % 4096) == 0 ) {
	return s;
    } else {
	return (s / 4096 + 1) * 4096;
    }
    
}

// allocate fresh pages
void *my_malloc( size_t s ) {
    void *ptr = mmap( 0, s, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0 );
    assert( ptr != 0 );
    return ptr;
}

void my_free( void *ptr ) {	
    
}


template<int PAD>
class lockfree_spin_barrier {
public:
    int m_num;
    volatile char *m_flags;
    
    void clear() {
	for( int i = 0; i < m_num; i++ ) {
	    m_flags[i * PAD] = 0;
	}
    }
    
    lockfree_spin_barrier( int num ) : m_num(num), m_flags(0) {

    }

    void alloc() {
	#if 0
	m_flags = ((char *)memalign(64, num * PAD + 60)) + 60;
	//m_flags = ((char *)memalign(64, num * PAD + 40)) + 40;
	#else
	
	//m_flags = (volatile char *)memalign(64, m_num * PAD );
	m_flags = (volatile char *)my_malloc(m_num * PAD );
	#endif
	clear();
    }

    inline void wait( const int slot ) {
	
	

	
	if( slot == 0 ) {
	    
	    int sum = 0;
	    do {
		sum = 0;
		
		for( int i = 0; i < m_num * PAD; i+=PAD ) {
		    sum += m_flags[i];
		}
// 	    printf( "sum: %d %d\n", slot, sum );
	    } while( sum != m_num - 1 );
	    clear();
	} else {
	    m_flags[slot * PAD] = 1;
	}
    }
    
    const char *id() {
	return "lockfree_spin_barrier";
    }
};

const static int lookupTree[129] =
{
    0,
    0,
    1,
    2, 2,
    3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
};
template<int PAD>
class recursive_spin_barrier {
public:
    


    int m_num;
    volatile char *m_flags;
    
    void clear() {
	for( int i = 0; i < m_num; i++ ) {
	    m_flags[i * PAD] = 0;
	}
    }
    
    recursive_spin_barrier( int num ) : m_num(num), m_flags(0) {

    }

    void alloc() {
	#if 0
	m_flags = ((char *)memalign(64, num * PAD + 60)) + 60;
	//m_flags = ((char *)memalign(64, num * PAD + 40)) + 40;
	#else
	
	//m_flags = (volatile char *)memalign(64, m_num * PAD );
	m_flags = (volatile char *)my_malloc(m_num * PAD );
	#endif
	clear();
    }

    inline void wait (int tid)
    {
	int n = m_num;
	int steps = lookupTree[m_num];
	
	if(tid % 2 == 0)
	{
	    int
	    i,
	    partner = 1,
	    active = 2;


	    for(i = 0; i < steps; i++)
	    {                	  	
		if(tid % active == 0)
		{
		    int commPartner =  tid + partner;
	    
		    if(commPartner < n) {
			while(m_flags[PAD * commPartner] == 0);	
		    }
		} else {
		    break;
		}
		active *= 2;
		partner *= 2;
	    }
	}

	m_flags[PAD * tid] = 1;

	if(tid == 0)
	{
	    int i;

	    for(i = 0; i < n; i++) {
		m_flags[PAD * i] = 0;
	    }
	}
	    
    }
    
};

class bxbarrier : public boost::barrier {
public:
    bxbarrier( int num ) : boost::barrier(num) {}
    inline void wait( int slot ) {
	boost::barrier::wait();
    }
    
    const char *id() {
	return "bxbarrier";
    }
    
    void alloc() {}
};
void stupid_atomic_increment( volatile int * pw );
static inline void my_atomic_increment( volatile int * pw )
{
    //atomic_exchange_and_add( pw, 1 );

    __asm__
    (
        "lock\n\t"
        "incl %0":
        "=m"( *pw ): // output (%0)
        "m"( *pw ): // input (%1)
        "cc" // clobbers
    );
}
static inline int my_atomic_exchange_and_add( volatile int * pw, int dv )
{
    // int r = *pw;
    // *pw += dv;
    // return r;

    int r;

    __asm__ __volatile__
    (
        "lock\n\t"
        "xadd %1, %0":
        "=m"( *pw ), "=r"( r ): // outputs (%0, %1)
        "m"( *pw ), "1"( dv ): // inputs (%2, %3 == %1)
        "memory", "cc" // clobbers
    );

    return r;
}
class atomic_spin_barrier {
    const int m_num;
    volatile int *m_ct;
    
public:
    
    atomic_spin_barrier( int num ) : m_num(num), m_ct(0) {}
    void alloc() {
	m_ct = (volatile int *)my_malloc( sizeof(int) );
	*m_ct = 0;
    }
    inline void wait( int slot ) {
	
	//__sync_fetch_and_add( &m_ct, 1 );
	
	
	//stupid_atomic_increment(&m_ct);
	
	
	if( slot == 0 ) {
	    while( *m_ct != m_num - 1 ) {}
	    *m_ct = 0;
	} else {
	    my_atomic_increment(m_ct);   
	}
    }
    const char *id() {
	return "atomic_spin_barrier";
    }
};

class mutex_spin_barrier {
    const int m_num;
    volatile int *m_ct;
    boost::mutex m_mtx;
public:
    
    mutex_spin_barrier( int num ) : m_num(num), m_ct(0) {}
    void alloc() {
	m_ct = (volatile int *)my_malloc( sizeof(int) );
	*m_ct = 0;
    }
    inline void wait( int slot ) {
	
	
	
	if( slot == 0 ) {
	    while( *m_ct != m_num - 1 ) {}
	    *m_ct = 0;
	} else {
	    m_mtx.lock();
	    (*m_ct)++;
	    m_mtx.unlock();    
	}
    }
    const char *id() {
	return "locked_spin_barrier";
    }
};


#ifndef BAR_N_THR
#warning "building with default settings"

#define BAR_N_THR (1)
#define BAR_N (20000000*4)
#define BAR_M (0)
#define BAR_PIN (1)
#define BAR_AS
#endif

#ifdef BAR_AS
    typedef atomic_spin_barrier my_barrier;
#endif

#ifdef BAR_LS1
    typedef lockfree_spin_barrier<1> my_barrier;
#endif

#ifdef BAR_LS64
    typedef lockfree_spin_barrier<64> my_barrier;
#endif

#ifdef BAR_MS
    typedef mutex_spin_barrier my_barrier;
#endif

#ifdef BAR_BX
    typedef bxbarrier my_barrier;
#endif

#ifdef BAR_RS1
    typedef recursive_spin_barrier<1> my_barrier;
#endif

#ifdef BAR_RS64
    typedef recursive_spin_barrier<64> my_barrier;
#endif


//template<int N, int M, int PIN, class my_barrier>
class worker {
    //boost::shared_ptr<boost::thread> m_thread;
    boost::thread *m_thread;
    my_barrier *m_barrier;
    //my_barrier *m_barrier2;
    volatile int *m_fence;
    std::vector<worker> *m_others;
    bool m_master;
    int m_slot;
    
//     boost::array<float,BAR_M> m_v1;
//     boost::array<float,BAR_M> m_v2;
//     boost::array<float,BAR_M> m_v3;
    float *m_v; // pointer to global template vector

    float *m_v1;
    float *m_v2;
    float *m_v3;
    volatile int m_res;
    
public:
    
    worker() 
    : m_barrier(0),
    m_fence(0),
    m_others(0),
    m_v(0),
    m_v1(0),
    m_v2(0),
    m_v3(0),
    m_res(-1)
    
    {}
    
    worker( my_barrier *barrier, volatile int *fence,std::vector<worker> *others, int slot, float *v, bool master = false ) 
     : m_barrier(barrier),
     m_fence(fence),
     m_others( others ),
     m_master( master ),
     m_slot(slot),
     m_v(v),
     m_v1(0),
     m_v2(0),
     m_v3(0),
     m_res(-1)
    {
	
    }
    ~worker() {
// 	free( m_v1 );
// 	free( m_v2 );
// 	free( m_v3 );

	my_free( m_v1 );
	my_free( m_v2 );
	my_free( m_v3 );
    }
    void fill() {
// 	m_v1 = (float*) malloc( BAR_M * sizeof(float));
// 	m_v2 = (float*) malloc( BAR_M * sizeof(float));
// 	m_v3 = (float*) malloc( BAR_M * sizeof(float));
	m_v1 = (float*) my_malloc( BAR_M * sizeof(float) );
	m_v2 = (float*) my_malloc( BAR_M * sizeof(float) );
	m_v3 = (float*) my_malloc( BAR_M * sizeof(float) );
	for( int i = 0; i < BAR_M; i++ ) {
	    m_v1[i] = m_v2[i] = m_v3[i] = m_v[i];
	}
	
    }
    
    
    void start() {
	//m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&worker::run, this)));
	m_thread = new boost::thread(boost::bind(&worker::run, this));
	//printf( "start\n" );
    }

    void pin_thread() {
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(m_slot * BAR_PIN, &cpuset);

	if(pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
	{
	    printf("\n\nThere was a problem finding a physical core for thread number %d to run on.\n", m_slot);
	  
	    assert(0);
	}
    }

    void run() {
	if( BAR_PIN != 0 ) {
	    pin_thread();
	}
	
	size_t allticks = 0;
	    
	double t1 = 0.0;
	fill();
	if( m_master ) {
	    m_barrier->alloc();
	   
	    t1 = ivy_mike::gettime();
	}
	long int seed = 1234 + m_slot;
	//const int N = 4000000 * 10;
	//const int M = 0;
	for( int j = 0; j < BAR_N; j++ ) {
	    
	    if( m_master ) {
		//printf( "setfence: %d %d\n", *m_fence, j );
		*m_fence = j; 
	    } else {
		
// 		printf( "waitfence: %d %d\n", *m_fence, j );
		while( *m_fence != j ) {}
	    }
	    
	    //m_barrier2->wait(m_slot);
	    
// 	    if( m_slot == 0 ) {
// 		nextIter = j;
// 	    } else {
// 		while( nextIter != j ) {}
// 		    
// 	    }

	    m_res = 0;

#if 1	    
	    int tmp = 0;
	    for( int i = 0; i < BAR_M; i++ ) {
 		m_v3[i] = m_v1[i] * m_v2[i];
		tmp += m_v3[i];
		//m_res++;
	    }



	    m_res = tmp;
#else
// alternative workload
	    const size_t vsize = sizeof(float) * BAR_M;
	    const int npart = 2000;
	    const size_t part_size = vsize / npart;
	    
	    char *v1 = (char *) m_v1;
	    char *v2 = (char *) m_v2;
	    char *v3 = (char *) m_v3;
	    
	    
	    int pos1 = int(randum(&seed) * npart) % npart;
	    int pos2 = int(randum(&seed) * npart) % npart;
// 	    printf( "pos: %d %d\n", pos1, pos2 );
	    memcpy( &v3[pos1 * part_size], &v1[pos2 * part_size], part_size );
 	    //memcpy( &v3[(j % npart) * part_size], &v2[(j % npart) * part_size], part_size );
#endif
	    //m_res = 666;
	    
	    if( m_master ) {
		ticks t1 = getticks();
		m_barrier->wait(m_slot);
		ticks t2 = getticks();
		//printf( "round %d\n", j );
		//printf( "ticks: %zd\n", t2 - t1 );
		
		allticks += (t2 - t1);
		for( typename std::vector<worker>::iterator it = m_others->begin(); it != m_others->end(); ++it ) {
		    
		    if( get_res() != it->get_res() ) {
			printf( "meeeep: %d %zd %d != %d\n", j, it - m_others->begin(), get_res(), it->get_res() );
		    }
		}
		
		
// 		if( j % 1000 == 0 ) {
// 		    printf( "%d\n", j );
// 		}
		
	    } else {
		m_barrier->wait(m_slot);
	    }
	    
	    
	}
	
	if( m_master ) {
	    printf( "tpb: %.3f\n", allticks / double(BAR_N));
	    printf( "master time: %.3fs\n", (ivy_mike::gettime() - t1));
	}
	
    }
    void join() {
	m_thread->join();
    }
    
    volatile int get_res() {
	return m_res;
    }
    
};

//template<int N_THR, int N, int M, int PIN, class my_barrier>
static void testcase1() {
    my_barrier barrier(BAR_N_THR);
    
    volatile int *fence = (volatile int *) my_malloc( sizeof(int) );
    *fence = -1;

    std::vector<worker> ws;
    //ws.reserve(8);
    
    srand(1234);
    boost::array<float,BAR_M> v;
    for( boost::array<float,BAR_M>::iterator it = v.begin(); it != v.end(); ++it ) {
	*it = rand() / 1000.0;
    }
    
    printf( "setup: N_THR: %d\n", BAR_N_THR );
    double t1 = ivy_mike::gettime();
    
    {
	// this code is used to identify the current barrier type for additional checking.
	// it can be removed if  __cxa_demangle does not work. It should work on any
	// serious c++ compiler... 
	
	int     status;

	char *rn1 = abi::__cxa_demangle(typeid(barrier).name(), 0, 0, &status);
	//char *rn2 = abi::__cxa_demangle(typeid(barrier2).name(), 0, 0, &status);
	printf( "setup: barrier types: %s\n", rn1 );
	
	free( rn1 );
// 	free( rn2 );
    }
    printf( "setup: N: %d, M: %d, PIN: %d\n", BAR_N, BAR_M, BAR_PIN );
    
    for( int i = 0; i < BAR_N_THR; i++ ) {
	bool is_master = (i == 0);
	//ws.push_back( worker<N,M,PIN,my_barrier>( &barrier, fence, &ws, i, is_master ) );
	ws.push_back( worker( &barrier, fence, &ws, i, v.c_array(), is_master ) );
    }
    
//     for( typename std::vector<worker<N,M,PIN,my_barrier> >::iterator it = ws.begin(); it != ws.end(); ++it ) {
    for( typename std::vector<worker>::iterator it = ws.begin()+1; it != ws.end(); ++it ) {
	it->start();

    }
    
    ws.begin()->run();
    //barrier.wait();
    
//     for( typename std::vector<worker<N,M,PIN,my_barrier> >::iterator it = ws.begin(); it != ws.end(); ++it ) {
    for( typename std::vector<worker>::iterator it = ws.begin()+1; it != ws.end(); ++it ) {
	it->join();

    }
    printf( "time: %.3f\n", ivy_mike::gettime() - t1 );
}
 
int main( int argc, char *argv[]) {
    cpu_set_t cpuset;

    // pin master thread to core 0 for initialization
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);

    if(pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
    {
	printf("\n\nThere was a problem finding a physical core for the master thread initilization.\n");
	
	assert(0);
    }

    
    
    
    {
	
	ticks t1 = getticks();
	double tend = ivy_mike::gettime() + 1.0;
	while( ivy_mike::gettime() < tend ) {}
	ticks t2 = getticks();
	
	printf( "ticks per second: %zd\n", size_t( t2 - t1 ));
	
    }
//     const int N_THR = 4;
//     const int N = 20000000*4;
//     const int M = 0;
//     const int PIN = 1;
    int bt = 1;
    if( argc > 1 ) {
	bt = atoi(argv[1]);
    }

    testcase1();

}