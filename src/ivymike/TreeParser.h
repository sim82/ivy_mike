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
#include <exception>
#include <cstdlib>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// #include <boost/weak_ptr.hpp>
#include <boost/intrusive/slist.hpp>
#include <list>
namespace ivy_mike {



class TreeParser {
public:
    struct LN {

        struct AData {

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



            AData() : isTip( false ), tipSerial(-1), support(-1) {}


        };

        static LN *create() {

            LN *n = new LN();
            n->next = new LN();
            n->next->next = new LN();
            n->next->next->next = n;
            n->data = n->next->data = n->next->next->data = new AData();

            return n;

        }

        //     static void freeRing( LN *n ) {
        //       delete n->next->next;
        //       delete n->next;
        //       delete n->data;
        //       delete n;
        //     }
        //
        //     static void freeRec( LN *n ) {
        //       if( n != 0 ) {
        //           freeRec( n->next->back );
        //           freeRec( n->next->next->back );
        //           freeRing( n );
        //       }
        //     }
        //     static void free( LN *n ) {
        //       freeRec( n->back );
        //       freeRec( n->next->back );
        //       freeRec( n->next->next->back );
        //       freeRing( n );
        //     }

        LN() : data(0), next(0), back(0), backLen(-1), backSupport(-1) {}
        ~LN() {

            // experiment: recursive destructor
            // basic method: the 'deleter' (=a destructor invoking 'delete' on any other object ptr) is responsible
            // to prevent the deleted object from double-deleting, by clearing its references.

            // WARNING: the destructor of LN will free the complete reachable tree!

            if ( back != 0 ) {
                // prevent 'back' from deleting 'this', by clearing the back reference
                back->back = 0;
                delete back;
            }
            if ( next != 0 ) {
                // prevent 'next' from deleting the shared AData object
                next->data = 0;
                if ( next->next != 0 ) {
                    // prevent 'next->next' from deleting 'this' (==next->next->next)
                    next->next->data = 0;
                    next->next->next = 0;
                }
                delete next;
            }
            //if( data != 0 ) {
            delete data;
            //}
        }

        AData *data;
        LN *next;
        LN *back;

        double backLen;
        double backSupport;
        std::string backLabel;


    };
    //String input;
private:
    // input as char array
    typedef std::vector<char> id_t;
    id_t inputA;

    typedef id_t::iterator idi_t;
    idi_t ptr;


    int nLeafs;
    int nInnerNodes;


    bool QUIET;
//      public TreeParser(String input) {
//      //      this.input = input;
//              this.inputA = input.toCharArray();
//              ptr = 0;
//      }


    void readFile(const char *f, std::vector<char> &data) {

        std::ifstream is(f);
        is.seekg( 0, std::ios_base::end );
        off_t size = is.tellg();
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
        while ( ptr != inputA.end() && std::iswspace(*ptr) ) {
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
                throw (std::string("bad branch label: ") + substring(lstart, ptr) );
            }

            return substring(lstart + 1, lend);


        } else {
            return std::string();
        }


    }

