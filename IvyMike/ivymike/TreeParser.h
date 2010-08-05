#include <vector>
#include <fstream>
#include <exception>
#include <cstdlib>
class LN;

class TreeParser {

    //String input;

    // input as char array
    typedef std::vector<char> id_t;
    id_t inputA;

    typedef id_t::iterator idi_t;
    idi_t ptr;


    int nLeafs;
    int nInnerNodes;


    bool QUIET;
//	public TreeParser(String input) {
//	//	this.input = input;
//		this.inputA = input.toCharArray();
//		ptr = 0;
//	}

    TreeParser(const char *f) {

        readFile(f, inputA);
        //  this.input = new String(inputA);
        ptr = inputA.begin();
    }

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
            //throw new RuntimeException("parse error: parseBranchLength expects ':' at " + ptr);
            return -1;
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


        skipWhitespace();

        // a leaf consists just of a data string. use the ':' as terminator for now (this is not correct, as there doesn't have to be a branch length (parsr will crash on tree with only one leaf...));
        //int end = findNext(ptr, ':');
        idi_t end = findEndOfBranch(ptr);
        std::string ld = substring(ptr, end);

        ptr = end;


        //	System.out.printf("leaf: %s\n", ld);
        LN n = LN.create();
        n.data.setTipName(ld);
        n.data.setTipSerial(nLeafs);
        //n.data = ld;
        n.data.isTip = true; // fake

        nLeafs++;
        return n;
    }

    private int findEndOfBranch(int pos) {
        char[] termchars = {
            ':',
            ',',
            ')'
        };

        try {
            while (!isOneOf(inputA[pos], termchars)) {
                pos++;
            }
        } catch (ArrayIndexOutOfBoundsException e) {
            printLocation();
            throw new RuntimeException( "reached end of input while looking for end of branch label" );
        }

        return pos;
    }

    private boolean isOneOf(byte c, char[] chars) {
for ( char tc : chars ) {
            if ( c == tc ) {
                return true;
            }
        }
        return false;
    }


#if 0
    private LN parseInnerNode() {
        skipWhitespace();


        // expect + consume '('
        if (inputA[ptr] != '(') {
            throw new RuntimeException("parse error: parseInnerNode expects '(' at " + ptr);
        }
        ptr++;

        // parse left node + branch length
        LN nl = parseNode();
        double l1 = parseBranchLength();
        String label1 = parseBranchLabel();


        skipWhitespace();


        // expect + consume ','
        if (inputA[ptr] != ',') {
            printLocation();
            throw new RuntimeException("parse error: parseInnerNode expects ',' at " + ptr);
        }
        ptr++;


        // parse right node + branch length
        LN nr = parseNode();
        double l2 = parseBranchLength();
        String label2 = parseBranchLabel();

        skipWhitespace();


        nInnerNodes++;
        if (inputA[ptr] == ')') {
            // 'normal' inner node: two childs
            ptr++;

            final double support;
            String nodeLabel = null;

            if ( inputA[ptr] == ';') {
                // oh my god. a fucking rooted tree
                double sup = Math.max( nl.data.getSupport(), nr.data.getSupport());
                //System.out.printf( "rooted shit: %s %s %f %f %f %f\n", label1, label2, nl.data.getSupport(), nr.data.getSupport(), l1, l2);
                twiddle( nl, nr, l1 + l2, label1, sup );

                return nl;
            }

            if ( inputA[ptr] != ':' && inputA[ptr] != ',' && inputA[ptr] != ')' ) {
                // the stuff between the closing '(' and the ':' of the branch length
                // is interpreted as node-label. If the node label corresponds to a float value
                // it is interpreted as branch support (or node support as a rooted-trees-only-please biologist would say)

                int lend = findEndOfBranch(ptr);

                nodeLabel = substring(ptr, lend);
                ptr = lend;

                boolean isDigit = true;
                for ( int i = 0; i < nodeLabel.length(); i++ ) {
                    isDigit &= Character.isDigit(nodeLabel.charAt(i));

                    if ( i == 0 ) {
                        isDigit &= (nodeLabel.charAt(i) != '0');
                    }
                }

                if ( isDigit ) {
                    support = Double.parseDouble(nodeLabel);
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

            LN n = LN.create();
            n.data.setSupport(support);
            n.data.setNodeLabel(nodeLabel);

            twiddle( nl, n.next, l1, label1, nl.data.getSupport() );
            twiddle( nr, n.next.next, l2, label2, nr.data.getSupport() );


            return n;
        } else if ( inputA[ptr] == ',') {
            // second comma found: three child nodes == pseudo root
            ptr++;

            LN nx = parseNode();

            double l3 = parseBranchLength();
            String label3 = parseBranchLabel();
            //   System.out.printf( "l3: %s\n", nx.data.getTipName() );
            skipWhitespace();

            if ( inputA[ptr] != ')' ) {
                printLocation();
                throw new RuntimeException("parse error: parseInnerNode (at root) expects ') at " + ptr);
            }
            ptr++;
            skipWhitespace();

            LN n = LN.create();

            twiddle( nl, n.next, l1, label1, nl.data.getSupport() );
            twiddle( nr, n.next.next, l2, label2, nr.data.getSupport() );
            twiddle( nx, n, l3, label3, nx.data.getSupport() );

//			System.out.printf( "root: %f %f %f\n", nl.data.getSupport(), nr.data.getSupport(), nx.data.getSupport() );
//			System.exit(0);
            return n;
        } else {
            printLocation();
            throw new RuntimeException("parse error: parseInnerNode expects ')'or ',' at " + ptr);
        }


    }

    /**
     * create an edge (=double link) between the two nodes and set branch length
     *
     * @param n1
     * @param n2
     * @param branchLen
     */
    private static void twiddle( LN n1, LN n2, double branchLen, String branchLabel, double support ) {
        if ( n1.back != null ) {
            throw new RuntimeException( "n1.back != null" );
        }

        if ( n2.back != null ) {
            throw new RuntimeException( "n2.back != null" );
        }

        n1.back = n2;
        n2.back = n1;

        n1.backLen = branchLen;
        n2.backLen = branchLen;
        n1.backLabel = branchLabel;
        n2.backLabel = branchLabel;
        n1.backSupport = support;
        n2.backSupport = support;

    }





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
