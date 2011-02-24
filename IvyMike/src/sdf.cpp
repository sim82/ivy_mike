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



#include "ivymike/sdf.h"
#include <cassert>
using ivy_mike::sdf;

int main( int argc, char* argv[]) {
    const char *filename = argv[1];
    assert( filename != 0 );
    std::ifstream is( filename );
    
    
    sdf s(is);
    
}