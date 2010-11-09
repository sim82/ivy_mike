#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
//#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <pthread.h>
namespace boost {
    class thread;
}


class spinlock {
private:  
    pthread_spinlock_t m_lock;
    
public:
    spinlock() {
	pthread_spin_init(&m_lock, PTHREAD_PROCESS_SHARED);
    }
    ~spinlock() {
	pthread_spin_destroy(&m_lock);
    }
    
    
    inline void lock() {
	pthread_spin_lock(&m_lock);
    }
    
    inline void unlock() {
	pthread_spin_unlock(&m_lock);
    }
    
};


class BackgroundReader {
    boost::scoped_ptr<boost::thread> m_thread;
    int m_socket;

    size_t m_size;
    boost::scoped_array<char> m_data;
    
    bool m_stop;
    
    size_t m_readPtr;
    size_t m_writePtr;
    
    size_t m_readCyc, m_writeCyc;
    void checkInvariant();
    void pushReadPtrToWriterCycle();
    spinlock m_lock;
//    boost::mutex m_lock;
    
    volatile int m_readbarrier;
    const static size_t m_maxsize = 2048;
    size_t m_nblocked;
    int m_nskipped1;
    int m_nskipped2;
    
public:
    BackgroundReader( int s, size_t size );
    virtual ~BackgroundReader();
    void run();
    ssize_t readPacket(void* buf, size_t size);
    void pushReadPtrPast(size_t arg1);
    void start();
    void join();
    
    
    static inline size_t getMaxsize() { return m_maxsize; }
};