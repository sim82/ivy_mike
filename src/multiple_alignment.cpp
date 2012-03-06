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

#include <fstream>
#include <cassert>
#include <stdexcept>

#include "ivymike/multiple_alignment.h"

using namespace ivy_mike;


bool multiple_alignment::load_phylip( std::istream &is ) {
    size_t nTaxon;
    size_t seqLen;
    
    if( !is.good() ) {
        throw std::runtime_error( "cannot read phylip file" );
    }

    is >> nTaxon;
    is >> seqLen;

//     printf( "header: %zd %zd\n", nTaxon, seqLen );

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