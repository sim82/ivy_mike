#include <fstream>
#include <cassert>

#include "ivymike/multiple_alignment.h"

using namespace ivy_mike;


bool multiple_alignment::load_phylip( std::istream &is ) {
    size_t nTaxon;
    size_t seqLen;

    is >> nTaxon;
    is >> seqLen;

    printf( "header: %zd %zd\n", nTaxon, seqLen );

    size_t n = 0;


    while ( !is.eof() ) {
        std::string name;
        std::string seq;

        is >> name;
        is >> seq;

		if( is.eof() ) {
				
			break;
		}
		
        names.push_back(name);
        data.push_back( std::vector<uint8_t>(seq.begin(), seq.end()));
		
		
//         printf( "name: %s\n", name.c_str() );
        n++;
    }

    assert( n == nTaxon );
//     printf( "n: %zd\n", n );

    return n == nTaxon;
}

bool multiple_alignment::load_phylip( const char *name ) {
	std::ifstream is( name );
	
	if( is.good() ) {
		return load_phylip( is );
	} else {
		//throw std::runtime_error( "cannot open phylip file" );
		return false;
	}
	
}