    LN *parseNode() {
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

    LN *parseLeaf() {
//         printf( "parseLeaf\n" );

        skipWhitespace();

        // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
        //int end = findNext(ptr, ':');
        idi_t end = findEndOfBranch(ptr);
        std::string ld = substring(ptr, end);

        ptr = end;


        //      System.out.printf("leaf: %s\n", ld);
        LN *n = LN::create();
        n->data->setTipName(ld);
        n->data->setTipSerial(nLeafs);
        //n.data = ld;
        n->data->isTip = true; // fake

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
    static void twiddle( LN *n1, LN *n2, double branchLen, std::string branchLabel, double support ) {
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

    LN *parseInnerNode() {
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
        LN *nl = parseNode();
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
        LN *nr = parseNode();
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
                double sup = std::max( nl->data->getSupport(), nr->data->getSupport());
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
                for ( int i = 0; i < nodeLabel.size(); i++ ) {
                    isDigit &= std::isdigit(nodeLabel.at(i));

                    if ( i == 0 ) {
                        isDigit &= (nodeLabel.at(i) != '0');
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

            LN *n = LN::create();
            n->data->setSupport(support);
            n->data->setNodeLabel(nodeLabel);

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );


            return n;
        } else if ( *ptr == ',') {
            // second comma found: three child nodes == pseudo root
            ptr++;

            LN *nx = parseNode();

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

            LN *n = LN::create();

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );
            twiddle( nx, n, l3, label3, nx->data->getSupport() );

//                      System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
//                      System.exit(0);
            return n;
        } else {
            printLocation();
            printf("parse error: parseInnerNode expects ')'or ',' at %zd\n", ptr - inputA.begin());
        }


    }


public:
    TreeParser(const char *f) {
        QUIET = false;
        readFile(f, inputA);
        //  this.input = new String(inputA);
        ptr = inputA.begin();
    }
    /**
         * Call this after object construction to parse the complete tree,
         * @return pseudo root of the tree
         */

    LN *parse() {
        nLeafs = 0;
        nInnerNodes = 0;

        skipWhitespace();

        if ( ptr >= inputA.end() ) {
            // seems like we hit the end of the file
            return 0;
        }
        // expect at least one node
        LN *node = parseNode();

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
#if 0






    private boolean isFloatChar(byte c) {
        return Character.isDigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
    }

    private int findFloat(int pos) {
        while (isFloatChar(inputA[pos])) {
            pos++;
        }

        return pos;
    }



    public int getNTips() {
        return nLeafs;
    }


    public int getNInnerNodes() {
        return nInnerNodes;
    }

    public static void main(String[] args) {
        TreeParser.QUIET = true;
        try {
            LN t = TreeParser.parse(new File( args[0] ));
            System.out.printf( "good\n" );
        } catch ( RuntimeException e ) {
            System.out.printf( "bad\n" );
        }
    }
#endif
};



using namespace boost;

class TreeParserMS {
public:
    
    class ln_pool;
#define MS_INTRUSIVE        
    
    struct LN 
#ifdef MS_INTRUSIVE
        : public intrusive::slist_base_hook<> 
#endif
        {

        struct AData {

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



            AData() : isTip( false ), tipSerial(-1), support(-1) {}


        };

        static LN *create( ln_pool &pool );/* {
            
            LN *n = new LN();
            n->next = new LN();
            n->next->next = new LN();
            n->next->next->next = n;
            n->data = n->next->data = n->next->next->data = new AData();

            return n;

        }*/

        //     static void freeRing( LN *n ) {
        //       delete n->next->next;
        //       delete n->next;
        //       delete n->data;
        //       delete n;
        //     }
        //
        //     static void freeRec( LN *n ) {
        //       if( n != 0 ) {
        //           freeRec( n->next->back );
        //           freeRec( n->next->next->back );
        //           freeRing( n );
        //       }
        //     }
        //     static void free( LN *n ) {
        //       freeRec( n->back );
        //       freeRec( n->next->back );
        //       freeRec( n->next->next->back );
        //       freeRing( n );
        //     }

        LN() : next(0), back(0), backLen(-1), backSupport(-1), mark(false) {}
        ~LN() {
        }

        boost::shared_ptr<AData> data;
        //LN *next;
        LN *next;
        LN *back;

        double backLen;
        double backSupport;
        std::string backLabel;

        bool mark;
    };
    

    class ln_pool {
#ifdef MS_INTRUSIVE        
        typedef intrusive::slist<LN> lt;
#else        
        typedef std::list<LN> lt;
#endif        
        lt m_list;
        
    public:
        LN *alloc() {
            
            //m_list.push_back( *(new LN()));
            
#ifdef MS_INTRUSIVE            
            LN *ln = new LN();
            m_list.push_front( *ln );
            return &m_list.front();
#else            
            m_list.push_back(LN());
            return &m_list.back();
            //assert( &m_list.back() != &ln );
#endif            
            
            //return new LN();
        }
        
        void mark( LN *n ) {
         
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
                 
#ifdef MS_INTRUSIVE                    
                    LN *ln = &(*it);
                    m_list.erase(it);
                    delete ln;
#else
                    m_list.erase(it);
#endif
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
        is.seekg( 0, std::ios_base::end );
        off_t size = is.tellg();
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
        while ( ptr != inputA.end() && std::iswspace(*ptr) ) {
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
                throw (std::string("bad branch label: ") + substring(lstart, ptr) );
            }

            return substring(lstart + 1, lend);


        } else {
            return std::string();
        }


    }

    LN *parseNode() {
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

    LN *parseLeaf() {
//         printf( "parseLeaf\n" );

        skipWhitespace();

        // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
        //int end = findNext(ptr, ':');
        idi_t end = findEndOfBranch(ptr);
        std::string ld = substring(ptr, end);

        ptr = end;


        //      System.out.printf("leaf: %s\n", ld);
        LN *n = LN::create( m_pool );
        n->data->setTipName(ld);
        n->data->setTipSerial(nLeafs);
        //n.data = ld;
        n->data->isTip = true; // fake

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
    static void twiddle( LN *n1, LN *n2, double branchLen, std::string branchLabel, double support ) {
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

    LN *parseInnerNode() {
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
        LN *nl = parseNode();
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
        LN *nr = parseNode();
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
                double sup = std::max( nl->data->getSupport(), nr->data->getSupport());
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
                for ( int i = 0; i < nodeLabel.size(); i++ ) {
                    isDigit &= std::isdigit(nodeLabel.at(i));

                    if ( i == 0 ) {
                        isDigit &= (nodeLabel.at(i) != '0');
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

            LN *n = LN::create( m_pool );
            n->data->setSupport(support);
            n->data->setNodeLabel(nodeLabel);

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );


            return n;
        } else if ( *ptr == ',') {
            // second comma found: three child nodes == pseudo root
            ptr++;

            LN *nx = parseNode();

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

            LN *n = LN::create( m_pool );

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );
            twiddle( nx, n, l3, label3, nx->data->getSupport() );

//                      System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
//                      System.exit(0);
            return n;
        } else {
            printLocation();
            printf("parse error: parseInnerNode expects ')'or ',' at %zd\n", ptr - inputA.begin());
        }


    }


public:
    TreeParserMS(const char *f, ln_pool &pool ) : m_pool(pool) {
        QUIET = false;
        readFile(f, inputA);
        //  this.input = new String(inputA);
        ptr = inputA.begin();
    }
    /**
         * Call this after object construction to parse the complete tree,
         * @return pseudo root of the tree
         */

    LN *parse() {
        nLeafs = 0;
        nInnerNodes = 0;

        skipWhitespace();

        if ( ptr >= inputA.end() ) {
            // seems like we hit the end of the file
            return 0;
        }
        // expect at least one node
        LN *node = parseNode();

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
#if 0






    private boolean isFloatChar(byte c) {
        return Character.isDigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
    }

    private int findFloat(int pos) {
        while (isFloatChar(inputA[pos])) {
            pos++;
        }

        return pos;
    }



    public int getNTips() {
        return nLeafs;
    }


    public int getNInnerNodes() {
        return nInnerNodes;
    }

    public static void main(String[] args) {
        TreeParser.QUIET = true;
        try {
            LN t = TreeParser.parse(new File( args[0] ));
            System.out.printf( "good\n" );
        } catch ( RuntimeException e ) {
            System.out.printf( "bad\n" );
        }
    }
#endif
};









#if 0

class TreeParserSP {
public:
    struct LN {
        typedef boost::shared_ptr<LN> LN_sp;
        typedef boost::weak_ptr<LN> LN_wp;

        struct AData {

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



            AData() : isTip( false ), tipSerial(-1), support(-1) {}


        };

        static LN_sp create() {

            LN *n = new LN();
            n->next = LN_wp(new LN());
            n->next->next = LN_wp( new LN() );
            n->next->next->next = LN_wp(n);
            n->data = n->next->data = n->next->next->data = LN_sp(new AData());

            return LN_sp(n);

        }

        //     static void freeRing( LN *n ) {
        //       delete n->next->next;
        //       delete n->next;
        //       delete n->data;
        //       delete n;
        //     }
        //
        //     static void freeRec( LN *n ) {
        //       if( n != 0 ) {
        //           freeRec( n->next->back );
        //           freeRec( n->next->next->back );
        //           freeRing( n );
        //       }
        //     }
        //     static void free( LN *n ) {
        //       freeRec( n->back );
        //       freeRec( n->next->back );
        //       freeRec( n->next->next->back );
        //       freeRing( n );
        //     }

        LN() : data(0), next(0), back(0), backLen(-1), backSupport(-1) {}
        ~LN() {

            // experiment: recursive destructor
            // basic method: the 'deleter' (=a destructor invoking 'delete' on any other object ptr) is responsible
            // to prevent the deleted object from double-deleting, by clearing its references.

            // WARNING: the destructor of LN will free the complete reachable tree!

            if ( back != 0 ) {
                // prevent 'back' from deleting 'this', by clearing the back reference
                back->back = 0;
                delete back;
            }
            if ( next != 0 ) {
                // prevent 'next' from deleting the shared AData object
                next->data = 0;
                if ( next->next != 0 ) {
                    // prevent 'next->next' from deleting 'this' (==next->next->next)
                    next->next->data = 0;
                    next->next->next = 0;
                }
                delete next;
            }
            //if( data != 0 ) {
            delete data;
            //}
        }



        boost::shared_ptr<AData> data;
        LN_wp next;
        LN_sp back;

        double backLen;
        double backSupport;
        std::string backLabel;


    };
    //String input;
private:
    // input as char array
    typedef std::vector<char> id_t;
    id_t inputA;

    typedef id_t::iterator idi_t;
    idi_t ptr;


    int nLeafs;
    int nInnerNodes;


    bool QUIET;
//      public TreeParser(String input) {
//      //      this.input = input;
//              this.inputA = input.toCharArray();
//              ptr = 0;
//      }


    void readFile(const char *f, std::vector<char> &data) {

        std::ifstream is(f);
        is.seekg( 0, std::ios_base::end );
        off_t size = is.tellg();
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
        while ( ptr != inputA.end() && std::iswspace(*ptr) ) {
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
                throw (std::string("bad branch label: ") + substring(lstart, ptr) );
            }

            return substring(lstart + 1, lend);


        } else {
            return std::string();
        }


    }

    LN *parseNode() {
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

    LN *parseLeaf() {
//         printf( "parseLeaf\n" );

        skipWhitespace();

        // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
        //int end = findNext(ptr, ':');
        idi_t end = findEndOfBranch(ptr);
        std::string ld = substring(ptr, end);

        ptr = end;


        //      System.out.printf("leaf: %s\n", ld);
        LN *n = LN::create();
        n->data->setTipName(ld);
        n->data->setTipSerial(nLeafs);
        //n.data = ld;
        n->data->isTip = true; // fake

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
    static void twiddle( LN *n1, LN *n2, double branchLen, std::string branchLabel, double support ) {
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

    LN *parseInnerNode() {
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
        LN *nl = parseNode();
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
        LN *nr = parseNode();
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
                double sup = std::max( nl->data->getSupport(), nr->data->getSupport());
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
                for ( int i = 0; i < nodeLabel.size(); i++ ) {
                    isDigit &= std::isdigit(nodeLabel.at(i));

                    if ( i == 0 ) {
                        isDigit &= (nodeLabel.at(i) != '0');
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

            LN *n = LN::create();
            n->data->setSupport(support);
            n->data->setNodeLabel(nodeLabel);

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );


            return n;
        } else if ( *ptr == ',') {
            // second comma found: three child nodes == pseudo root
            ptr++;

            LN *nx = parseNode();

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

            LN *n = LN::create();

            twiddle( nl, n->next, l1, label1, nl->data->getSupport() );
            twiddle( nr, n->next->next, l2, label2, nr->data->getSupport() );
            twiddle( nx, n, l3, label3, nx->data->getSupport() );

//                      System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
//                      System.exit(0);
            return n;
        } else {
            printLocation();
            printf("parse error: parseInnerNode expects ')'or ',' at %zd\n", ptr - inputA.begin());
        }


    }


public:
    TreeParser(const char *f) {
        QUIET = false;
        readFile(f, inputA);
        //  this.input = new String(inputA);
        ptr = inputA.begin();
    }
    /**
         * Call this after object construction to parse the complete tree,
         * @return pseudo root of the tree
         */

    LN *parse() {
        nLeafs = 0;
        nInnerNodes = 0;

        skipWhitespace();

        if ( ptr >= inputA.end() ) {
            // seems like we hit the end of the file
            return 0;
        }
        // expect at least one node
        LN *node = parseNode();

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
#if 0






    private boolean isFloatChar(byte c) {
        return Character.isDigit(c) || c == '.' || c == 'e' || c == 'E' || c == '-';
    }

    private int findFloat(int pos) {
        while (isFloatChar(inputA[pos])) {
            pos++;
        }

        return pos;
    }



    public int getNTips() {
        return nLeafs;
    }


    public int getNInnerNodes() {
        return nInnerNodes;
    }

    public static void main(String[] args) {
        TreeParser.QUIET = true;
        try {
            LN t = TreeParser.parse(new File( args[0] ));
            System.out.printf( "good\n" );
        } catch ( RuntimeException e ) {
            System.out.printf( "bad\n" );
        }
    }
#endif
};
#endif

}
