#include <cstdio>
#include <boost/filesystem.hpp>
#include <deque>
#include <boost/concept_check.hpp>

namespace bf = boost::filesystem;

int main() {
    bf::path home( "/home/sim" );
    
//    
    std::deque<bf::path> todo_list;
    todo_list.push_back(home);
    class bla {
    public:
	int operator()() {
	    return 2;
	    
	}
    };
    
    
    bla b;
    
    printf( "bla: %d\n", b());
    
    while( todo_list.size() > 0 ) {
    
	bf::path cur = todo_list.front();
	todo_list.pop_front();
	
	printf( "dir: %s (%zd)\n", cur.file_string().c_str(), todo_list.size() ); 
	
	std::vector<std::string> dir_list;
	
	
	size_t ct = 0;
	try {
	    for( bf::directory_iterator it( cur ), end; it != end; ++it ) {
	// 	bf::path p = it->path();
		
	// 	it->sta
		if( !bf::is_directory( it->status() )) {
	//	    dir_list.push_back(it->path().filename());
		    ct++;
		} else if( !bf::is_symlink( it->status() )) {
		    todo_list.push_back(it->path());
		}
	// 	printf( "it: %s\n", p.file_string().c_str() );
	    }
	} catch( bf::filesystem_error x ) {
	    
	    printf( "boost exception\n" );
	    x.what();
	}
    
// 	std::sort( dir_list.begin(), dir_list.end() );
	
	printf( "%zd files\n", ct );
//     
//     
// 	for( std::vector< std::string >::const_iterator it = dir_list.begin(); it != dir_list.end(); ++it ) {
// 	    printf( "file: %s\n", it->c_str() );
// 	    
// 	}
    
    }    
    
}