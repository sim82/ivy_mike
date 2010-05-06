#include "MultipleAlignment.h"


MultipleAlignment* MultipleAlignment::loadPhylip(const char* name) {
    std::ifstream is(name);

    size_t nTaxon;
    size_t seqLen;

    is >> nTaxon;
    is >> seqLen;

    printf( "header: %zd %zd\n", nTaxon, seqLen );

    size_t n = 0;

    MultipleAlignment *ma = new MultipleAlignment();//nTaxon, seqLen);
    while ( !is.eof() ) {
        std::string name;
        std::string seq;

        is >> name;
        is >> seq;

        ma->m_names.push_back(name);
        ma->m_data.push_back(seq);
        //printf( "name: %s\n", name.c_str() );
        n++;
    }

    assert( n == nTaxon );
    printf( "n: %zd\n", n );

    return ma;
}
