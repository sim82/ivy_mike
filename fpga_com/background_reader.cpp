/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <cstdio>
#include <csignal>
#include <sys/socket.h>
#include <boost/thread/locks.hpp>

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
        N_THREADS(16),
        m_run_barrier(N_THREADS + 1)
        //m_native_handle(0)
{

}

background_reader::~background_reader()
{

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
        
        if( s < 1024 && s != -1 ) {
            printf( "recved term packet %d\n", pthread_self() );
        }
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
}


int main() {
    fpga_con_t fc;

    fpga_con_init( &fc, "131.159.28.113", 12340, 12350 );



    background_reader bgr( fc.s, 1024 * 1024 * 10 );

    bgr.start();

    char buf[1024];
    memset( buf, 0, 1024 );
    
    
    const size_t rbuf_size = 10 * 1024;
    char rbuf[rbuf_size];

    printf( "sleep\n" );
  //  usleep( 2000000 );
    printf( "close\n" );
    //close( fc.s );

    //bgr.stop();
    //bgr.interrupt();
    //getchar();

   // bgr.join();

    printf( "joined\n" );
    size_t n = 0;
    for ( int i = 0; i < 10; i++ ) {
        fpga_con_send( &fc, buf, 1024 );
        printf( "sent\n" );


        
        while (true) {
            size_t s = bgr.block_recv( rbuf, rbuf_size );
            n++;
//             printf( "recv: %zd\n", s );
            if ( s < 1024 ) {
                break;
            }
        }

        printf( "recved: %zd\n", n );
    }
    
    bgr.stop();
    bgr.interrupt();
    bgr.join();
    printf( "bg reader joined. exit.\n" );
}
