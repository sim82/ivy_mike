#ifndef __ivy_mike__thread_h
#define __ivy_mike__thread_h

#ifdef WIN32
#error ivy_mike threads not available on windows
#endif

#include <pthread.h>
#include <vector>



namespace ivy_mike {
class thread {
    pthread_t m_thread;
    
    
    thread( const thread &other );
    const thread &operator=( const thread &other );
    
    template<typename callable>
    static void *call( void *f ) {
        callable *c = static_cast<callable *>(f);
        
        (*c)();
        delete c;
        return 0;
    }
    
public:
    template<typename callable>
    thread( const callable &c ) {
//         std::cout << "bla\n";
        
        callable *cp = new callable(c);
//         *cp = c;
        pthread_create( &m_thread, 0, call<callable>, cp );
    }
    
    
    void join() {
        void *rv;
        pthread_join(m_thread, &rv );
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