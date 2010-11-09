
#include <cfloat>
#include <vector>
#include <map>
#include <fstream>
#include "ParsVecCluster.h"




int main2( int argc, char *argv[] ) {
    std::string basedir( "/space/pa_testcase/pnas_new/" );

    std::string name( basedir + "pvs.ser" );
    std::string dname( basedir + "pvs_d.ser" );
    std::string bbd_name( basedir + "bbd.ser" );

//     initBits(bits,16);

    boost::multi_array<float,2> bbd;
    {
        std::ifstream is( bbd_name.c_str() );
        boost::archive::binary_iarchive ia(is);

        size_t w, h;
        ia >> w;
        ia >> h;

        bbd.resize( boost::extents[w][h] );
        ia >> boost::serialization::make_array<float>(bbd.data(), bbd.num_elements() );

// 	printf( "read bbd: %d %d\n", w, h );
    }

    std::ifstream is(name.c_str());

    boost::archive::binary_iarchive ia(is);

    std::map<int, std::vector<char> >pvs;
//     std::vector< std::vector<char> > pvsv;

    ia & pvs;

//     printf( "%zd %zd\n", pvs.size(), pvs[0].size() );

    ParsVecCluster pvc( pvs, bbd, 1000 );

    return 0;
}



int main( int argc, char *argv[] ) {
    int ret = main2( argc, argv );
    printf( "done\n" );
//     getchar();
    return ret;
}
