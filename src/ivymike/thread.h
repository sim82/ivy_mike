#ifndef __ivy_mike__thread_h
#define __ivy_mike__thread_h

#ifdef WIN32
#error ivy_mike threads not available on windows
#endif

#include <pthread.h>



namespace ivy_mike {
class thread {
    pthread_t m_thread;
    
    
    thread( const thread &other );
    const thread &operator=( const thread &other );
    
    template<typename callable>
    void *call( void *f ) {
        
    }
    
public:
    template<typename callable>
    thread( const callable &c ) {
        pthread_create( &m_thread, 0, )
        
    }
    
};



}


#endif