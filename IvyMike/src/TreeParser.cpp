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


#include <auto_ptr.h>
#include "ivymike/TreeParser.h"
#include <boost/timer.hpp>
#include <iostream>

using std::cout;

using namespace ivymike;

TreeParserMS::LN *TreeParserMS::LN::create( ln_pool &pool ) {
    
    LN *n = pool.alloc();
    n->next = pool.alloc();
    n->next->next = pool.alloc();
    n->next->next->next = n;
    n->data.reset(new AData());
    n->next->data = n->data;
    n->next->next->data = n->data;
    return n;
    
//     LN *n = new LN();
//     n->next = new LN();
//     n->next->next = new LN();
//     n->next->next->next = n;
//     n->data = n->next->data = n->next->next->data = new AData();
// 
//     return n;

}


int main() {
//     getchar();
    //ivymike::TreeParser tp( "./RAxML_bipartitions.1604.BEST.WITH" );
    
    boost::timer t;
    ivymike::TreeParserMS::ln_pool pool;
    ivymike::TreeParserMS tp( "/space/newPNAS/PNAS.ntree", pool );
    ivymike::TreeParserMS::LN * n = tp.parse();
    printf( "n: %f %d\n", n->backLen, n->data->isTip );
    
    assert( n->next->back != 0 && n->next->next->back != 0 );
    
    n->next->back->back = n->next->next->back;
    n->next->next->back->back = n->next->back;
    n = n->next->back;
    
    
    {
        boost::timer t2;
        pool.clear();
        pool.mark(n);
        pool.sweep();
        
        cout << t2.elapsed() << std::endl;
    }
    
    
    {
        boost::timer t2;
        pool.clear();
     //   pool.mark(n);
        pool.sweep();
        
        cout << t2.elapsed() << std::endl;
    }
    //ivymike::LN *n = tp.parse();
    
//     getchar();
    //ivymike::LN::free( n );
//     delete n;
//     getchar();
    
    cout << t.elapsed() << std::endl;
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