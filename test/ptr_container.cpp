#include <stdio.h>

#include <boost/ptr_container/ptr_vector.hpp>
#include <algorithm>


template <typename Iter>
void exec( Iter b, Iter e ) {
    while( b != e ) {
	(*b)();
	b++;
    }
}

class bla : boost::noncopyable {
  
//     bla(bla&);
//     bla& operator=(bla&);
	
    static int s_serial;
public:
    int m_x;
    int m_serial;
    bla(int x) : m_x(x), m_serial( s_serial++ ){}
    
    void operator()() {
	print();
    }
    void print() {
	printf( "this: %d %d %p\n", m_x, m_serial, this );
    }
};

int bla::s_serial = 0;

bla* new_clone( const bla& other ) {
    int x = other.m_x;
    return new bla( x );
}
int main() {
    
    
    boost::ptr_vector<bla> v;
    
    
    v.push_back(new bla(1));
    v.push_back(new bla(2));
    
    exec(v.begin(), v.end());
    std::auto_ptr<boost::ptr_vector<bla> > v2c = v.clone();
    boost::ptr_vector<bla> &v2 = *v2c;
    
    exec( v2.begin(), v2.end() );
    
}