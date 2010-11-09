/*
 * Copyright (C) 2009 Simon A. Berger
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

#ifndef _ivy_MultilpleAlignment
#define _ivy_MultilpleAlignment
#include <vector>
#include <string>

class MultipleAlignment {
    std::vector<std::string> m_names;
    std::vector<std::string> m_data;
public:
	    
//     MultipleAlignment( size_t nTaxon, size_t seqLen ) : m_names(nTaxon), m_data(nTaxon)
//     {
//     }


    static MultipleAlignment *loadPhylip( const char *name ) ;
    
    inline size_t size() {
	return m_names.size();
    }
    
    inline const std::string &getName( size_t n ) {
	return m_names.at(n);
    }
    
    inline const std::string &getSequence( size_t n ) {
	return m_data.at(n);
    }
    
};


#endif