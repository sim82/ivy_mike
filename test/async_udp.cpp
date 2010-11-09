#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread.hpp>


using namespace boost::asio::ip;

using boost::asio::io_service;


namespace {

struct recv_handler {
    struct state {
	std::vector<uint8_t> m_rv;
	volatile int lock;
	
	state() : m_rv(2048), lock(0) {};
	
    };
    
    boost::shared_ptr<state> m_impl;
    
    //recv_handler( state *impl ) : m_impl(impl) {}
    recv_handler() : m_impl(new state()) {}
    
    void operator() (const boost::system::error_code& ec, std::size_t bytes_transferred ) 
    {
	std::vector<uint8_t>::iterator it = m_impl->m_rv.begin();
	
// 	char *xxx = 0;
// 	*xxx = 666;
	
	printf( "recved: %s %zd %d %d %d %d\n", ec.message().c_str(), bytes_transferred, *it++, *it++, *it++, *it++ );
	std::cout << "thread: " << boost::this_thread::get_id() << "\n";
	m_impl->lock = 1;
    }
    inline bool islocked() {
	return m_impl->lock == 0;
    }
    
};
    
    
std::vector<uint8_t> rv(2048);
volatile int sem = 0;
void receive( const boost::system::error_code& ec, std::size_t bytes_transferred ) {
    printf( "recv: %zd\n", bytes_transferred );
    sem = 1;
}
    
}

int main() {
    io_service ios;
    udp::socket ssock(ios);
    udp::socket rsock(ios);
    
    udp::endpoint oep( boost::asio::ip::address::from_string("131.159.28.113"), 21845 );
    udp::endpoint lep;
    lep.port(26001);
    
    rsock.open(udp::v4());
    rsock.bind(lep);
    
    ssock.open(udp::v4());
  
    recv_handler rh;
    {
	io_service::work sentinel(ios);
	
	int thread_count = 1;
	std::list<boost::thread*> threads;
	while (thread_count-- > 0)
	{
	    boost::thread* new_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, &ios));
	    threads.push_back(new_thread);
	}
	std::vector<uint8_t> sv(8);
	
//	rsock.async_receive(boost::asio::buffer(rv), receive );
	rsock.async_receive(boost::asio::buffer(rh.m_impl->m_rv), rh );
	ssock.send_to( boost::asio::buffer(sv), oep );
	
	
	//while( sem == 0 ) {}
	
	while( rh.islocked() ) {}
    }
    
}