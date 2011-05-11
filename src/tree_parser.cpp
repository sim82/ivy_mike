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


#include <memory>
#include "ivymike/tree_parser.h"

#include <iostream>

using std::cout;

using namespace ivy_mike;

int tree_parser_ms::adata::s_serial = 0;

tree_parser_ms::lnode *tree_parser_ms::lnode::create( ln_pool &pool ) {
    
    lnode *n = pool.alloc();
    n->next = pool.alloc();
    n->next->next = pool.alloc();
    n->next->next->next = n;
    
    n->m_ldata.reset(pool.get_adata_factory().alloc_ldata());
    n->next->m_ldata.reset(pool.get_adata_factory().alloc_ldata());
    n->next->next->m_ldata.reset(pool.get_adata_factory().alloc_ldata());
    n->m_data.reset( pool.get_adata_factory().alloc_adata() );
    n->next->m_data = n->m_data;
    n->next->next->m_data = n->m_data;
    return n;
    
//     LN *n = new LN();
//     n->next = new LN();
//     n->next->next = new LN();
//     n->next->next->next = n;
//     n->data = n->next->data = n->next->next->data = new AData();
// 
//     return n;

}





// int main() {
// //     getchar();
//     //ivymike::TreeParser tp( "./RAxML_bipartitions.1604.BEST.WITH" );
//     
//     boost::timer t;
//     
//     ivymike::TreeParser tp( "/space/newPNAS/PNAS.ntree" );
//     std::auto_ptr<ivymike::TreeParser::LN> n(tp.parse());
//     
//     //ivymike::LN *n = tp.parse();
//     printf( "n: %f %d\n", n->backLen, n->data->isTip );
// //     getchar();
//     //ivymike::LN::free( n );
// //     delete n;
// //     getchar();
//     
//     cout << t.elapsed() << std::endl;
// }