#include <cstdio>

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/array.hpp>

#include <fstream>
#include <fcntl.h>
using namespace boost;

struct test {
    int v;
    array<int,32>a;
    
    
    
};

int main() {
    interprocess::managed_mapped_file mfile(interprocess::open_or_create, "/scratch/MyMappedFile",  128 * 1024 * 1024 * 10);           //Mapped file size

    for( int i = 0; i < 1000000 * 5; i++ ) {
    
        if( i % 1024 == 0 ) {
            printf( "%d\n", i );    
            
        }
        
        std::stringstream s;
        s << "test" << i;
        test *t = mfile.find_or_construct<test>( s.str().c_str() )();
        assert( t != 0 );
        
        t->a[0] = i;
        
    }
    mfile.flush();
    
}    


static void write( interprocess::managed_mapped_file &mfile ) {
    for( int i = 0; i < 10; i++ ) {
        std::stringstream s;
        s << "test" << i;
        
        test* t1 = mfile.construct<test>(s.str().c_str())();
        std::fill( t1->a.begin(), t1->a.end(), 666 + i );
    }
    
}


static void read( interprocess::managed_mapped_file &mfile ) {
    for( int i = 0; i < 10; i++ ) {
        std::stringstream s;
        s << "test" << i;
        
        test* t1 = mfile.find<test>(s.str().c_str()).first;
        printf( "%d: %d\n", i, t1->a[0] );
    }
}

int mainxx( int argc, char *argv[] ) {
    

    
    if( argc == 1 ) {
        interprocess::managed_mapped_file mfile(interprocess::create_only, "/tmp/MyMappedFile",  65536);           //Mapped file size
        write(mfile);
        read(mfile);
        getchar();
        
    } else {
        interprocess::managed_mapped_file mfile(interprocess::open_only, "/tmp/MyMappedFile");           //Mapped file size
        read(mfile);
        
    }
    
    
    
    
    
    
//     struct shm_remove
//     {
//         shm_remove() { interprocess::shared_memory_object::remove("MySharedMemory"); }
//         ~shm_remove(){ interprocess::shared_memory_object::remove("MySharedMemory"); }
//     } remover;
//    
//     interprocess::managed_shared_memory shm(interprocess::create_only,"MySharedMemory", 65536);
//     
//     shm.allocate(100);
//     
}

int mainx() {
    const char *filename = "/scratch/test.big";
    interprocess::file_mapping fm( filename, interprocess::read_only );
    
    off_t file_size;
    {
        std::ifstream ist(filename);
        ist.seekg(0, std::ios::end);
	file_size = ist.tellg();
    }
    int h = fm.get_mapping_handle().handle;
//     fcntl( h, F_NOCACHE );
    
    interprocess::mapped_region mr( fm, interprocess::read_only );
    
    char* p = (char*) mr.get_address();
    printf( "size: %zd\n", file_size );
    char xxx;
    for( off_t i = 0; i < file_size; i+=4096 ) {
	xxx += p[i];
	if( i % (4096 * 1024 * 10 ) == 0 ) {
	    printf( "%zd: %d\n", i, p[i] );
	}
    }
    
    printf( "xxx: %d\n", xxx );
    getchar();
}


