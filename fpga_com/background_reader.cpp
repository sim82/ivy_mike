/*
 * Copyright (C) 2011 Simon A. Berger
 * 
 *  This program is free software; you may redistribute it and/or modify its
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 */

#include <cstdio>
#include <csignal>
#include <sys/socket.h>
#include <boost/thread/locks.hpp>
#include <endian.h>

#include "background_reader.h"
#include "fpga_com.h"

static void handler( int signal ) {
    //printf( "signal: %d\n", signal );
}

background_reader::background_reader(int s, size_t size)
        : m_stop(false),
        m_socket(s),
        m_max_size(size),
        m_pq_mem(0),
        m_pq_max_mem(0),
        m_pq_max_depth(0),
        N_THREADS(1),
        m_run_barrier(N_THREADS + 1),
        m_threads_joined(false)
        //m_native_handle(0)
{

}

background_reader::~background_reader()
{
    if( !m_threads_joined ) {
        printf( "WARNING: background_reader::~background_reader(): threads not joined!\n" );
    }
}


void background_reader::start() {
    /// m_thread.reset( new boost::thread( boost::bind(&background_reader::run, this)));
   
    
   
    printf( "starting %zd background reader threads\n", N_THREADS );
    //boost::barrier run_barrier( N_THREADS + 1 );
    
    while( m_thread_group.size() < N_THREADS ) {
        m_thread_group.create_thread(boost::bind(&background_reader::run, this ));
    }
    
    m_run_barrier.wait();
}

void background_reader::run()
{
    // BIG-UGLY-KLUDGE-WARNING!
    // we have to install a signal handler, in order to disable the auto-restart behaviour of the recv call...
    // This is necessary to make the recv call interuptable (= return -1 on interrupt). Otherwise the
    // bg threads would either wait forever, or we would need at least two syscalls per recv (any solution that involves select).
    //
    // the 'handler' does nothing, as we are only interested in interrupting the recv...

    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction( SIGUSR1, &sa, 0 );

    //m_native_handle = pthread_self();
    
    {
        lock_guard_t lock( m_nh_mtx );
        m_native_handles.push_back( pthread_self() );
        
    }
    
    const size_t MPU = 9000;

    char buf[MPU];

    //run_barrier->wait();
    //run_barrier = 0;
    m_run_barrier.wait();
    
    while ( !m_stop ) {
        ssize_t s = recv( m_socket, buf, MPU, 0 );
        
//         if( s < 1024 && s != -1 ) {
//             printf( "recved term packet %p\n", (void*)pthread_self() );
//         }
        //printf( "recv: %zd\n", s );
        if ( s == -1 && errno == EINTR ) {
             printf( "interrupted\n" );
            
        } else {
            lock_guard_t lock( m_pq_mtx );
            m_pq.push_back(std::string(buf, buf+s));
            m_pq_mem += s;
            m_pq_max_mem = std::max( m_pq_mem, m_pq_max_mem );
            
            
            m_pq_max_depth = std::max( m_pq.size(), m_pq_max_depth );
        }
//         printf( "notify\n" );
        m_can_read_condition.notify_one();
    }
    
    printf( "background reader thread exit\n" );
}

ssize_t background_reader::block_recv(void* buf, size_t size)
{
    boost::unique_lock<mutex_t> lock( m_pq_mtx );

    while ( ! m_pq.size() > 0 && !m_stop ) {
//         printf( "cond_wait\n" );
        m_can_read_condition.wait(lock);
    }

    if( m_pq.size() == 0 ) {
        return -1;
    }
    
    std::string &pbuf = m_pq.front();

    m_pq_mem -= pbuf.size();
    
    
    ssize_t s = std::min( size, pbuf.size() );
    std::copy( pbuf.begin(), pbuf.end(), (char *)buf );
    m_pq.pop_front();

    return s;
}


ssize_t background_reader::poll()
{
    lock_guard_t lock( m_pq_mtx );
    if( m_pq.size() > 0 ) {
        return m_pq.front().size();
    } else {
        return -1;
    }
}


