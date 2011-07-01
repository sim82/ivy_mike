#ifndef __IvyMike_time_h
#define __IvyMike_time_h

#include <iostream>
#include <numeric>

namespace ivy_mike {

double gettime(void);


class timer {
    double m_st;
    
public:
    timer() : m_st(gettime()) {};
    
    double elapsed() const {
        return gettime() - m_st;
    }
    
    
};


class perf_timer {
    const static size_t n_int = 16;
    size_t cur_int;
    double int_ticks[n_int];
    
    static double my_getticks();
public:  
    perf_timer() : cur_int(0) {
        add_int();
    }
    
    void add_int() {
        if( cur_int >= n_int - 1 ) {
            return;
        }
        int_ticks[cur_int] = my_getticks();
        ++cur_int;
    }
    
    void print() {
        double sum_nticks = 0.0;
        
        for( size_t i = 1; i < cur_int; ++i ) {
            sum_nticks += int_ticks[i] - int_ticks[i-1];
        }
        for( size_t i = 1; i < cur_int; ++i ) {
            double d = int_ticks[i] - int_ticks[i-1];
            
            std::cout << "int " << i - 1 << ": " << d << " (" << d / sum_nticks * 100 <<  "%)\n";
        }
    }
    
};

}
#endif