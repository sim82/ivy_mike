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
using namespace ivy_mike;
// sadsafdasf
// int main( int argc, char* argv[]) {
//     const char *filename = argv[1];
//     assert( filename != 0 );
//     std::ifstream is( filename );
//     
//     
//     ivy_mike::sdf_full s(is);
//     
// }

namespace ivy_mike {
template class sdf_impl<sdf_int_full>;
template class sdf_impl<sdf_int_eco>;
}
// #include "ivymike/sdf2.h"
// 
// template class ivy_mike_2::sdf_impl<ivy_mike_2::sdf_int_full>;
// template class ivy_mike_2::sdf_impl<ivy_mike_2::sdf_int_eco>;