void background_reader::interrupt() {
    //boost::thread::native_handle_type h = m_thread->native_handle();
   // pthread_kill( m_native_handle, SIGUSR1 );
   
    lock_guard_t lock( m_nh_mtx );
    for( std::vector<pthread_t>::iterator it = m_native_handles.begin(); it != m_native_handles.end(); ++it ) {
        pthread_kill( *it, SIGUSR1 );
    }
   
}

void background_reader::join() {
   // m_thread->join();
   m_thread_group.join_all();

   printf( "pq max size (bytes): %zd\n", m_pq_max_mem );
   printf( "pq max depth (#packets): %zd\n", m_pq_max_depth );
   
   m_threads_joined = true;
}

ssize_t background_reader::purge()
{
    lock_guard_t lock( m_pq_mtx );
    
    ssize_t s = m_pq.size();
    m_pq.clear();

    return s;
}



// int mainx() {
//     fpga_con_t fc;
// 
//     fpga_con_init( &fc, "131.159.28.113", 12340, 12350 );
// 
// 
// 
//     background_reader bgr( fc.s, 1024 * 1024 * 10 );
// 
//     bgr.start();
// 
//     char buf[1024];
//     memset( buf, 0, 1024 );
//     
//     
//     const size_t rbuf_size = 10 * 1024;
//     char rbuf[rbuf_size];
// 
//     printf( "sleep\n" );
//   //  usleep( 2000000 );
//     printf( "close\n" );
//     //close( fc.s );
// 
//     //bgr.stop();
//     //bgr.interrupt();
//     //getchar();
// 
//    // bgr.join();
// 
//     printf( "joined\n" );
//     size_t n = 0;
//     for ( int i = 0; i < 10; i++ ) {
//         fpga_con_send( &fc, buf, 1024 );
//         printf( "sent\n" );
// 
// 
//         
//         while (true) {
//             size_t s = bgr.block_recv( rbuf, rbuf_size );
//             n++;
// //             printf( "recv: %zd\n", s );
//             if ( s < 1024 ) {
//                 break;
//             }
//         }
// 
//         printf( "recved: %zd\n", n );
//     }
//     
//     bgr.stop();
//     bgr.interrupt();
//     bgr.join();
//     printf( "bg reader joined. exit.\n" );
// }



static inline background_reader &get_bgr( fpga_bgr_t *cbgr ) {
    assert( cbgr != 0 );
    assert( cbgr->bgr_cpp != 0 );
    return *(static_cast<background_reader*>(cbgr->bgr_cpp));
}

void fpga_bgr_init( fpga_bgr_t *cbgr, int socket, size_t size ) {
    memset( cbgr, 0, sizeof( fpga_bgr_t ));
    
    background_reader *bgr = new background_reader(socket, size);
    cbgr->bgr_cpp = bgr;
}

void fpga_bgr_delete( fpga_bgr_t *cbgr ) {

    background_reader *bgr = static_cast<background_reader*>(cbgr->bgr_cpp);
    delete bgr;
    
}

void fpga_bgr_start( fpga_bgr_t *cbgr) {
    background_reader &bgr = get_bgr(cbgr);
    bgr.start();
}
ssize_t fpga_bgr_block_recv( fpga_bgr_t *cbgr, void *buf, size_t size ) {
    background_reader &bgr = get_bgr(cbgr);
    
    return bgr.block_recv(buf, size);
}
ssize_t fpga_bgr_poll( fpga_bgr_t *cbgr ) {
    background_reader &bgr = get_bgr(cbgr);
    
    return bgr.poll();
    
}
void fpga_bgr_stop_interrupt_join( fpga_bgr_t *cbgr) {
    background_reader &bgr = get_bgr(cbgr);
    
    bgr.stop();
    bgr.interrupt();
    bgr.join();
}


// template <typename T>
// inline static T swap_endian( T &v ) {
//     
//     T vo = v;
//     uint8_t *vb = (uint8_t*) &vo;
//     
//     for( size_t i = 0; i < sizeof(T) / 2; i++ ) {
//         std::swap(vb[i], vb[sizeof(T) - i - 1]);
//         
//     }
//     return vo;
// }

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
        return *((T*)&t);
    }
};


template<typename T>
struct swap_endian<T,4> {
    inline T operator()( T &v ) {
        uint32_t t = __bswap_32(*((uint32_t*)&v));
        return *((T*)&t);
    }
};

