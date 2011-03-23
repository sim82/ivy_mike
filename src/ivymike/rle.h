#ifndef __ivy_mike__rle_h
#define __ivy_mike__rle_h

static inline void rle_enc( boost::dynamic_bitset<> &bs, std::vector<uint16_t> &enc ) {
    
    
    boost::dynamic_bitset<> bsi = bs;
    bsi.flip();
    boost::dynamic_bitset<> *cs = &bs;
    
    int64_t last_one = 0;
    int64_t next_one = cs->find_first();
    
    while( next_one != bs.npos ) {
        enc.push_back( next_one - last_one );
        last_one = next_one;
        
        if( cs == &bs ) {
            
            cs = &bsi;
        } else {
            
            cs = &bs;
        }
        
        next_one = cs->find_next(last_one);
    }
    enc.push_back( bs.size() - last_one );
    
}

static inline void rle_dec( std::vector<uint16_t> &enc, boost::dynamic_bitset<> &dec ) {

    bool what = false;
    
    for( std::vector< uint16_t >::const_iterator it = enc.begin(); it != enc.end(); ++it ) {
        for( int i = 0; i < *it; i++ ) {
            dec.push_back(what);
        }
        
        what = !what;
    }
    
}

#endif