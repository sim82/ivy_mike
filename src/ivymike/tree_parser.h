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

#ifndef __ivymike__tree_parser_h
#define __ivymike__tree_parser_h

#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <cctype>
#include "demangle.h"

#include "smart_ptr.h"
// #include <boost/weak_ptr.hpp>
#include <boost/intrusive/slist.hpp>
#include <list>
namespace ivy_mike {






namespace tree_parser_ms {
using namespace boost;
    
struct adata {

    bool isTip;
    std::string tipName;
    void setTipName( const std::string &name ) {
        tipName = name;
    }

    int tipSerial;
    void setTipSerial( int serial ) {
        tipSerial = serial;
    }

    double support;
    double getSupport() {
        return support;
    }
    void setSupport(double sup) {

        support = sup;
    }

    std::string nodeLabel;
    void setNodeLabel(const std::string &nl) {

        nodeLabel = nl;
    }

    static int s_serial;
    const int m_serial;
    
    adata() : isTip( false ), tipSerial(-1), support(-1), m_serial(s_serial++) {}

    virtual ~adata() {}
    
    virtual void visit() {}

    virtual void print( std::ostream &os ) const {
        if( isTip ) {
        
            os << "(TIP:" << m_serial << ": " << tipName.c_str() << ")";
        } else {
            os << "(INNER:" << m_serial << ")";
        }    
    }
    
    template<typename T>
    T* get_as() {
        if( !ivy_mike::isa<T>(*this) ) {
            throw std::runtime_error( "dynamic cast failed\n" );
        }
        
        return static_cast<T*>(this);
    }

};
    
inline std::ostream &operator<<(std::ostream &os, const adata &ad ) {
    ad.print( os );
    return os;
}


struct ldata {
    virtual ~ldata() {}
};


class ln_pool;

    
class lnode
    : public intrusive::slist_base_hook<> 

{

    lnode( const lnode &other ) {}
    const lnode &operator=(const lnode &other ) { return *this; }

public:
    static lnode *create( ln_pool &pool );

	
	
    lnode() : next(0), back(0), backLen(-1), backSupport(-1), mark(false), m_thisptr(this), towards_root(false) {}
    virtual ~lnode() {
// 		std::cout << "~lnode\n";
    }
    
    sptr::weak_ptr<lnode>get_smart_ptr() {
        return sptr::weak_ptr<lnode>(m_thisptr);
    }
    
    sptr::shared_ptr<lnode>dealloc() {
        sptr::shared_ptr<lnode>tmp(m_thisptr);
        m_thisptr.reset();
        return tmp;
    }
    sptr::shared_ptr<adata> m_data;
    sptr::shared_ptr<ldata> m_ldata;
    //LN *next;
    lnode *next;
    lnode *back;

    double backLen;
    double backSupport;
    std::string backLabel;

    bool mark;
    
    sptr::shared_ptr<lnode> m_thisptr;
    
    bool towards_root;
    
    
};



class node_data_factory {

public:
    virtual adata *alloc_adata() {
        return new adata();
    }
    
    
    virtual ldata *alloc_ldata() {
        return 0;
    }

    virtual ~node_data_factory() {}
};

class ln_pool 
{
     
    typedef intrusive::slist<lnode> lt;
    std::vector<lnode *> m_pinned_root;

    lt m_list;

    //sptr::shared_ptr<node_data_factory> m_ad_fact;
    std::auto_ptr<node_data_factory> m_ad_fact;
public:
    
    // this version takes ownership of fact!
    // TODO: change to unique_ptr when available!
    ln_pool( std::auto_ptr<node_data_factory> fact ) : m_ad_fact(fact) {}
    
    //ln_pool( sptr::shared_ptr<node_data_factory> fact ) : m_ad_fact(fact) {}
    ln_pool() : m_ad_fact(new node_data_factory) {}
    
    ~ln_pool() {
        clear();
        sweep();
    }
    node_data_factory &get_adata_factory() {
        return *m_ad_fact;
    }
    