template<typename T>
struct swap_endian<T,8> {
    inline T operator()( T &v ) {
        uint64_t t = __bswap_64(*((uint64_t*)&v));
        return *((T*)&t);
    }
};




template<typename T,size_t N>
struct passthrough {
  
    inline T operator()( T &v ) {
        return v;
    }
    
};




// template <typename T>
// inline T passthrough( T &v ) {
//  
//     return v;
// }
// 

template <typename T,class F>
static bool fpga_bgr_recv_genv( fpga_bgr_t *cbgr, T *buf, size_t n ) {  
    background_reader &bgr = get_bgr(cbgr);
    
    
    
    T *buf_end = buf + n;
    T *ptr = buf;
    
    const size_t MPU = 9000;
    uint8_t rbuf[MPU];
    
    F filter;
    
    while( ptr < buf_end ) {
    
        
        ssize_t size = bgr.block_recv(rbuf, MPU);
        printf( "ptr: %p %zd %d %d %d %d\n", ptr, size, rbuf[0], rbuf[1], rbuf[2], rbuf[3] );
        assert( size > 0 );
        
        ssize_t ne = size / sizeof(T);
        ssize_t left = buf_end - buf;
        
        ssize_t to_copy = std::min( ne, left );
        //memcpy( ptr, rbuf, to_copy * sizeof(T));
        T* end = ptr + to_copy;
        for( int i = 0; ptr < end; ptr++, i++ ) {
            *ptr = filter(((T*)rbuf)[i]);
        }
        
        //ptr += to_copy;
    }
    
    return true;
}
    

bool fpga_bgr_recv_charv( fpga_bgr_t *bgr, char *buf, size_t n ) {
    return fpga_bgr_recv_genv<char,passthrough<char,1> >( bgr, buf, n );
}

bool fpga_bgr_recv_shortv( fpga_bgr_t *bgr, short *buf, size_t n ) {
    return fpga_bgr_recv_genv<short,swap_endian<short,2> >( bgr, buf, n );
}
    
bool fpga_bgr_recv_intv( fpga_bgr_t *bgr, int *buf, size_t n ) {
    return fpga_bgr_recv_genv<int,swap_endian<int,4> >( bgr, buf, n );
}

bool fpga_bgr_recv_floatv( fpga_bgr_t *bgr, float *buf, size_t n ) {
    return fpga_bgr_recv_genv<float,swap_endian<float,4> >( bgr, buf, n );
}

bool fpga_bgr_recv_doublev( fpga_bgr_t *bgr, double *buf, size_t n ) {
    return fpga_bgr_recv_genv<double,swap_endian<double,8> >( bgr, buf, n );
}




int main() {
    fpga_con_t fc;

    fpga_con_init( &fc, "131.159.28.113", 12340, 12350 );

    fpga_bgr_t bgr;

    fpga_bgr_init( &bgr, fc.s, 1024 * 1024 * 10 );
    
    fpga_bgr_start( &bgr );
    
    char buf[1024];
    memset( buf, 0, 1024 );
    int ibuf[1000];
    std::fill( ibuf, ibuf + 1000, 666 );
    
    
    const size_t rbuf_size = 10 * 1024;
    char rbuf[rbuf_size];

//     printf( "sleep\n" );
  //  usleep( 2000000 );
//     printf( "close\n" );
    //close( fc.s );

    //bgr.stop();
    //bgr.interrupt();
    //getchar();

   // bgr.join();

//     printf( "joined\n" );
    size_t n = 0;
    for ( int i = 0; i < 1; i++ ) {
        //fpga_con_send( &fc, buf, 1024 );
        fpga_con_send_intv( &fc, ibuf, 1000 );
        printf( "sent\n" );

        double iv[1000];
        bool succ = fpga_bgr_recv_doublev( &bgr, iv, 1000 );
        
        for( int i = 0; i < 1000; i++ ) {
            printf( "%.2f ", iv[i] );
            if( i % 20 == 19 ) {
                printf( "\n" );
            }
        }
        printf( "\n" );

        printf( "recved: %d\n", succ );
    }
    
    fpga_bgr_stop_interrupt_join( &bgr );
    printf( "bg reader joined. exit.\n" );
    
    fpga_bgr_delete( &bgr );
}