#ifndef __ivy_mike__thread_h
#define __ivy_mike__thread_h

// WARNING: this implementation is not very exception safe. use boost if you are interested in correctness...

#ifdef WIN32
#error ivy_mike threads not available on windows
#endif

#include <pthread.h>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cerrno>
namespace ivy_mike {
class thread {
    
public:
    typedef pthread_t native_handle_type;
private:
    native_handle_type m_thread;
    bool m_valid_thread;
    
    thread( const thread &other );
    const thread &operator=( const thread &other );
    
    template<typename callable>
    static void *call( void *f ) {
        std::auto_ptr<callable> c(static_cast<callable *>(f));
        (*c)();
       
        return 0;
    }
    
public:
    
    thread() : m_valid_thread( false ) {
        
    }
    
    template<typename callable>
    thread( const callable &c ) {
        // UUHM, this code looks a bit clumsy, but I could not figure out anything better without cheating (=looking at the boost::thread code)
        // TODO: pthread error handling!
        int ret = pthread_create( &m_thread, 0, call<callable>, new callable(c) );
        
        if( ret == EAGAIN ) {
            throw std::runtime_error( "could not create thread: resource_unavailable_try_again" );
        } else if( ret != 0 ) {
            throw std::runtime_error( "could not create thread: internal error" );
        } else {
            m_valid_thread = true;
        }
    }
    
    ~thread() {
        // this (=nothing) is actually the right thing to do (TM) if I read 30.3.1.3 if the iso c++0x standard corrctly
        if( joinable() ) {
            std::cerr << "ivy_mike::thread warning: destructor of joinable thread called. possible ressource leak\n";
        }
    }
    
    void swap( thread &other ) {
        std::swap( m_thread, other.m_thread );
        std::swap( m_valid_thread, other.m_valid_thread );
    }
    
    bool joinable() {
        return m_valid_thread;
    }
    
    void join() {
        
        if( joinable() ) {
            void *rv;
            pthread_join(m_thread, &rv );
            m_valid_thread = false;
        }
    }
    
    native_handle_type native_handle() {
        if( !joinable() ) {
            throw std::runtime_error( "native_handle: thread not joinable" );
        }
        return m_thread;
    }
};

class thread_group {
    std::vector<ivy_mike::thread *> m_threads;
    
public:
    template<typename callable>
    void create_thread( const callable &c ) {
        ivy_mike::thread *t = new ivy_mike::thread( c );
        
        m_threads.push_back(t);
    }
    
    
    void join_all() {
        for( std::vector<ivy_mike::thread *>::iterator it = m_threads.begin(); it != m_threads.end(); ++it ) {
            (*it)->join();
            
            delete (*it);
        }
        
        m_threads.resize(0);
    }
    
    size_t size() {
        return m_threads.size();
    }
};

static void swap( thread &t1, thread &t2 ) {
    t1.swap(t2);
}

class mutex {
    pthread_mutex_t m_mtx;

public:
    
    mutex() {
        pthread_mutex_init( &m_mtx, 0);
    }
    ~mutex() {
        pthread_mutex_destroy(&m_mtx);
    }
    
    void lock() {
        pthread_mutex_lock(&m_mtx);
    }
    
    void unlock() {
        pthread_mutex_unlock(&m_mtx);
    }
};


template <typename mtx_t>
class lock_guard {
    mtx_t &m_mtx;
    
public:
    lock_guard( mtx_t &mtx ) 
     : m_mtx(mtx)
    {
        m_mtx.lock();
    }
    
    ~lock_guard() {
        m_mtx.unlock();
    }
};
}


#endif