    lnode *alloc() {
        lnode *n = new lnode();
        m_list.push_front( *n );
        return &m_list.front();
        
        //return new LN();
    }
    
    void mark( lnode *n ) ;
    
    void sweep() ;
    void clear() ;
    
    void pin_root( lnode *n );
    void unpin_root( lnode *n );
};


class ln_pool_pin {
	lnode * m_pin;
	ln_pool &m_pool;

public:
	ln_pool_pin( lnode *node, ln_pool &pool ) : m_pin( node ), m_pool(pool) {
		m_pool.pin_root(m_pin);
	}

	~ln_pool_pin() {
		if( m_pin != 0 ) {
			release();
		}
	}

	void release() {
		if( m_pin == 0 ) {
			throw std::runtime_error( "ln_pool_pin::release called with m_pin == 0" );
		}
		m_pool.unpin_root(m_pin);
		m_pin = 0;
	}


};

class parser {
public:

    //String input;
private:
    // input as char array
    typedef std::vector<char> id_t;
    id_t inputA;

    typedef id_t::iterator idi_t;
    idi_t ptr;


    int nLeafs;
    int nInnerNodes;
    ln_pool &m_pool;

    bool QUIET;
//      public TreeParser(String input) {
//      //      this.input = input;
//              this.inputA = input.toCharArray();
//              ptr = 0;
//      }


    void readFile(const char *f, std::vector<char> &data) ;
    void substring( const idi_t &from, const idi_t &to, std::string &out ) ;

    std::string substring( const idi_t from, const idi_t to ) ;

    void printLocation() ;

    void skipWhitespace() ;



    std::string parseBranchLabel() ;

    lnode *parseNode() ;
    idi_t findNext( idi_t pos, char c) ;

    bool isFloatChar(char c) ;

    idi_t findFloat(idi_t pos) ;

    double parseBranchLength() ;

    lnode *parseLeaf() ;

    idi_t findEndOfBranch(idi_t pos) ;

    bool isOneOf(char c, const char *cs) ;
    lnode *parseInnerNode() ;


public:
     /**
     * create an edge (=double link) between the two nodes and set branch length
     *
     * @param n1
     * @param n2
     * @param branchLen
     */
    static void twiddle( lnode *n1, lnode *n2, double branchLen, std::string branchLabel, double support ) ;

    
    
    parser(const char *f, ln_pool &pool ) : m_pool(pool) {
        QUIET = false;
        readFile(f, inputA);
        //  this.input = new String(inputA);
        ptr = inputA.begin();
    }
    /**
     * Call this after object construction to parse the complete tree,
     * @return pseudo root of the tree
     */

    lnode *parse() ;

};

inline void print_newick( lnode *node, std::ostream &os, bool root = true ) {
    if( node->m_data->isTip ) {
        os << node->m_data->tipName << ":" << node->backLen;
        if( !node->backLabel.empty() ) {
            os << "[" << node->backLabel << "]";
        }
    } else {
        os << "(";
        print_newick(node->next->back, os, false);
        os << ",";
        print_newick(node->next->next->back, os, false);
        if( root ) {
            os << ",";
            print_newick(node->back, os, false);
            os << ");";
        } else {
            os << "):" << node->backLen;
            
            if( !node->backLabel.empty() ) {
                os << "[" << node->backLabel << "]";
            }
        }
    }
}

inline lnode *towards_tree( lnode *node ) {
    if( !node->m_data->isTip || node->back != 0 ) {
        return node;
    } else if( node->next->back != 0 ) {
        return node->next;
    } else if( node->next->next->back != 0 ) {
        return node->next->next;
    } else {
        throw std::runtime_error( "cannot find valid back pointer." );
    }
}

inline lnode *next_non_tip( lnode *node ) {
    if( !node->m_data->isTip ) {
        return node;
    } else {
        if( node->back->m_data->isTip ) {
            throw std::runtime_error( "cannot find non-tip node: two connected tips." );
        }
        return node->back;
    }
}


} // namespace tree_parser_ms

}
#endif
