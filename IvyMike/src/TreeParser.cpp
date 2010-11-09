#include <auto_ptr.h>
#include "ivymike/TreeParser.h"
#include <boost/timer.hpp>
#include <iostream>

using std::cout;

int main() {
//     getchar();
    //ivymike::TreeParser tp( "./RAxML_bipartitions.1604.BEST.WITH" );
    
    boost::timer t;
    
    ivymike::TreeParser tp( "/space/newPNAS/PNAS.ntree" );
    std::auto_ptr<ivymike::TreeParser::LN> n(tp.parse());
    
    //ivymike::LN *n = tp.parse();
    printf( "n: %f %d\n", n->backLen, n->data->isTip );
//     getchar();
    //ivymike::LN::free( n );
//     delete n;
//     getchar();
    
    cout << t.elapsed() << std::endl;
}