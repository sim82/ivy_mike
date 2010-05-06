#ifndef _ivy_MultilpleAlignment
#define _ivy_MultilpleAlignment
#include <vector>
#include <string>

class MultipleAlignment {
    std::vector<std::string> m_names;
    std::vector<std::string> m_data;
public:
	    
//     MultipleAlignment( size_t nTaxon, size_t seqLen ) : m_names(nTaxon), m_data(nTaxon)
//     {
//     }


    static MultipleAlignment *loadPhylip( const char *name ) ;
    
    inline size_t size() {
	return m_names.size();
    }
    
    inline const std::string &getName( size_t n ) {
	return m_names.at(n);
    }
    
    inline const std::string &getSequence( size_t n ) {
	return m_data.at(n);
    }
    
};


#endif