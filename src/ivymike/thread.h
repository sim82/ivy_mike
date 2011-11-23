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


#ifndef __ivy_mike__thread_h
#define __ivy_mike__thread_h

// WARNING: these thread implementations are not very exception safe. use boost if you are interested in correctness...

#ifdef WIN32
// funny win32 api fact: you can deactivate everything and the thread implementation still works.
// This clearly proves that threads are the only part of the win32 api which does not suck. What does the "32" stand for, btw.?
#include "ivymike/disable_shit.h"

#include <windows.h> // oh noes, the dreadful msdn advise from hell: "Header Winbase.h (include Windows.h)"
// And I heard a voice in the midst of the four beasts,
// And I looked and behold: a pale horse.
// And his name, that sat on him, was Windows.h.
// And Instant Namespace Pollution followed with him.

//#if defined(WIN32) && !defined(_M_X64)
//#define GODAWFULLY_STUPID_32_BIT_WINDOWS_CALLING_CONVENTION __stdcall
//#else
//#define GODAWFULLY_STUPID_32_BIT_WINDOWS_CALLING_CONVENTION
//#endif


namespace ivy_mike {
// TODO: the windows implementation is completely untested, but seems to work for pw_dist


class thread {
public:
	typedef HANDLE native_handle_type;


private:
	HANDLE m_thread;

	thread( const thread &other );
	const thread& operator=(const thread &other );

	template<typename callable>
    static DWORD __stdcall call( void *f ) {
        std::auto_ptr<callable> c(static_cast<callable *>(f));
        try {
            (*c)();
        } catch( std::runtime_error x ) {
            std::cerr << "uncaught std::runtime_error in ivy_mike::thread:\n" << x.what() << std::endl;
//             std::cerr << x.what() << std::endl;
            
            throw;
        } catch( std::exception x ) {
            std::cerr << "uncaught std::exception in ivy_mike::thread:\n" << x.what() << std::endl;
//             std::cerr << x.what() << std::endl;
            
            throw;
        }
       
        return 0;
    }
    
public:

	thread() : m_thread(NULL) {}

	template<typename callable>
    thread( const callable &c ) {
		m_thread = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            call<callable>,       // thread function name
            new callable(c),          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier 

        if( m_thread == NULL) {
            throw std::runtime_error( "could not create thread: don't know why." );
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
        //std::swap( m_valid_thread, other.m_valid_thread );
    }
    
    bool joinable() {
        return m_thread != NULL;
    }
    
    void join() {
        
        if( joinable() ) {

			WaitForSingleObject( m_thread, INFINITE );

            m_thread = NULL;
        }
    }
    
    native_handle_type native_handle() {
        if( !joinable() ) {
            throw std::runtime_error( "native_handle: thread not joinable" );
        }
        return m_thread;
    }
};



class mutex {
    CRITICAL_SECTION m_cs;

public:
    
    mutex() {
		InitializeCriticalSection( &m_cs );
    }
    ~mutex() {
		DeleteCriticalSection( &m_cs );
    }
    
    inline void lock() {
		EnterCriticalSection( &m_cs );
    }
    
    inline void unlock() {
		LeaveCriticalSection( &m_cs );
    }
};



}

#else

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
        
         try {
            (*c)();
        } catch( std::runtime_error x ) {
            std::cerr << "uncaught std::runtime_error in ivy_mike::thread: " << x.what() << std::endl;
//             std::cerr << x.what() << std::endl;
            
            throw;
        } catch( std::exception x ) {
            std::cerr << "uncaught std::exception in ivy_mike::thread" << std::endl;
//             std::cerr << x.what() << std::endl;
            
            throw;
        }
       
        return 0;
    }
    
public:
    
    thread() : m_valid_thread( false ) {
        
    }
    
    template<typename callable>
    thread( const callable &c ) {
        // dynamically allocating a copy of callable seems to be the only way to get the object into the 
		// thread start function without specializing the whole thread object...
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



class mutex {
    pthread_mutex_t m_mtx;

public:
    
    mutex() {
        pthread_mutex_init( &m_mtx, 0);
    }
    ~mutex() {
        pthread_mutex_destroy(&m_mtx);
    }
    
    inline void lock() {
        pthread_mutex_lock(&m_mtx);
    }
    
    inline void unlock() {
        pthread_mutex_unlock(&m_mtx);
    }
};



}
#endif

namespace ivy_mike {
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

inline void swap( thread &t1, thread &t2 ) {
    t1.swap(t2);
}


template <typename mtx_t>
class lock_guard {
    mtx_t &m_mtx;
    

    lock_guard( const lock_guard & );
    lock_guard &operator=(const lock_guard & );

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
