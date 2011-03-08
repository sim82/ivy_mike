#ifndef __IvyMike_time_h
#define __IvyMike_time_h



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

}
#endif