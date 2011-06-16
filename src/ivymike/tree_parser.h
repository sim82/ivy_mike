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


#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <sstream>
#include <cctype>

#if WIN32
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>


namespace sptr = boost;
#else
#include <tr1/memory>
#include <memory>
namespace sptr = std::tr1;



#endif
// #include <boost/weak_ptr.hpp>
#include <boost/intrusive/slist.hpp>
#include <list>
namespace ivy_mike {


    
// class tree_parser {
// public:
//     struct ln {
// 
//         struct adata {
// 
//             bool isTip;
//             std::string tipName;
//             void setTipName( const std::string &name ) {
//                 tipName = name;
//             }
// 
//             int tipSerial;
//             void setTipSerial( int serial ) {
//                 tipSerial = serial;
//             }
// 
//             double support;
//             double getSupport() {
//                 return support;
//             }
//             void setSupport(double sup) {
// 
//                 support = sup;
//             }
// 
//             std::string nodeLabel;
//             void setNodeLabel(const std::string &nl) {
// 
//                 nodeLabel = nl;
//             }
// 
// 
// 
//             adata() : isTip( false ), tipSerial(-1), support(-1) {}
// 
// 
//         };
// 
//         static ln *create() {
// 
//             ln *n = new ln();
//             n->next = new ln();
//             n->next->next = new ln();
//             n->next->next->next = n;
//             n->data = n->next->data = n->next->next->data = new adata();
// 
//             return n;
// 
//         }
// 
//         //     static void freeRing( LN *n ) {
//         //       delete n->next->next;
//         //       delete n->next;
//         //       delete n->data;
//         //       delete n;
//         //     }
//         //
//         //     static void freeRec( LN *n ) {
//         //       if( n != 0 ) {
//         //           freeRec( n->next->back );
//         //           freeRec( n->next->next->back );
//         //           freeRing( n );
//         //       }
//         //     }
//         //     static void free( LN *n ) {
//         //       freeRec( n->back );
//         //       freeRec( n->next->back );
//         //       freeRec( n->next->next->back );
//         //       freeRing( n );
//         //     }
// 
//         ln() : data(0), next(0), back(0), backLen(-1), backSupport(-1) {}
//         ~ln() {
// 
//             // experiment: recursive destructor
//             // basic method: the 'deleter' (=a destructor invoking 'delete' on any other object ptr) is responsible
//             // to prevent the deleted object from double-deleting, by clearing its references.
// 
//             // WARNING: the destructor of LN will free the complete reachable tree!
// 
//             if ( back != 0 ) {
//                 // prevent 'back' from deleting 'this', by clearing the back reference
//                 back->back = 0;
//                 delete back;
//             }
//             if ( next != 0 ) {
//                 // prevent 'next' from deleting the shared AData object
//                 next->data = 0;
//                 if ( next->next != 0 ) {
//                     // prevent 'next->next' from deleting 'this' (==next->next->next)
//                     next->next->data = 0;
//                     next->next->next = 0;
//                 }
//                 delete next;
//             }
//             //if( data != 0 ) {
//             delete data;
//             //}
//         }
// 
//         adata *data;
//         ln *next;
//         ln *back;
// 
//         double backLen;
//         double backSupport;
//         std::string backLabel;
// 
// 
//     };
//     //String input;
// private:
//     // input as char array
//     typedef std::vector<char> id_t;
//     id_t inputA;
// 
//     typedef id_t::iterator idi_t;
//     idi_t ptr;
// 
// 
//     int nLeafs;
//     int nInnerNodes;
// 
// 
//     bool QUIET;
// //      public TreeParser(String input) {
// //      //      this.input = input;
// //              this.inputA = input.toCharArray();
// //              ptr = 0;
// //      }
// 
// 
//     void readFile(const char *f, std::vector<char> &data) {
// 
//         std::ifstream is(f);
//         is.seekg( 0, std::ios_base::end );
//         off_t size = is.tellg();
//         is.seekg( 0, std::ios_base::beg );
// 
//         data.resize( size );
// 
//         is.read( data.data(), size );
// 
//     }
//     void substring( const idi_t &from, const idi_t &to, std::string &out ) {
// //         return new String( Arrays.copyOfRange( inputA, from, to));
// 
//         out.clear();
// 
//         out.append(from, to );
//     }
// 
//     std::string substring( const idi_t from, const idi_t to ) {
// //         return new String( Arrays.copyOfRange( inputA, from, to));
// 
// 
// 
//         return std::string(from, to );
//     }
// 
//     void printLocation() {
//         if ( QUIET ) {
//             return;
//         }
// 
//         idi_t pos1 = std::max(inputA.begin(), ptr - 40);
//         idi_t pos2 = std::min(inputA.end(), ptr + 40);
// 
// 
// 
//         printf( "%s\n", substring(pos1, pos2).c_str());
// 
//         for (idi_t i = pos1; i < ptr; ++i) {
//             printf(" ");
//         }
//         printf("^\n");
//     }
// 
//     void skipWhitespace() {
//         while ( ptr != inputA.end() && std::iswspace(*ptr) ) {
//             ++ptr;
//         }
//         if ( ptr == inputA.end() ) {
// 
//             printLocation();
//             printf( "hit end of input while skipping white-spaces\n" );
//             throw std::exception();
//         }
// 
//     }
// 
// 
// 
//     std::string parseBranchLabel() {
//         if ( *ptr == '[' ) {
//             idi_t lstart = ptr;
//             ++ptr;
// 
// 
//             idi_t lend = findNext(ptr, ']' );
// 
//             ptr = lend+1;
// 
//             // labels have the form [blablabla], so the label content starts at lstart + 1
// 
//             if ( lend - (lstart+1) <= 0 ) {
//                 printLocation();
//                 throw (std::string("bad branch label: ") + substring(lstart, ptr) );
//             }
// 
//             return substring(lstart + 1, lend);
// 
// 
//         } else {
//             return std::string();
//         }
// 
// 
//     }
// 
//     ln *parseNode() {
// //         printf( "parseNode\n" );
//         skipWhitespace();
// 
//         // lookahead: determine node type
//         if ( *ptr == '(') {
//             return parseInnerNode();
//         } else {
//             return parseLeaf();
//         }
//     }
//     idi_t findNext( idi_t pos, char c) {
// 
// 
//         while ( pos < inputA.end() && *pos != c) {
//             ++pos;
//         }
// 
//         if ( pos >= inputA.end() ) {
//             throw "reached end of input in find next";
// 
//         }
//         return pos;
//     }
// 
//     bool isFloatChar(char c) {
//         return std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
//     }
// 
//     idi_t findFloat(idi_t pos) {
//         while (isFloatChar(*pos)) {
//             ++pos;
//         }
// 
//         return pos;
//     }
// 
//     double parseBranchLength() {
//         skipWhitespace();
// 
//         // expect + consume ':'
//         if ( *ptr != ':') {
//             printLocation();
//             printf("parse error: parseBranchLength expects ':' at %zd\n", ptr - inputA.begin());
//             throw std::exception();
//         } else {
// 
//             ++ptr;
// 
//             skipWhitespace();
// 
//             idi_t lend = findFloat(ptr);
//             if (lend == ptr) {
//                 printf( "missing float number at %zd\n", ptr - inputA.begin() );
//                 throw std::exception();
//             }
// 
//             double l = atof(substring(ptr, lend).c_str());
//             ptr = lend;
// 
//             return l;
//         }
//     }
// 
//     ln *parseLeaf() {
// //         printf( "parseLeaf\n" );
// 
//         skipWhitespace();
// 
//         // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
//         //int end = findNext(ptr, ':');
//         idi_t end = findEndOfBranch(ptr);
//         std::string ld = substring(ptr, end);
// 
//         ptr = end;
// 
// 
//         //      System.out.printf("leaf: %s\n", ld);
//         ln *n = ln::create();
//         n->data->setTipName(ld);
//         n->data->setTipSerial(nLeafs);
//         //n.data = ld;
//         n->data->isTip = true; // fake
// 
//         nLeafs++;
//         return n;
//     }
// 
//     idi_t findEndOfBranch(idi_t pos) {
//         const char *termchars = ":,)";
// 
// 
//         while ( pos < inputA.end() && !isOneOf( *pos, termchars )) {
//             ++pos;
//         }
//         if ( pos == inputA.end() ) {
//             printLocation();
//             printf( "reached end of input while looking for end of branch label" );
//             throw std::exception();
//         }
// 
//         return pos;
//     }
// 
//     bool isOneOf(char c, const char *cs) {
//         while ( *cs != 0 ) {
//             if ( c == *cs ) {
//                 return true;
//             }
//             ++cs;
//         }
//         return false;
//     }
// 
// 
// 
//     /**
//      * create an edge (=double link) between the two nodes and set branch length
//      *
//      * @param n1
//      * @param n2
//      * @param branchLen
//      */
//     static void twiddle( ln *n1, ln *n2, double branchLen, std::string branchLabel, double support ) {
//         if ( n1->back != 0 ) {
//             printf( "n1.back != null" );
//             throw std::exception();
//         }
// 
//         if ( n2->back != 0 ) {
//             printf( "n2.back != null" );
//             throw std::exception();
//         }
// 
//         n1->back = n2;
//         n2->back = n1;
// 
//         n1->backLen = branchLen;
//         n2->backLen = branchLen;
//         n1->backLabel = branchLabel;
//         n2->backLabel = branchLabel;
//         n1->backSupport = support;
//         n2->backSupport = support;
// 
//     }
// 
//     ln *parseInnerNode() {
// //         printf( "parseInnerNode\n" );
//         skipWhitespace();
// 
// 
//         // expect + consume '('
//         if ( *ptr != '(') {
//             printLocation();
//             printf("parse error: parseInnerNode expects '(' at %zd\n", ptr - inputA.begin());
//             throw std::exception();
//         }
//         ptr++;
// 
//         // parse left node + branch length
//         ln *nl = parseNode();
//         double l1 = parseBranchLength();
//         std::string label1 = parseBranchLabel();
// 
// 
//         skipWhitespace();
// 
// 
//         // expect + consume ','
//         if ( *ptr != ',') {
//             printLocation();
//             printf("parse error: parseInnerNode expects ',' at %zd\n", ptr - inputA.begin());
//             throw std::exception();
//         }
//         ptr++;
// 
// 
//         // parse right node + branch length
//         ln *nr = parseNode();
//         double l2 = parseBranchLength();
//         std::string label2 = parseBranchLabel();
// 
//         skipWhitespace();
// 
// 
//         nInnerNodes++;
//         if ( *ptr == ')') {
//             // 'normal' inner node: two childs
//             ptr++;
// 
//             double support;
//             std::string nodeLabel;
// 
//             if ( *ptr == ';') {
//                 // oh my god. a fucking rooted tree
//                 double sup = std::max( nl->data->getSupport(), nr->data->getSupport());
//                 //System.out.printf( "rooted shit: %s %s %f %f %f %f\n", label1, label2, nl.data.getSupport(), nr.data.getSupport(), l1, l2);
//                 twiddle( nl, nr, l1 + l2, label1, sup );
// 
//                 return nl;
//             }
// 
//             if ( *ptr != ':' && *ptr != ',' && *ptr != ')' ) {
//                 // the stuff between the closing '(' and the ':' of the branch length
//                 // is interpreted as node-label. If the node label corresponds to a float value
//                 // it is interpreted as branch support (or node support as a rooted-trees-only-please biologist would say)
// 
//                 idi_t lend = findEndOfBranch(ptr);
// 
//                 nodeLabel = substring(ptr, lend);
//                 ptr = lend;
// 
//                 bool isDigit = true;
//                 for ( int i = 0; i < nodeLabel.size(); i++ ) {
//                     isDigit &= std::isdigit(nodeLabel.at(i));
// 
//                     if ( i == 0 ) {
//                         isDigit &= (nodeLabel.at(i) != '0');
//                     }
//                 }
// 
//                 if ( isDigit ) {
// 
//                     support = std::atof(nodeLabel.c_str());
//                 } else {
// 
//                     support = -1;
//                 }
// 
// 
// //                int lend = findFloat(ptr);
// //                if (lend == ptr) {
// //                    printLocation();
// //                    throw new RuntimeException("missing float number at " + ptr);
// //                }
// //
// //                support = Double.parseDouble(substring(ptr, lend));
// //                ptr = lend;
//             } else {
//                 support = -1.0;
//             }
// 
//             ln *n = ln::create();
//             n->data->setSupport(support);
//             n->data->setNodeLabel(nodeLabel);
// 
//             twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
//             twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );
// 
// 
//             return n;
//         } else if ( *ptr == ',') {
//             // second comma found: three child nodes == pseudo root
//             ptr++;
// 
//             ln *nx = parseNode();
// 
//             double l3 = parseBranchLength();
//             std::string label3 = parseBranchLabel();
//             //   System.out.printf( "l3: %s\n", nx.data.getTipName() );
//             skipWhitespace();
// 
//             if ( *ptr != ')' ) {
//                 printLocation();
// 
//                 printf("parse error: parseInnerNode (at root) expects ') at %zd\n", ptr - inputA.begin());
//                 throw std::exception();
//             }
//             ptr++;
//             skipWhitespace();
// 
//             ln *n = ln::create();
// 
//             twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
//             twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );
//             twiddle( nx, n, l3, label3, nx->data->getSupport() );
// 
// //                      System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
// //                      System.exit(0);
//             return n;
//         } else {
//             printLocation();
//             printf("parse error: parseInnerNode expects ')'or ',' at %zd\n", ptr - inputA.begin());
//         }
// 
// 
//     }
// 
// 
// public:
//     tree_parser(const char *f) {
//         QUIET = false;
//         readFile(f, inputA);
//         //  this.input = new String(inputA);
//         ptr = inputA.begin();
//     }
//     /**
//          * Call this after object construction to parse the complete tree,
//          * @return pseudo root of the tree
//          */
// 
//     ln *parse() {
//         nLeafs = 0;
//         nInnerNodes = 0;
// 
//         skipWhitespace();
// 
//         if ( ptr >= inputA.end() ) {
//             // seems like we hit the end of the file
//             return 0;
//         }
//         // expect at least one node
//         ln *node = parseNode();
// 
//         // expect terminating ';'
//         if (ptr >= inputA.end()) {
//             throw "parse error. parse: end of input. missing ';'";
//         }
// 
//         // branch length might be present for historical reasons
//         if ( *ptr == ':' ) {
//             parseBranchLength();
//         }
// 
//         if ( *ptr != ';') {
//             printLocation();
//             throw "parse error. parse expects ';'";
//         }
//         // consume terminating ;
//         ++ptr;
//         return node;
//     }
// #if 0
// 
// 
// 
// 
// 
// 
//     private boolean isFloatChar(byte c) {
//         return Character.isDigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
//     }
// 
//     private int findFloat(int pos) {
//         while (isFloatChar(inputA[pos])) {
//             pos++;
//         }
// 
//         return pos;
//     }
// 
// 
// 
//     public int getNTips() {
//         return nLeafs;
//     }
// 
// 
//     public int getNInnerNodes() {
//         return nInnerNodes;
//     }
// 
//     public static void main(String[] args) {
//         TreeParser.QUIET = true;
//         try {
//             LN t = TreeParser.parse(new File( args[0] ));
//             System.out.printf( "good\n" );
//         } catch ( RuntimeException e ) {
//             System.out.printf( "bad\n" );
//         }
//     }
// #endif
// };





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

};
    
