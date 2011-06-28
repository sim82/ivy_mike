#ifndef __ivy_mike__algorithm_h
#define __ivy_mike__algorithm_h

namespace ivy_mike {
// the mighty twizzle algorithm
template<typename iiter1_, typename iiter2_, typename oiter_, typename function_>
function_ binary_twizzle( iiter1_ first1, iiter1_ last1, iiter2_ first2, oiter_ res, function_ func ) {
    for( ; first1 != last1; ++first1, ++first2, ++res ) {
        *res = func( *first1, *first2 );
    }
    return func;
}

template<typename iiter1_, typename iiter2_>
size_t count_equal( iiter1_ first1, iiter1_ last1, iiter2_ first2 ) {
    size_t count = 0;
    for( ; first1 != last1; ++first1, ++first2 ) {
        if( *first1 == *first2 ) {
            ++count;
        }
    }
    
    return count;
}
}
#endif