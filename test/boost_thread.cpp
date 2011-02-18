#include <cstdio>
#include <cstdlib>

#include <boost/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

class work {
    
    
public:
    int xxx;
  //  boost::shared_ptr<boost::asio::io_service::work> token;
    
    boost::promise<int> m_prom;
    void operator()() {
	printf( "working...\n" );
	
	int x = 0;
	for( int i = 0; i < 100000000; i++ ) {
	    x += i;
	}
	
	
	xxx = x;
	m_prom.set_value(xxx);
    }
    boost::unique_future<int>get_future() {
	m_prom.get_future();
    }
    

};

// typedef boost::numeric::ublas::ma
int main() {
    
    int xxx = 0;
    
    boost::asio::io_service sched;

    
    
    boost::thread_group threads;
    
    std::vector<boost::shared_ptr<work> > works;
    for( int i = 0; i < 10; i++ ) {
	
	works.push_back( boost::shared_ptr<work>( new work() ));
	
    }
    
    {
	
//	boost::shared_ptr<boost::asio::io_service::work> xwork(new boost::asio::io_service::work(sched));
	
	boost::asio::io_service::work xwork(sched);
	
	for( int i = 0; i < 2; i++ ) {
	    threads.create_thread( boost::bind(&boost::asio::io_service::run, &sched));
	}
	
// 	if( true )
// 	{
// 	    int x = 0;
// 	    for( int i = 0; i < 100000000; i++ ) {
// 		x += i;
// 	    }
// 	    xxx = x;
// 	}
	
	
	for( int i = 0; i < works.size(); i++ ) {
	//    works[i].token = xwork;
	    sched.post( boost::bind( &work::operator(), works[i] ));
	    //sched.post( works[i] );
	}
    }
    
    for( int i = 0; i < works.size(); i++ ) {
	
	boost::unique_future<int>f(works[i]->get_future());
	f.wait();
	int v = f.get();
	printf( "future %d: %d\n", i, v );
    }
    
    //sched.post( boost::bind( &boost::asio::io_service::stop, &sched ));
    printf( "joining\n" );
    threads.join_all();
    printf( "joined\n" );
    
    return xxx;
}