struct ldata {
    virtual ~ldata() {}
};


class ln_pool;

    
struct lnode 
    : public intrusive::slist_base_hook<> 

{

    

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

inline std::ostream &operator<<(std::ostream &os, const adata &ad ) {
    ad.print( os );
    return os;
}


class node_data_factory {

public:
    virtual adata *alloc_adata() {
        return new adata();
    }
    
    
    virtual ldata *alloc_ldata() {
        return 0;
    }
};

class ln_pool 
{
     
    typedef intrusive::slist<lnode> lt;
 
    lt m_list;

    //sptr::shared_ptr<node_data_factory> m_ad_fact;
    std::auto_ptr<node_data_factory> m_ad_fact;
public:
    
    // this version takes ownership of fact!
    ln_pool( node_data_factory *fact ) : m_ad_fact(fact) {}
    
    //ln_pool( sptr::shared_ptr<node_data_factory> fact ) : m_ad_fact(fact) {}
    ln_pool() : m_ad_fact(new node_data_factory) {}
    
    
    node_data_factory &get_adata_factory() {
        return *m_ad_fact;
    }
    
    lnode *alloc() {
        lnode *n = new lnode();
        m_list.push_front( *n );
        return &m_list.front();
        
        //return new LN();
    }
    
    void mark( lnode *n ) {
        
        n->mark = true;
        n->next->mark = true;
        n->next->next->mark = true;
        
        if( n->back != 0 ) {
            if( !n->back->mark ) {
                mark( n->back );
            }
        }
        
        if( n->next->back != 0 ) {
            if( !n->next->back->mark ) {
                mark( n->next->back );
            }
        }
        
        if( n->next->next->back != 0 ) {
            if( !n->next->next->back->mark ) {
                mark( n->next->next->back );
            }
        }
        
    }
    
    void sweep() {
        
        size_t size1 = m_list.size();
        
        for( lt::iterator it = m_list.begin(); it != m_list.end(); ) {
            lt::iterator next = it;
            next++;
            if( !it->mark ) {
                //lnode *ln = &(*it);
                // TODO: review: is this safe? shouldn't be able to hit any dead lnode (if they unlink themselves properly and the tree is consistent)
                if( it->back != 0 ) {
					it->back->back = 0;
				}
                
                m_list.erase(it);
                //delete ln;
				it->dealloc();
            }
            
            it = next;
        }
        
        size_t size2 = m_list.size();
        
        printf( "sweep: %zd -> %zd\n", size1, size2 );
    }
    void clear() {
        for( lt::iterator it = m_list.begin(); it != m_list.end(); ++it ) {
            it->mark = false;
        }
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


    void readFile(const char *f, std::vector<char> &data) {

        std::ifstream is(f);
		if( !is.good() ) {
			throw std::runtime_error( "cannot open newick file" );
		}
		
        is.seekg( 0, std::ios_base::end );
		std::ifstream::off_type size = is.tellg();
        is.seekg( 0, std::ios_base::beg );

        data.resize( size );

        is.read( data.data(), size );

    }
    void substring( const idi_t &from, const idi_t &to, std::string &out ) {
//         return new String( Arrays.copyOfRange( inputA, from, to));

        out.clear();

        out.append(from, to );
    }

    std::string substring( const idi_t from, const idi_t to ) {
//         return new String( Arrays.copyOfRange( inputA, from, to));



        return std::string(from, to );
    }

    void printLocation() {
        if ( QUIET ) {
            return;
        }

        idi_t pos1 = std::max(inputA.begin(), ptr - 40);
        idi_t pos2 = std::min(inputA.end(), ptr + 40);



        printf( "%s\n", substring(pos1, pos2).c_str());

        for (idi_t i = pos1; i < ptr; ++i) {
            printf(" ");
        }
        printf("^\n");
    }

    void skipWhitespace() {
        while ( ptr != inputA.end() && std::isspace(*ptr) ) {
            ++ptr;
        }
        if ( ptr == inputA.end() ) {

            printLocation();
            printf( "hit end of input while skipping white-spaces\n" );
            throw std::exception();
        }

    }



    std::string parseBranchLabel() {
        if ( *ptr == '[' ) {
            idi_t lstart = ptr;
            ++ptr;


            idi_t lend = findNext(ptr, ']' );

            ptr = lend+1;

            // labels have the form [blablabla], so the label content starts at lstart + 1

            if ( lend - (lstart+1) <= 0 ) {
                printLocation();

				std::stringstream ss;
				ss << "bad branch label: " << substring(lstart, ptr);
                throw ss.str();
            }

            return substring(lstart + 1, lend);


        } else {
            return std::string();
        }


    }

    lnode *parseNode() {
//         printf( "parseNode\n" );
        skipWhitespace();

        // lookahead: determine node type
        if ( *ptr == '(') {
            return parseInnerNode();
        } else {
            return parseLeaf();
        }
    }
    idi_t findNext( idi_t pos, char c) {


        while ( pos < inputA.end() && *pos != c) {
            ++pos;
        }

        if ( pos >= inputA.end() ) {
            throw "reached end of input in find next";

        }
        return pos;
    }

    bool isFloatChar(char c) {
        return std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
    }

    idi_t findFloat(idi_t pos) {
        while (isFloatChar(*pos)) {
            ++pos;
        }

        return pos;
    }

    double parseBranchLength() {
        skipWhitespace();

        // expect + consume ':'
        if ( *ptr != ':') {
            printLocation();
            printf("parse error: parseBranchLength expects ':' at %zd\n", ptr - inputA.begin());
            throw std::exception();
        } else {

            ++ptr;

            skipWhitespace();

            idi_t lend = findFloat(ptr);
            if (lend == ptr) {
                printf( "missing float number at %zd\n", ptr - inputA.begin() );
                throw std::exception();
            }

            double l = atof(substring(ptr, lend).c_str());
            ptr = lend;

            return l;
        }
    }

    lnode *parseLeaf() {
//         printf( "parseLeaf\n" );

        skipWhitespace();

        // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
        //int end = findNext(ptr, ':');
        idi_t end = findEndOfBranch(ptr);
        std::string ld = substring(ptr, end);

        ptr = end;


        //      System.out.printf("leaf: %s\n", ld);
        lnode *n = lnode::create( m_pool );
        n->m_data->setTipName(ld);
        n->m_data->setTipSerial(nLeafs);
        //n.data = ld;
        n->m_data->isTip = true; // fake

        nLeafs++;
        return n;
    }

    idi_t findEndOfBranch(idi_t pos) {
        const char *termchars = ":,)";


        while ( pos < inputA.end() && !isOneOf( *pos, termchars )) {
            ++pos;
        }
        if ( pos == inputA.end() ) {
            printLocation();
            printf( "reached end of input while looking for end of branch label" );
            throw std::exception();
        }

        return pos;
    }

    bool isOneOf(char c, const char *cs) {
        while ( *cs != 0 ) {
            if ( c == *cs ) {
                return true;
            }
            ++cs;
        }
        return false;
    }



    /**
     * create an edge (=double link) between the two nodes and set branch length
     *
     * @param n1
     * @param n2
     * @param branchLen
     */
    static void twiddle( lnode *n1, lnode *n2, double branchLen, std::string branchLabel, double support ) {
        if ( n1->back != 0 ) {
            printf( "n1.back != null" );
            throw std::exception();
        }

        if ( n2->back != 0 ) {
            printf( "n2.back != null" );
            throw std::exception();
        }

        n1->back = n2;
        n2->back = n1;

        n1->backLen = branchLen;
        n2->backLen = branchLen;
        n1->backLabel = branchLabel;
        n2->backLabel = branchLabel;
        n1->backSupport = support;
        n2->backSupport = support;

    }

    lnode *parseInnerNode() {
//         printf( "parseInnerNode\n" );
        skipWhitespace();


        // expect + consume '('
        if ( *ptr != '(') {
            printLocation();
            printf("parse error: parseInnerNode expects '(' at %zd\n", ptr - inputA.begin());
            throw std::exception();
        }
        ptr++;

        // parse left node + branch length
        lnode *nl = parseNode();
        double l1 = parseBranchLength();
        std::string label1 = parseBranchLabel();


        skipWhitespace();


        // expect + consume ','
        if ( *ptr != ',') {
            printLocation();
            printf("parse error: parseInnerNode expects ',' at %zd\n", ptr - inputA.begin());
            throw std::exception();
        }
        ptr++;


        // parse right node + branch length
        lnode *nr = parseNode();
        double l2 = parseBranchLength();
        std::string label2 = parseBranchLabel();

        skipWhitespace();


        nInnerNodes++;
        if ( *ptr == ')') {
            // 'normal' inner node: two childs
            ptr++;

            double support;
            std::string nodeLabel;

            if ( *ptr == ';') {
                // oh my god. a fucking rooted tree
                double sup = std::max( nl->m_data->getSupport(), nr->m_data->getSupport());
                //System.out.printf( "rooted shit: %s %s %f %f %f %f\n", label1, label2, nl.data.getSupport(), nr.data.getSupport(), l1, l2);
                twiddle( nl, nr, l1 + l2, label1, sup );

                return nl;
            }

            if ( *ptr != ':' && *ptr != ',' && *ptr != ')' ) {
                // the stuff between the closing '(' and the ':' of the branch length
                // is interpreted as node-label. If the node label corresponds to a float value
                // it is interpreted as branch support (or node support as a rooted-trees-only-please biologist would say)

                idi_t lend = findEndOfBranch(ptr);

                nodeLabel = substring(ptr, lend);
                ptr = lend;

                bool isDigit = true;
                for ( size_t i = 0; i < nodeLabel.size(); i++ ) {
                    isDigit = isDigit && std::isdigit(nodeLabel.at(i));

                    if ( i == 0 ) {
                        isDigit = isDigit && (nodeLabel.at(i) != '0');
                    }
                }

                if ( isDigit ) {

                    support = std::atof(nodeLabel.c_str());
                } else {

                    support = -1;
                }


//                int lend = findFloat(ptr);
//                if (lend == ptr) {
//                    printLocation();
//                    throw new RuntimeException("missing float number at " + ptr);
//                }
//
//                support = Double.parseDouble(substring(ptr, lend));
//                ptr = lend;
            } else {
                support = -1.0;
            }

            lnode *n = lnode::create( m_pool );
            n->m_data->setSupport(support);
            n->m_data->setNodeLabel(nodeLabel);

            twiddle( nl, n->next, l1, label1, nl->m_data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->m_data->getSupport() );


            return n;
        } else if ( *ptr == ',') {
            // second comma found: three child nodes == pseudo root
            ptr++;

            lnode *nx = parseNode();

            double l3 = parseBranchLength();
            std::string label3 = parseBranchLabel();
            //   System.out.printf( "l3: %s\n", nx.data.getTipName() );
            skipWhitespace();

            if ( *ptr != ')' ) {
                printLocation();

                printf("parse error: parseInnerNode (at root) expects ') at %zd\n", ptr - inputA.begin());
                throw std::exception();
            }
            ptr++;
            skipWhitespace();

            lnode *n = lnode::create( m_pool );

            twiddle( nl, n->next, l1, label1, nl->m_data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->m_data->getSupport() );
            twiddle( nx, n, l3, label3, nx->m_data->getSupport() );

//                      System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
//                      System.exit(0);
            return n;
        } else {
            printLocation();
            printf("parse error: parseInnerNode expects ')'or ',' at %zd\n", ptr - inputA.begin());
            throw std::runtime_error( "bailing out" );
        }


    }


public:
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

    lnode *parse() {
        nLeafs = 0;
        nInnerNodes = 0;

        skipWhitespace();

        if ( ptr >= inputA.end() ) {
            // seems like we hit the end of the file
            return 0;
        }
        // expect at least one node
        lnode *node = parseNode();

        // expect terminating ';'
        if (ptr >= inputA.end()) {
            throw "parse error. parse: end of input. missing ';'";
        }

        // branch length might be present for historical reasons
        if ( *ptr == ':' ) {
            parseBranchLength();
        }

        if ( *ptr != ';') {
            printLocation();
            throw "parse error. parse expects ';'";
        }
        // consume terminating ;
        ++ptr;
        return node;
    }

};


} // namespace tree_parser_ms


}
