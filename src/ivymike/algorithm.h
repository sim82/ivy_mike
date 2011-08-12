#ifndef __ivy_mike__algorithm_h
#define __ivy_mike__algorithm_h

#include <algorithm>

namespace ivy_mike {
// the mighty twizzle algorithm (uhm, wouldn't the name binary_transform be more appropriate?)
template<typename iiter1_, typename iiter2_, typename oiter_, typename function_>
function_ binary_twizzle( iiter1_ first1, iiter1_ last1, iiter2_ first2, oiter_ res, function_ func ) {
    for( ; first1 != last1; ++first1, ++first2, ++res ) {
        *res = func( *first1, *first2 );
    }
    return func;
}

template<typename iiter1_, typename iiter2_, typename pred_>
size_t binary_count_if( iiter1_ first1, iiter1_ last1, iiter2_ first2, pred_ pred ) {
    size_t count = 0;
    while( first1 != last1 ) {
        if( pred( *first1++, *first2++) ) {
            ++count;
        }
    }
    
    return count;
    
}


// template<typename iiter1_, typename iiter2_, typename oiter_, typename function_>
// size_t binary_transform( iiter1_ first1, iiter1_ last1, iiter2_ first2, oiter_ res, pred_ pred ) {
//     size_t count = 0;
//     while( first1 != last1 ) {
//         if( pred( *first1++, *first2++) ) {
//             ++count;
//         }
//     }
//     
//     return count;
//     
// }

template<typename iiter1_, typename iiter2_>
size_t count_equal( iiter1_ first1, iiter1_ last1, iiter2_ first2 ) {
    return binary_count_if( first1, last1, first2, std::equal_to<typename iiter1_::value_type>() );
}


template<typename T>
  struct scaler_clamp {
  	T m_s;
  	T m_hi, m_lo;
  	scaler_clamp( T s, T lo, T hi ) : m_s(s), m_hi(hi), m_lo(lo) {}

  	T operator()( T v ) {
  		return std::min( m_hi, std::max( m_lo, v * m_s ));;
  	}

  };


}
#endif
