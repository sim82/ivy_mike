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



#ifndef __ivy_mike__statistics_h
#define __ivy_mike__statistics_h

#include <stdexcept>

#include <algorithm>
#include <math.h>

namespace ivy_mike {
    
template <typename T>    
double mean( std::vector<T> &v ) {
    T sum = 0;
    
    for( typename std::vector<T>::iterator it = v.begin(); it != v.end(); ++it ) {
        sum += *it;
    }
    
    return double(sum) / v.size();
}

template <typename T>
double correlationx( std::vector<T> &v1, std::vector<T> &v2) {
    double m1 = mean(v1);
    double m2 = mean(v2);
    
    assert( v1.size() == v2.size() );
    
    double cov = 0;
    double var1 = 0;
    double var2 = 0;
    for( int i = 0; i < v1.size(); i++ ) {
        const double d1 = (v1[i] - m1);
        const double d2 = (v2[i] - m2);
        cov += d1 * d2;
        
        var1 += d1 * d1;
        var2 += d2 * d2;
    }
    return cov / (sqrt(var1) * sqrt(var2));
}


template <typename iter>    
double mean( const iter &vb, const iter &ve ) {
    typename iter::value_type sum = 0;
    
    for( iter it = vb; it != ve; ++it ) {
        sum += *it;
    }
    
    return double(sum) / (ve-vb);
}

template <typename iter>
double correlation( const iter &vb1, const iter &ve1, const iter &vb2, const iter &ve2) {
    double m1 = mean(vb1, ve1);
    double m2 = mean(vb2, ve2);
    
    
    if ((ve1-vb1) != (ve2-vb2)) {
        throw std::runtime_error( "iter ranges have different size" );
    }
//     assert( (ve1-vb1) == (ve2-vb2) );
    
    double cov = 0;
    double var1 = 0;
    double var2 = 0;
    for( iter it1 = vb1, it2 = vb2; it1 != ve1; ++it1, ++it2 ) {
        const double d1 = (*it1 - m1);
        const double d2 = (*it2 - m2);
        cov += d1 * d2;
        
        var1 += d1 * d1;
        var2 += d2 * d2;
    }
    return cov / (sqrt(var1) * sqrt(var2));
}

template <typename cont>
double correlation( const cont &v1, const cont &v2) {
    return correlation( v1.begin(), v1.end(), v2.begin(), v2.end() );
}




float covariance( std::vector<float> &v1, std::vector<float> &v2) {
    float m1 = mean(v1);
    float m2 = mean(v2);
    
    assert( v1.size() == v2.size() );
    
    float cov = 0;
    float var1 = 0;
    float var2 = 0;
    for( int i = 0; i < v1.size(); i++ ) {
        const float d1 = (v1[i] - m1);
        const float d2 = (v2[i] - m2);
        cov += d1 * d2;
    }
    return cov;
}
float std_dev( std::vector<float> &v ) {
    float m = mean(v);
    
    float var = 0;
    for( int i = 0; i < v.size(); i++ ) {
        const float d = (v[i] - m);
        var += d * d;
    }
    
    return sqrt(var);
}
    
    

    

}
#endif