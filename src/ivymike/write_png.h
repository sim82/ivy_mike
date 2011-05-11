#include <iostream>
#include <boost/multi_array.hpp>


namespace ivy_mike {

template<typename ma_t>
static void write_png( const ma_t &ma, std::ostream &os ) {
    
    // the boost::multi_array iterators are way too complicated...
    
    typedef typename ma_t::value_type::value_type v_t;
    
    v_t max_value = 0;
    v_t min_value = 0;
    for( typename ma_t::const_iterator row_it = ma.begin(); row_it !=  ma.end(); ++row_it ) {
        v_t max_col = *(std::max_element( (*row_it).begin(), (*row_it).end() ));
        v_t min_col = *(std::min_element( (*row_it).begin(), (*row_it).end() ));
        
        max_value = std::max( max_value, max_col );
        min_value = std::min( min_value, min_col );
    }
    
    float rng = max_value-min_value;
    
    os << "P2\n";
    os << ma.size() << " " << ma[0].size() << "\n";
    os << 255 << "\n";
    for( typename ma_t::const_iterator row_it = ma.begin(); row_it !=  ma.end(); ++row_it ) {
        for( typename ma_t::value_type::const_iterator col_it = row_it->begin(); col_it !=  row_it->end(); ++col_it ) {
            os << int((*col_it - min_value) / rng * 255) << " ";
        }
        os << "\n";
    }
    
}

}