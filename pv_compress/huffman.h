// ==============================================================
//
//  Copyright (c) 1999-2001 by Alex Vinokur.  This work and all works
//  derived from it may be copied and modified without any
//  restrictions other than that a copy of this copyright notice
//  must be included in any copy of this work or any derived work.
//
// ==============================================================

///////////////////////////////////////

#ifndef huf_service_H
#define huf_service_H

///////////////////////////////////////

static char id_huf_service_H[] = "@(#)## n-ary Huffman Template Algorithm ## Author : Alex Vinokur ## "__FILE__;

// ##############################################################
// =============================
//  n-ary Huffman Template Algorithm
//  The algorithm (program) contains the following files :
//  - huf_service.H
//  - huf_class.H
//  - huf_methods.H
//  - huf_main.C
// =============================
//
//  FILE : huf_service.H
//
//  AUTHOR : Alex Vinokur
//
//  DESCRIPTION :
//         Definition and implementation
//         of the following auxiliary template functions :
//         ----------------------------------------------
//         - string             to_str (...)
//         - void               add_to_vector (...)
//         - void               fill_vector (...)
//         - unsigned int       get_width (...)
//         - string             gstr_vect_ptrs (...)
//         - string             gstr_vector (...)       // two functions
//         - string             gstr_path (...)
//         - string             gstr_map (...)
//         - ostream&           operator<< (...)        // two operators
//         ----------------------------------------------
//
//  DATE           VERSION
//  ----           -------
//  Aug-26-1999    NHTA 1.0
//  Jul-05-2001    NHTA 1.1
//  Sep-11-2001    NHTA 1.2
//
// ##############################################################


#include <strstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
// #include <algo.h>
#include <functional>
#include <iostream>

#include <iterator>
#include <fstream>
#include <typeinfo>
#include <iomanip>
#include <assert.h>
#include <limits.h>
using namespace std;



//#######################################################
//##### PART : DEFINES & CONSTANTS ######################
//#######################################################

#define MIN_VALUE(x,y)  ((x) < (y) ? (x) : (y))
#define MAX_VALUE(x,y)  ((x) > (y) ? (x) : (y))
//#define ASSERT(x)       if (!(x)) {cerr << endl << endl << "\t=== BUG IN PROGRAM ===" << endl;}; assert (x)
#define ASSERT(x) assert(x)


#define FATAL_TITLE     "FATAL ERROR : "
#define FATAL_SHIFT     "            : "
#define FATAL_MSG(x)    cerr << endl \
                             << FATAL_TITLE \
                             << x \
                             << endl \
                             << FATAL_SHIFT \
                             << "File - " \
                             << __FILE__ \
                             << ", Line#" \
                             << __LINE__ \
                             << endl; \
                             exit (1)


#define ERROR_TITLE     "ERROR : "
#define ERROR_SHIFT     "      : "
#define ERROR_MSG(x)    cerr << endl \
                             << ERROR_TITLE \
                             << x \
                             << endl \
                             << ERROR_SHIFT \
                             << "File - " \
                             << __FILE__ \
                             << ", Line#" \
                             << __LINE__ \
                             << endl;


//#######################################################
//##### PART : typedefs #################################
//#######################################################
typedef unsigned int    CODE;



//#######################################################
//##### PART : FUNCTIONS ################################
//#######################################################


//#####################################################3
template <typename T>
string to_str (T value_i, int width_i = -1, char fill_i = ' ', const string& prefix_i = string ())
{
    string          ret_stringValue;
    strstream       tmp_strstream;

    //=================================
    if (width_i > 0)
    {
        tmp_strstream.width (width_i);
        tmp_strstream.fill (fill_i);
    }
    tmp_strstream << prefix_i;
    tmp_strstream << value_i;

    //=================================
    tmp_strstream << ends;
    ret_stringValue = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);
    //=================================
    return ret_stringValue;
} // string to_str (T value_i)



//#####################################################3
template <typename T>
void add_to_vector (vector<T>& vector_i, const basic_string<T>& string_i)
{
    copy (string_i.begin (), string_i.end (), back_inserter (vector_i));
} //void add_to_vector (T value_o)



//#####################################################3
template <typename T>
void fill_vector (vector<T>& vector_i, const basic_string<T>& string_i)
{
    vector_i = vector<T> ();
    add_to_vector (vector_i, string_i);
} //void fill_vector (T value_o)




//#####################################################3
template <typename T>
unsigned int get_width (T value_i)
{
    unsigned int    ret_intValue;
    strstream       tmp_strstream;

    tmp_strstream << value_i;

    //=================================
    tmp_strstream << ends;
    ret_intValue = string (tmp_strstream.str()).size ();
    tmp_strstream.rdbuf()->freeze (0);
    //=================================
    return ret_intValue;
} // unsigned int get_width (T value_i)



//#####################################################
template <typename T1>
string   gstr_vect_ptrs (const vector<T1*>& vector_i, const string& delimiter_i = "")
{
    strstream               tmp_strstream;
    string                  tmp_string;
    unsigned int            cur_index;

    cout << delimiter_i << endl;
    for (cur_index = 0; cur_index < vector_i.size (); cur_index++)
    {
        cout << "vector element ["
             << cur_index << "] : "
             << (*(vector_i [cur_index]))
             << delimiter_i
             << endl;
    }


    tmp_strstream << ends;
    tmp_string = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);

    //===================
    return tmp_string;

} // gstr_vect_ptrs (const vector<CODE>& vector_i)



//#####################################################
template <typename T1>
string   gstr_vector (const vector<T1>& vector_i, unsigned int start_pos_i = 0, unsigned int end_pos_i = UINT_MAX, const string& delimiter_i = "")
{

    if (vector_i.empty ())
    {
        return "Empty Vector";
    }
    //=====================================
    if (end_pos_i == UINT_MAX)
    {
        end_pos_i = vector_i.size () - 1;
    }
    ASSERT (end_pos_i < vector_i.size ());
    ASSERT (start_pos_i <= end_pos_i);
    //=====================================

    strstream               tmp_strstream;
    string                  tmp_string;
    ostream_iterator<T1>    out (tmp_strstream, delimiter_i.c_str ());

    //copy (vector_i.begin () + start_pos_i, vector_i.begin () + end_pos_i + 1, out);
    for( typename vector<T1>::const_iterator i = vector_i.begin(); i != vector_i.end(); ++i ) {
        tmp_strstream << *i;
        tmp_strstream << " ";
    }

    tmp_strstream << ends;
    tmp_string = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);

    //===================
    return tmp_string;

} // gstr_vector (const vector<CODE>& vector_i)



//#####################################################
template <typename T1>
ostream& operator<< (ostream& o, const vector<T1>& vector_i)
{
    return o << gstr_vector (vector_i);
}


//#####################################################
template <typename T1>
string   gstr_vector (const vector<T1>& vector_i, const string& delimiter_i, unsigned int start_pos_i = 0, unsigned int end_pos_i = UINT_MAX)
{
    return gstr_vector (vector_i, start_pos_i, end_pos_i, delimiter_i);
} // string   gstr_vector - 2


//#####################################################
template <unsigned int ARY>
string   gstr_path (const vector<CODE>& path_i)
{
    const string    delimiter_CNS = (ARY > 16) ? "." : "";
    strstream       tmp_strstream;
    string          tmp_string;

    if (path_i.empty ())
    {
        tmp_strstream << "This is Huffman Tree Root";
    }
    else
    {
        for (unsigned int cur_index = 0; cur_index < path_i.size (); cur_index++)
        {
            if (cur_index > 0)
            {
                tmp_strstream << delimiter_CNS;
            }
            tmp_strstream << path_i [cur_index];
        }
    }
    //=====================================
    tmp_strstream << ends;
    tmp_string = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);

    //===================
    return tmp_string;

} // gstr_path (const vector<CODE>& path_i)




//#######################################
template <typename T1, typename T2>
string gstr_map (const map<T1, T2, less<T1> >& map_i, const string &shift_i = string ())
{
    strstream       tmp_strstream;
    string          tmp_string;

    tmp_strstream << endl;
    tmp_strstream << endl;
    tmp_strstream << shift_i;
    tmp_strstream << "\tmap size = "
    << map_i.size ()
    << endl;

    typename map<T1, T2, less<T1> >::const_iterator  cur_const_iter;
    for (cur_const_iter = map_i.begin(); !(cur_const_iter == map_i.end()); cur_const_iter++)
    {
        tmp_strstream << shift_i;
        tmp_strstream << "map element ["
        << (*cur_const_iter).first
        << "] = "
        << "<"
        << (*cur_const_iter).second
        << ">";
        tmp_strstream << endl;
    }
    tmp_strstream << ends;
    tmp_string = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);

    return tmp_string;

} // string gstr_map



//#######################################
template <typename T1, typename T2>
ostream& operator<<(ostream &str_o, const map<T1, T2, less<T1> >& map_i)
{
    return str_o << gstr_map (map_i);

} // ostream& operator<<(ostream &str_o, const map<T1>& map_i)


#endif  // huf_service_H


//#######################################################
//################ END OF FILE ##########################
//#######################################################
// ==============================================================
//
//  Copyright (c) 1999-2001 by Alex Vinokur.  This work and all works
//  derived from it may be copied and modified without any
//  restrictions other than that a copy of this copyright notice
//  must be included in any copy of this work or any derived work.
//
// ==============================================================

///////////////////////////////////////

#ifndef huf_class_H
#define huf_class_H

///////////////////////////////////////

static char id_huf_class_H[] = "@(#)## n-ary Huffman Template Algorithm ## Author : Alex Vinokur ## "__FILE__;

// ##############################################################
// =============================
//  n-ary Huffman Template Algorithm
//  The algorithm (program) contains the following files :
//  - huf_service.H
//  - huf_class.H
//  - huf_methods.H
//  - huf_main.C
// =============================
//
//  FILE : huf_class.H
//
//  AUTHOR : Alex Vinokur
//
//  DESCRIPTION :
//         Definition of the following template classes :
//         ----------------------------------------------
//         - Cell                         <SYMBOL, WEIGHT>
//         - Node                         <SYMBOL, WEIGHT>
//         - InternalNode                 <SYMBOL, WEIGHT>
//         - TerminalNode                 <SYMBOL, WEIGHT>
//         - BasicHuffmanTree             <SYMBOL, WEIGHT, ARY>
//         - LoadedHuffmanTree            <SYMBOL, WEIGHT, ARY>
//         - DriedHuffmanTree             <WEIGHT, ARY>
//         ----------------------------------------------
//
//         Definition and implementation of the following template classes :
//         ----------------------------------------------
//         - lessNodesCompare             <SYMBOL, WEIGHT>
//         - lessNodesCorrectingCompare01 <SYMBOL, WEIGHT>
//         - lessNodesCorrectingCompare02 <SYMBOL, WEIGHT>
//         - lessCellsCompare             <SYMBOL, WEIGHT>
//         - lessVectorsAlterCompare      <T>
//         ----------------------------------------------
//
//  DATE           VERSION
//  ----           -------
//  Aug-26-1999    NHTA 1.0
//  Jul-05-2001    NHTA 1.1
//  Sep-11-2001    NHTA 1.2
//
// ##############################################################

//======================
// #include "huf_service.H"
//======================

//#######################################################
//##### PART : DEFINES & CONSTANTS ######################
//#######################################################

//#define       SHOW_HUFFMAN_PROCESS_STATUS


//#######################################################
//##### PART : DECLARATIONS #############################
//#######################################################

template <typename SYMBOL, typename WEIGHT>
class Cell;

template <typename SYMBOL, typename WEIGHT>
class Node;

template <typename SYMBOL, typename WEIGHT>
class InternalNode ;

template <typename SYMBOL, typename WEIGHT>
class TerminalNode ;

template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
class BasicHuffmanTree;

template <typename SYMBOL, typename WEIGHT, unsigned int ARY = 2>
class LoadedHuffmanTree;

template <typename WEIGHT, unsigned int ARY = 2>
class DriedHuffmanTree;

template <typename SYMBOL, typename WEIGHT>
class lessNodesCompare;

template <typename SYMBOL, typename WEIGHT>
class lessNodesCorrectingCompare01;

template <typename SYMBOL, typename WEIGHT>
class lessNodesCorrectingCompare02;

template <typename SYMBOL, typename WEIGHT>
class lessCellsCompare;

template <typename T1>
class lessVectorsAlterCompare;


//-----------------------
template <typename SYMBOL, typename WEIGHT>
istream& operator>>(istream &stream_o, Cell<SYMBOL, WEIGHT>& instance_i)
{
    stream_o >> instance_i.data_symbol_ >> instance_i.data_weight_;
    return stream_o;
}


//-----------------------
template <typename SYMBOL, typename WEIGHT>
ostream& operator<<(ostream &str_o, const Node<SYMBOL, WEIGHT>& instance_i)
{
    const string shift_CNS = "\t---> ";
    return str_o << endl
           << shift_CNS
           << "weight_ = "
           << instance_i.weight_
           << gstr_map (instance_i.mapSymbols_, shift_CNS);
}


//#######################################################
//##### PART : template class Cell ######################
//############ Definition ###############################
//#######################################################

//----------- template class Cell -----------
template <typename SYMBOL, typename WEIGHT>
class Cell
{
    template <typename S1, typename W1, unsigned int A1> friend class BasicHuffmanTree;

    friend class TerminalNode<SYMBOL, WEIGHT>;
    friend class lessCellsCompare<SYMBOL, WEIGHT>;
    friend istream& operator>> <SYMBOL, WEIGHT> (istream &str_o, Cell<SYMBOL, WEIGHT>& instance_i);

private :
    SYMBOL  data_symbol_;
    WEIGHT  data_weight_;
    unsigned int    symbol_original_index_;
    vector<CODE>    symbol_path_;
protected :

public :
    Cell () {}
    Cell (
        const SYMBOL&   data_symbol_i,
        const WEIGHT&   data_weight_i,
        unsigned int    symbol_original_index_i = UINT_MAX
    );
    virtual ~Cell () {}

};




//#######################################################
//##### PART : template class Node ######################
//############ Definition ###############################
//#######################################################

//----------- template class Node -----------
template <typename SYMBOL, typename WEIGHT>
class Node
{
    template <typename S1, typename W1, unsigned int A1> friend class BasicHuffmanTree;

    friend class InternalNode<SYMBOL, WEIGHT>;
    friend class lessNodesCompare<SYMBOL, WEIGHT>;
    friend class lessNodesCorrectingCompare01<SYMBOL, WEIGHT>;
    friend class lessNodesCorrectingCompare02<SYMBOL, WEIGHT>;
    friend ostream& operator<< <SYMBOL, WEIGHT> (ostream &str_o, const Node<SYMBOL, WEIGHT>& instance_i);



private :
protected :
public:
        typedef map<SYMBOL, WEIGHT, less<SYMBOL> > Node_MAP_SYMBOLS;
    Node_MAP_SYMBOLS        mapSymbols_;
    WEIGHT                  weight_;
    bool                    is_TerminalNode_;
    int                     absorbtion_stage_;
    int                     creation_stage_;

public :
    Node () {
        weight_ = WEIGHT ();
        absorbtion_stage_ = -2;
        creation_stage_ = -1;
    }
    virtual ~Node () {}
};




//#######################################################
//##### PART : template class InternalNode ##############
//############ Definition ###############################
//#######################################################

//----------- template class InternalNode -----------
template <typename SYMBOL, typename WEIGHT>
class InternalNode : public Node<SYMBOL, WEIGHT>
{
    template <typename S1, typename W1, unsigned int A1> friend class BasicHuffmanTree;
private :
    vector<Node<SYMBOL, WEIGHT>*>   arc_;
protected :
    void addNode (Node<SYMBOL, WEIGHT> const *  const ptr2_i);
public :
    InternalNode ()  {
        this->is_TerminalNode_ = false;
    }
    ~InternalNode () {}
};




//#######################################################
//##### PART : template class TerminalNode ##############
//############ Definition ###############################
//#######################################################

//----------- template class TerminalNode -----------
template <typename SYMBOL, typename WEIGHT>
class TerminalNode : public Node<SYMBOL, WEIGHT>
{
    template <typename S1, typename W1, unsigned int A1> friend class BasicHuffmanTree;
private :

protected :
public :
    TerminalNode () {
        Node<SYMBOL, WEIGHT>::is_TerminalNode_ = true;
    }
    TerminalNode (const Cell<SYMBOL, WEIGHT>& cell_i);
    ~TerminalNode () {}
};



//#######################################################
//##### PART : template class less... ###################
//#######################################################

//#######################################################
//----------- template class lessNodesCompare -----------
template <typename SYMBOL, typename WEIGHT>
class lessNodesCompare
{
public:
    bool operator()(
        const Node<SYMBOL, WEIGHT>* const left_i,
        const Node<SYMBOL, WEIGHT>* const right_i
    )
    {
        return (left_i->weight_ < right_i->weight_);
    }
};


//#######################################################
//------- template class lessNodesCorrectingCompare01 -----
template <typename SYMBOL, typename WEIGHT>
class lessNodesCorrectingCompare01
{
public:
    bool operator()(
        const Node<SYMBOL, WEIGHT>* const left_i,
        const Node<SYMBOL, WEIGHT>* const right_i
    )
    {
        return ((left_i->weight_ == right_i->weight_) ? (!(left_i->is_TerminalNode_)) : (left_i->weight_ < right_i->weight_));
    }
};



//#######################################################
//------- template class lessNodesCorrectingCompare02 -----
template <typename SYMBOL, typename WEIGHT>
class lessNodesCorrectingCompare02
{
public:
    bool operator()(
        const Node<SYMBOL, WEIGHT>* const left_i,
        const Node<SYMBOL, WEIGHT>* const right_i
    )
    {
        return ((left_i->is_TerminalNode_ == right_i->is_TerminalNode_) ? (left_i->weight_ < right_i->weight_) : (!(left_i->is_TerminalNode_)));
    }
};


//#######################################################
//----------- template class lessCellsCompare -----------
template <typename SYMBOL, typename WEIGHT>
class lessCellsCompare
{
public:
    bool operator()(
        const Cell<SYMBOL, WEIGHT>& left_i,
        const Cell<SYMBOL, WEIGHT>& right_i
    )
    {

        return (left_i.data_weight_ < right_i.data_weight_);
    }
};




//#######################################################
//----------- template class lessVectorsAlterCompare -----------
template <typename T1>
class lessVectorsAlterCompare
{
public:
    bool operator()(
        const vector<T1>&       left_i,
        const vector<T1>&       right_i
    )
    {
        if (left_i.size () < right_i.size ())
        {
            return true;
        }

        if (left_i.size () > right_i.size ())
        {
            return false;
        }

        return (left_i < right_i);
    }
};




//#######################################################
//##### PART : template class BasicHuffmanTree ##########
//############ Definition ###############################
//#######################################################


//#######################################################
//----------- template class BasicHuffmanTree -----------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
class BasicHuffmanTree
{
    typedef map<SYMBOL, Cell<SYMBOL, WEIGHT>, less<SYMBOL> > Tree_MAP_SYMBOLS;
    typedef map<vector<CODE>, SYMBOL, less<vector<CODE> > > Tree_MAP_HUFFMAN_DECODING;

private :
    Tree_MAP_SYMBOLS                mapAlphabet_;
    Tree_MAP_HUFFMAN_DECODING       mapHuffmanCodes_;
    InternalNode<SYMBOL, WEIGHT>    rootNode_;
    vector<vector<CODE> >           vectorHuffmanCodes_;


    void    createAllTerminalNodes (
        const vector<Cell<SYMBOL, WEIGHT> >&    data_vector_i,
        vector<Node<SYMBOL, WEIGHT>*>&          vectorHuffmanProcess_i
    );
    void    createInternalNode (
        vector<Node<SYMBOL, WEIGHT>*>&          vectorHuffmanProcess_i,
        int                                     cur_stage_i
    );

    void    createHuffmanTree (
        vector<Node<SYMBOL, WEIGHT>*>&          vectorHuffmanProcess_i
    );

    void    doBeautyTreatment (
        vector<Node<SYMBOL, WEIGHT>*>&          vectorHuffmanProcess_i
    );


    void    storeHuffmanCodes ();


    bool            encodeOneSymbol (
        const SYMBOL&   symbol_i,
        vector<CODE>&   path_o)
    const;


    bool            decodeOneSymbol (
        const vector<CODE>&     encoded_msg_i,
        unsigned int&           cur_start_position_io,
        unsigned int&           cur_symbol_number_io,
        vector<CODE>&           cur_symbol_code_o,
        SYMBOL&                 cur_symbol_value_o
    ) const;

    bool            decodeOneSymbol (
        const vector<CODE>&     encoded_symbol_code_i,
        SYMBOL&                 decoded_symbol_value_o
    ) const;

    bool            testAllCodes (bool show_i = false) const;

    void            showHuffmanProcessStatus (
        vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i,
        int                             cur_stage_i = 0,
        const string&                   msg_i = string ()
    ) const;

    static void     print_show_title_S (
        const string&   spaces_offset_i,
        unsigned int    setw_symbol_i,
        const string&   symbol_title_i,
        unsigned int    setw_weight_i,
        const string&   weight_title_i,
        const string&   code_title_i
    );

    static void     print_show_line_S (
        const string&   spaces_offset_i,
        unsigned int    setw_symbol_i,
        const SYMBOL&   symbol_i,
        unsigned int    setw_weight_i,
        const WEIGHT&   weight_i,
        const vector<CODE>&     path_i
    );


    bool            knowSymbolWeight (
        const SYMBOL&   symbol_i,
        WEIGHT&         weight_o
    ) const;

    bool            knowCodeSymbol (
        const vector<CODE>&     path_i,
        SYMBOL&                 symbol_o
    ) const;

    WEIGHT          getWeightsSum () const;

    WEIGHT          getAverageWeight () const
    {
        return (getWeightsSum ()/getAlphabetSize ());
    }

    unsigned int    getCodeAry () const {
        return ARY;
    }

    unsigned int    getLongestSymbolSize () const;

    unsigned int    getAlphabetSize () const
    {
        return vectorHuffmanCodes_.size ();
    }

    unsigned int    getShortestCodeSize () const
    {
        return vectorHuffmanCodes_[0].size ();
    }

    unsigned int    getLongestCodeSize () const
    {
        return vectorHuffmanCodes_[vectorHuffmanCodes_.size () - 1].size ();
    }

    unsigned int    getCodeSizesSum () const;

    float           getAverageCodeSize () const
    {
        return (static_cast<float>(getCodeSizesSum ())/static_cast<float>(getAlphabetSize ()));
    }

    WEIGHT          getAverageWeightedCodeSize () const
    {
        return (getWeightedCodeSizesSum ())/(getAlphabetSize ());
    }

    WEIGHT          getWeightedCodeSizesSum () const;

protected :
    void    doBasicHuffmanTree (
        const vector<Cell<SYMBOL, WEIGHT> >& data_vector_i
    );

    BasicHuffmanTree () {}

    BasicHuffmanTree (
        const vector<Cell<SYMBOL, WEIGHT> >& data_vector_i
    );

    BasicHuffmanTree (const string& data_file_name_i);

    virtual ~BasicHuffmanTree () {}

public :
    bool            encodeMsg (
        const vector<SYMBOL>&   source_msg_i,
        vector<CODE>&           encoded_msg_o)
    const;

    bool            encodeMsg (
        const basic_string<SYMBOL>&     source_msg_i,
        string&                         encoded_msg_o)
    const;

    bool            decodeMsg (
        const vector<CODE>&     encoded_msg_i,
        vector<SYMBOL>&         decoded_msg_o
    ) const;

    bool            decodeMsg (
        const string&                   encoded_msg_i,
        basic_string<SYMBOL>&           decoded_msg_o
    ) const;


    void            showAll (const string& msg_i = string ()) const;
};





//#######################################################
//##### PART : template class LoadedHuffmanTree #########
//############ Definition ###############################
//#######################################################

//----------- template class LoadedHuffmanTree -----------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
class LoadedHuffmanTree : public BasicHuffmanTree<SYMBOL, WEIGHT, ARY>
{

public :
    LoadedHuffmanTree () : BasicHuffmanTree<SYMBOL, WEIGHT, ARY> () {}

    LoadedHuffmanTree (
        const vector<Cell<SYMBOL, WEIGHT> >& data_vector_i
    )
            :
            BasicHuffmanTree<SYMBOL, WEIGHT, ARY> (data_vector_i) {}

    LoadedHuffmanTree (const string& data_file_name_i)
            : BasicHuffmanTree<SYMBOL, WEIGHT, ARY> (data_file_name_i) {}

    ~LoadedHuffmanTree () {}
};





//#######################################################
//##### PART : template class DriedHuffmanTree ##########
//############ Definition ###############################
//#######################################################

//----------- template class DriedHuffmanTree -----------
template <typename WEIGHT, unsigned int ARY>
class DriedHuffmanTree : public BasicHuffmanTree<string, WEIGHT, ARY>
{
private :

    void    doDriedHuffmanTree (
        const vector<WEIGHT>& weight_vector_i
    );
protected :
public :

    DriedHuffmanTree (
        const vector<WEIGHT>& weight_vector_i
    );
    DriedHuffmanTree (const string& weights_file_name_i);

    ~DriedHuffmanTree () {}
};


#endif  // huf_class_H

//#######################################################
//################ END OF FILE ##########################
//#######################################################
// ==============================================================
//
//  Copyright (c) 1999-2001 by Alex Vinokur.  This work and all works
//  derived from it may be copied and modified without any
//  restrictions other than that a copy of this copyright notice
//  must be included in any copy of this work or any derived work.
//
// ==============================================================

///////////////////////////////////////

#ifndef huf_methods_H
#define huf_methods_H

///////////////////////////////////////

static char id_huf_methods_H[] = "@(#)## n-ary Huffman Template Algorithm ## Author : Alex Vinokur ## "__FILE__;

// ##############################################################
// =============================
//  n-ary Huffman Template Algorithm
//  The algorithm (program) contains the following files :
//  - huf_service.H
//  - huf_class.H
//  - huf_methods.H
//  - huf_main.C
// =============================
//
//  FILE : huf_methods.H
//
//  AUTHOR : Alex Vinokur
//
//  DESCRIPTION :
//         Implementation of methods of the following template classes :
//         ----------------------------------------------
//         - Cell                         <SYMBOL, WEIGHT>
//         - Node                         <SYMBOL, WEIGHT>
//         - InternalNode                 <SYMBOL, WEIGHT>
//         - TerminalNode                 <SYMBOL, WEIGHT>
//         - BasicHuffmanTree             <SYMBOL, WEIGHT, ARY>
//         - DriedHuffmanTree             <WEIGHT, ARY>
//         ----------------------------------------------
//         Note. The following class has no its own methods :
//         - LoadedHuffmanTree            <SYMBOL, WEIGHT, ARY>
//         ----------------------------------------------
//
//  DATE           VERSION
//  ----           -------
//  Aug-26-1999    NHTA 1.0
//  Jul-05-2001    NHTA 1.1
//  Sep-11-2001    NHTA 1.2
//
// ##############################################################



//=====================
// #include "huf_class.H"
//=====================




//#######################################################
//##### PART : template class Cell ######################
//############ Methods ##################################
//#######################################################

//-----------------------
// Constructor
template <typename SYMBOL, typename WEIGHT>
Cell<SYMBOL, WEIGHT>::Cell (
    const SYMBOL&   data_symbol_i,
    const WEIGHT&   data_weight_i,
    unsigned int    symbol_original_index_i
)
{
    data_symbol_            = data_symbol_i;
    data_weight_            = data_weight_i;
    symbol_original_index_  = symbol_original_index_i;
} // Cell<SYMBOL, WEIGHT>::Cell (







//#######################################################
//##### PART : template class Node ######################
//############ Methods ##################################
//#######################################################







//#######################################################
//##### PART : template class InternalNode ##############
//############ Methods ##################################
//#######################################################

//-----------------------
template <typename SYMBOL, typename WEIGHT>
void InternalNode<SYMBOL, WEIGHT>::addNode(Node<SYMBOL, WEIGHT> const * const ptr2_i)
{
    SYMBOL  cur_symbol;
    WEIGHT  cur_weight;

    typename Node<SYMBOL, WEIGHT>::Node_MAP_SYMBOLS::const_iterator                const_iterSymbols;


    ASSERT (!(ptr2_i == NULL));

    //=============== ptr2 =====================
    this->weight_ += ptr2_i->weight_;

    for (const_iterSymbols = ptr2_i->mapSymbols_.begin();
            !(const_iterSymbols == ptr2_i->mapSymbols_.end());
            const_iterSymbols++
        )
    {
        cur_symbol = (*const_iterSymbols).first;
        cur_weight = (*const_iterSymbols).second;
        //==========================================
        ASSERT (!this->mapSymbols_.count (cur_symbol));
        //==========================================
        if (!((this->mapSymbols_.insert (typename Node<SYMBOL, WEIGHT>::Node_MAP_SYMBOLS::value_type (cur_symbol, cur_weight))).second))
        {
            ASSERT (0);
        }
        //==========================================
    }


} // void InternalNode<SYMBOL, WEIGHT>::addNode(





//#######################################################
//##### PART : template class TerminalNode ##############
//############ Methods ##################################
//#######################################################

//-----------------------
// Constructor
template <typename SYMBOL, typename WEIGHT>
TerminalNode<SYMBOL, WEIGHT>::TerminalNode (const Cell<SYMBOL, WEIGHT>& cell_i)
{
    //==========================================
    this->is_TerminalNode_ = true;
    //==========================================
    ASSERT (!this->mapSymbols_.count (cell_i.data_symbol_));

    this->weight_ = cell_i.data_weight_;
    if (!((this->mapSymbols_.insert (typename Node<SYMBOL,WEIGHT>::Node_MAP_SYMBOLS::value_type (cell_i.data_symbol_, this->weight_))).second))
    {
        ASSERT (0);
    }

} // TerminalNode<SYMBOL, WEIGHT>::TerminalNode





//#######################################################
//##### PART : template class BasicHuffmanTree ##########
//############ Methods ##################################
//#######################################################

//-----------------------
// Constructor-1
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::BasicHuffmanTree (
    const vector<Cell<SYMBOL, WEIGHT> >& data_vector_i
)
{
    doBasicHuffmanTree (data_vector_i);

} // BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::BasicHuffmanTree ()


//-----------------------
// Constructor-2
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::BasicHuffmanTree (const string& data_file_name_i)
{
    vector<Cell<SYMBOL, WEIGHT> >   data_vector;

    ifstream fin (data_file_name_i.c_str ());

    if (!fin)
    {
        FATAL_MSG ("Cannot open file <"
                   << data_file_name_i
                   << "> for reading"
                   << endl
                   << FATAL_SHIFT
                   << "The file must contain data to be Huffman-coded"
                  );
    }

    copy(istream_iterator<Cell<SYMBOL, WEIGHT> >(fin),
         istream_iterator<Cell<SYMBOL, WEIGHT> >(),
         back_inserter(data_vector));

    doBasicHuffmanTree (data_vector);
} // BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::BasicHuffmanTree ()



//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::doBasicHuffmanTree (
    const vector<Cell<SYMBOL, WEIGHT> >& data_vector_i
)
{
    if (!(ARY >= 2))
    {
        FATAL_MSG ("Illegal ARY - "
                   << ARY
                   << " (Must be >= 2)"
                  );
    }

    switch (data_vector_i.size ())
    {
    case 0 :
        FATAL_MSG ("Empty alphabet"
                   << endl
                   << FATAL_SHIFT
                   << "Alphabet size must be >= 2"
                  );
        break;

    case 1 :
        FATAL_MSG ("Alphabet size = "
                   << data_vector_i.size ()
                   << endl
                   << FATAL_SHIFT
                   << "Alphabet size must be >= 2"
                  );
        break;

    default :
        break;
    } // switch


    ASSERT (ARY >= 2);
    if (!((data_vector_i.size () - 1)%(ARY - 1) == 0))
    {
        FATAL_MSG ("Illegal alphabet size (N = "
                   << data_vector_i.size ()
                   << ") for "
                   << ARY
                   << "-ary tree."
                   << endl
                   << FATAL_SHIFT
                   << "N must satisfy the following condition : (N - 1) mod ("
                   << (ARY - 1)
                   << ") = 0"
                   << endl
                   << FATAL_SHIFT
                   << "In reality "
                   << (data_vector_i.size () - 1)
                   << " mod ("
                   << (ARY - 1)
                   << ")"
                   << " = "
                   << ((data_vector_i.size () - 1)%(ARY - 1))
                  );
    }

    ASSERT (data_vector_i.size () > 1);

    ASSERT ((data_vector_i.size () - 1)%(ARY - 1) == 0);

    //=================
    vector<Node<SYMBOL, WEIGHT>*>           vectorHuffmanProcess;

    createAllTerminalNodes (data_vector_i, vectorHuffmanProcess);
    createHuffmanTree (vectorHuffmanProcess);
    storeHuffmanCodes ();

    //=================
    InternalNode<SYMBOL, WEIGHT>*           ptrRootNode;
    ASSERT (vectorHuffmanProcess.size () == 1);
    ptrRootNode = dynamic_cast<InternalNode<SYMBOL, WEIGHT>*> (vectorHuffmanProcess [0]);
    ASSERT (!(ptrRootNode == NULL));
    //=================
    rootNode_ = *ptrRootNode;
    //=================
    delete (vectorHuffmanProcess [0]);
    //=================
    ASSERT (testAllCodes ());
    //=================
    ASSERT (mapAlphabet_.size () == mapHuffmanCodes_.size ());
    ASSERT (mapAlphabet_.size () == vectorHuffmanCodes_.size ());
    ASSERT (mapHuffmanCodes_.size () == vectorHuffmanCodes_.size ());
    //=================
} // BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::doBasicHuffmanTree ()




//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createInternalNode (
    vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i,
    int                             cur_stage_i
)
{
    unsigned int    cur_arc;
    typename map<SYMBOL, WEIGHT, less<SYMBOL> >::iterator    iterSymbols;
    InternalNode<SYMBOL, WEIGHT>*   newPtrInternalNode = new InternalNode<SYMBOL, WEIGHT> ();

    ASSERT (!(newPtrInternalNode == NULL));

    Node<SYMBOL, WEIGHT>*   front_element;
    ASSERT (newPtrInternalNode->arc_.empty ());
    for (cur_arc = 0; cur_arc < ARY; cur_arc++)
    {
        front_element = vectorHuffmanProcess_i.front ();
        front_element->absorbtion_stage_ = cur_stage_i;
        newPtrInternalNode->addNode (front_element);

        for (iterSymbols = front_element->mapSymbols_.begin();
                !(iterSymbols == front_element->mapSymbols_.end());
                iterSymbols++
            )
        {
            ASSERT (mapAlphabet_.count ((*iterSymbols).first));
            vector<CODE>& alias_symbol_path = mapAlphabet_ [(*iterSymbols).first].symbol_path_;
            ASSERT (newPtrInternalNode->arc_.size () == cur_arc);
            alias_symbol_path.insert (alias_symbol_path.begin (), newPtrInternalNode->arc_.size ());
        }
        //=====================================
        vectorHuffmanProcess_i.erase (vectorHuffmanProcess_i.begin ());
        newPtrInternalNode->creation_stage_ = cur_stage_i;
        newPtrInternalNode->arc_.push_back (front_element);
        //==========================================
    }
    ASSERT (newPtrInternalNode->arc_.size () == ARY);

    //==========================================
    vectorHuffmanProcess_i.push_back (newPtrInternalNode);
    stable_sort (vectorHuffmanProcess_i.begin (), vectorHuffmanProcess_i.end (), lessNodesCompare<SYMBOL, WEIGHT> ());
    //==========================================
    ASSERT ((((ARY - 1) * cur_stage_i) + vectorHuffmanProcess_i.size ()) == mapAlphabet_.size ());

    //---------------------

} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createInternalNode


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createHuffmanTree (
    vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i
)
{
#ifdef SHOW_HUFFMAN_PROCESS_STATUS
    showHuffmanProcessStatus (vectorHuffmanProcess_i);
#endif

    int cur_stage = 0;
    while (vectorHuffmanProcess_i.size () > 1)
    {
        cur_stage++;
        createInternalNode (vectorHuffmanProcess_i, cur_stage);
        doBeautyTreatment (vectorHuffmanProcess_i);
#ifdef SHOW_HUFFMAN_PROCESS_STATUS
        showHuffmanProcessStatus (vectorHuffmanProcess_i, cur_stage);
#endif
    }
    ASSERT (vectorHuffmanProcess_i.size () == 1);
    ASSERT (!(vectorHuffmanProcess_i [0]->is_TerminalNode_));

} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createHuffmanTree ()



//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::doBeautyTreatment (
    vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i
)
{
    ASSERT (!(vectorHuffmanProcess_i.empty ()));
    if (vectorHuffmanProcess_i.size () == 1)
    {
        return;
    }
    stable_sort (vectorHuffmanProcess_i.begin (), vectorHuffmanProcess_i.end (), lessNodesCorrectingCompare01<SYMBOL, WEIGHT> ());
    stable_sort (vectorHuffmanProcess_i.begin (), vectorHuffmanProcess_i.begin () + ARY, lessNodesCorrectingCompare02<SYMBOL, WEIGHT> ());

} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::doBeautyTreatment ()




//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createAllTerminalNodes (
    const vector<Cell<SYMBOL, WEIGHT> >&    data_vector_i,
    vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i
)
{
    unsigned int    cur_index;
    SYMBOL          cur_symbol;
    WEIGHT          cur_weight;

    for (cur_index = 0; cur_index < data_vector_i.size (); cur_index++)
    {
        //==========================================
        cur_symbol = data_vector_i [cur_index].data_symbol_;
        cur_weight = data_vector_i [cur_index].data_weight_;
        //==========================================
        if (mapAlphabet_.count(cur_symbol))
        {
            FATAL_MSG ("Symbol <"
                       << cur_symbol
                       << "> occurs more than one time"
                       << endl
                       << FATAL_SHIFT
                       << "See symbol ["
                       << cur_index
                       << "]"
                       << " and "
                       << "symbol ["
                       << (*(mapAlphabet_.find (cur_symbol))).second.symbol_original_index_
                       << "]"
                       << endl
                       << FATAL_SHIFT
                       << "Note! First symbol is symbol [0]"
                      );
        }
        //==========================================
        if (!((mapAlphabet_.insert (typename Tree_MAP_SYMBOLS::value_type (cur_symbol, Cell<SYMBOL, WEIGHT> (cur_symbol, cur_weight, cur_index)))).second))
        {
            ASSERT (0);
        }
    } // for (unsigned int i = 0; i < data_vector_i.size (); i++)

    //=====================================
    TerminalNode<SYMBOL, WEIGHT>*   newPtrTerminalNode;
    typename Tree_MAP_SYMBOLS::iterator      iterAlphabet;
    for (iterAlphabet = mapAlphabet_.begin();
            !(iterAlphabet == mapAlphabet_.end());
            iterAlphabet++
        )
    {
        newPtrTerminalNode = new TerminalNode<SYMBOL, WEIGHT> ((*iterAlphabet).second);
        ASSERT (!(newPtrTerminalNode == NULL));
        newPtrTerminalNode->creation_stage_ = 0;

        vectorHuffmanProcess_i.push_back (newPtrTerminalNode);
    }

    stable_sort (vectorHuffmanProcess_i.begin (), vectorHuffmanProcess_i.end (), lessNodesCompare<SYMBOL, WEIGHT> ());
} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::createAllTerminalNodes




//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::storeHuffmanCodes ()
{

    typename Tree_MAP_SYMBOLS::iterator                      iterAlphabet;

    //=====================================
    for (iterAlphabet = mapAlphabet_.begin();
            !(iterAlphabet == mapAlphabet_.end());
            iterAlphabet++
        )
    {
        ASSERT (!mapHuffmanCodes_.count ((*iterAlphabet).second.symbol_path_));
        if (!((mapHuffmanCodes_.insert (typename Tree_MAP_HUFFMAN_DECODING::value_type ((*iterAlphabet).second.symbol_path_, (*iterAlphabet).first))).second))
        {
            ASSERT (0);
        }

        vectorHuffmanCodes_.push_back ((*iterAlphabet).second.symbol_path_);

    }

    stable_sort (vectorHuffmanCodes_.begin (), vectorHuffmanCodes_.end (), lessVectorsAlterCompare<CODE> ());


} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::storeHuffmanCodes ()


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
unsigned int BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getCodeSizesSum () const
{
    unsigned int    ret_intValue = 0;
    for (unsigned int cur_index = 0; cur_index < vectorHuffmanCodes_.size (); cur_index++)
    {
        ret_intValue += vectorHuffmanCodes_[cur_index].size ();
    }
    return ret_intValue;
} // unsigned int BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getCodeSizesSum () const


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getWeightsSum () const
{
    typename Tree_MAP_SYMBOLS::const_iterator        const_iterAlphabet;
    WEIGHT ret_WEIGHT_Value;
    //=====================================
    ret_WEIGHT_Value = WEIGHT ();
    for (const_iterAlphabet = mapAlphabet_.begin();
            !(const_iterAlphabet == mapAlphabet_.end());
            const_iterAlphabet++
        )
    {
        ret_WEIGHT_Value += (*const_iterAlphabet).second.data_weight_;
    }

    return ret_WEIGHT_Value;

} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getWeightsSum () const


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
unsigned int BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getLongestSymbolSize () const
{
    typename Tree_MAP_SYMBOLS::const_iterator        const_iterAlphabet;

    //=====================================
    unsigned int ret_intValue = 0;

    for (const_iterAlphabet = mapAlphabet_.begin();
            !(const_iterAlphabet == mapAlphabet_.end());
            const_iterAlphabet++
        )
    {
        strstream       tmp_strstream;
        tmp_strstream << (*const_iterAlphabet).first;
        tmp_strstream << ends;
        ret_intValue = MAX_VALUE (ret_intValue, string (tmp_strstream.str()).size ());
        tmp_strstream.rdbuf()->freeze (0);
    }

    //=====================================
    return ret_intValue;

} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getLongestSymbolSize () const




//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getWeightedCodeSizesSum () const
{
    typename Tree_MAP_SYMBOLS::const_iterator        const_iterAlphabet;
    WEIGHT ret_WEIGHT_Value;
    //=====================================
    ret_WEIGHT_Value = WEIGHT ();
    for (const_iterAlphabet = mapAlphabet_.begin();
            !(const_iterAlphabet == mapAlphabet_.end());
            const_iterAlphabet++
        )
    {
        ret_WEIGHT_Value += ((*const_iterAlphabet).second.data_weight_ * (*const_iterAlphabet).second.symbol_path_.size ());
    }
    //=====================================
    return ret_WEIGHT_Value;
} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::getWeightedCodeSizesSum () const


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeOneSymbol (const SYMBOL& symbol_i, vector<CODE>& path_o) const
{
    if (!mapAlphabet_.count (symbol_i))
    {
        ERROR_MSG ("Symbol <"
                   << symbol_i
                   << "> is out of Alphabet"
                  );
        showAll ();
        return false;

    }
    path_o = (*(mapAlphabet_.find (symbol_i))).second.symbol_path_;

    //=====================================
    return true;

} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeOneSymbol () const


//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::knowSymbolWeight (const SYMBOL& symbol_i, WEIGHT& weight_o) const
{
    if (!mapAlphabet_.count (symbol_i))
    {
        ERROR_MSG ("Symbol <"
                   << symbol_i
                   << "> is out of Alphabet"
                  );
        showAll ();
        return false;

    }
    weight_o = (*(mapAlphabet_.find (symbol_i))).second.data_weight_;

    //=====================================
    return true;

} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::knowSymbolWeight () const




//-----------------------
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::knowCodeSymbol (const vector<CODE>& path_i, SYMBOL& symbol_o) const
{
    if (!mapHuffmanCodes_.count (path_i))
    {
        ERROR_MSG ("Code <"
                   << gstr_path<ARY> (path_i)
                   << "> is out of Huffman Codes for this Alphabet"
                  );
        showAll ();
        return false;

    }

    symbol_o = (*(mapHuffmanCodes_.find (path_i))).second;

    //=====================================
    return true;

} // WEIGHT BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::knowCodeSymbol () const




//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::showAll (const string& msg_i) const
{

    cout << endl;
    cout << "########### showAll (BEGIN) ###########" << endl;
    if (!msg_i.empty ())
    {
        cout << "\t" << "===== " << msg_i << " =====" << endl;
        cout << endl;
    }

    cout << endl;
    cout << "\t" << "-> This is ";
    switch (ARY)
    {
    case 1 :
        ASSERT (ARY > 1);
        break;

    case 2 :
        cout << "Binary";
        break;

    case 3 :
        cout << "Ternary";
        break;

    default :
        cout << ARY << "-ary";
        break;
    }
    cout << " Huffman Coding <-" << endl;

    cout << "\t" << "Alphabet size \t\t= " << getAlphabetSize () << endl;
    cout << "\t" << "Shortest code size \t= " << getShortestCodeSize () << endl;
    cout << "\t" << "Longest code size \t= " << getLongestCodeSize () << endl;
    cout << endl;
    cout << "\t" << "Weights sum \t\t= " << getWeightsSum () << endl;
    cout << "\t" << "Average weight \t\t= " << getAverageWeight () << endl;
    cout << endl;
    cout << "\t" << "Code-sizes sum \t\t= " << getCodeSizesSum () << endl;
    cout << "\t" << "Average code-size \t= " << getAverageCodeSize () << endl;
    cout << endl;
    cout << "\t" << "Weighted code-sizes sum = " << getWeightedCodeSizesSum () << endl;
    cout << "\t" << "Ave. weighted code-size = " << getAverageWeightedCodeSize () << endl;
    cout << endl;
    cout << endl;



    //==================================
    typename Tree_MAP_SYMBOLS::const_iterator                const_iterAlphabet;
    typename Tree_MAP_HUFFMAN_DECODING::const_iterator       const_iterHuffmanCodes;
    vector<CODE>                            cur_path;
    WEIGHT                                  cur_weight;
    SYMBOL                                  cur_symbol;
    const string                            spaces_offset_CNS = "   ";
    const string                            weight_title_CNS = "Weight";
    const string                            symbol_title_CNS = "Symbol";
    const string                            code_title_CNS = "Code";
    const int                               setw_weight_CNS = 12;
    const int                               setw_symbol_CNS = MAX_VALUE (symbol_title_CNS.size (), getLongestSymbolSize ());

    cout << endl;
    cout << endl;
    cout << "\t======================= " << endl;
    cout << "\tSymbols and their codes" << endl;
    cout << "\t -> Sorted by Symbol" << endl;
    cout << "\t======================= " << endl;
    print_show_title_S (
        spaces_offset_CNS,
        setw_symbol_CNS,
        symbol_title_CNS,
        setw_weight_CNS,
        weight_title_CNS,
        code_title_CNS
    );

    //=====================================
    for (const_iterAlphabet = mapAlphabet_.begin();
            !(const_iterAlphabet == mapAlphabet_.end());
            const_iterAlphabet++
        )
    {
        if (!encodeOneSymbol ((*const_iterAlphabet).first, cur_path))
        {
            ASSERT (0);
        }
        if (!knowSymbolWeight ((*const_iterAlphabet).first, cur_weight))
        {
            ASSERT (0);
        }

        print_show_line_S (
            spaces_offset_CNS,
            setw_symbol_CNS,
            (*const_iterAlphabet).first,
            setw_weight_CNS,
            cur_weight,
            cur_path
        );

    } // for (const_iterAlphabet = mapAlphabet_.begin() ...
    //=====================================

    cout << endl;
    cout << endl;
    cout << "\t=========================" << endl;
    cout << "\tCodes and their symbols" << endl;
    cout << "\t -> Lexico-Sorted by Code" << endl;
    cout << "\t=========================" << endl;
    print_show_title_S (
        spaces_offset_CNS,
        setw_symbol_CNS,
        symbol_title_CNS,
        setw_weight_CNS,
        weight_title_CNS,
        code_title_CNS
    );

    //=====================================
    for (const_iterHuffmanCodes = mapHuffmanCodes_.begin();
            !(const_iterHuffmanCodes == mapHuffmanCodes_.end());
            const_iterHuffmanCodes++
        )
    {
        if (!knowCodeSymbol ((*const_iterHuffmanCodes).first, cur_symbol))
        {
            ASSERT (0);
        }
        if (!knowSymbolWeight (cur_symbol, cur_weight))
        {
            ASSERT (0);
        }


        print_show_line_S (
            spaces_offset_CNS,
            setw_symbol_CNS,
            cur_symbol,
            setw_weight_CNS,
            cur_weight,
            (*const_iterHuffmanCodes).first
        );

    } // for (const_iterHuffmanCodes = mapHuffmanCodes_.begin() ...
    //=====================================


    cout << endl;
    cout << endl;
    cout << "\t=======================" << endl;
    cout << "\tCodes and their symbols" << endl;
    cout << "\t -> Sorted by Code Size" << endl;
    cout << "\t=======================" << endl;
    print_show_title_S (
        spaces_offset_CNS,
        setw_symbol_CNS,
        symbol_title_CNS,
        setw_weight_CNS,
        weight_title_CNS,
        code_title_CNS
    );

    //=====================================
    unsigned int    the_index;
    for (the_index = 0; the_index < vectorHuffmanCodes_.size (); the_index++)
    {
        if (!knowCodeSymbol (vectorHuffmanCodes_ [the_index], cur_symbol))
        {
            ASSERT (0);
        }
        if (!knowSymbolWeight (cur_symbol, cur_weight))
        {
            ASSERT (0);
        }

        print_show_line_S (
            spaces_offset_CNS,
            setw_symbol_CNS,
            cur_symbol,
            setw_weight_CNS,
            cur_weight,
            vectorHuffmanCodes_ [the_index]
        );
    } // for (const_iterHuffmanCodes = mapHuffmanCodes_.begin() ...
    //=====================================



    cout << endl;
    cout << endl;
    cout << "\t=======================" << endl;
    cout << "\tCodes and their symbols" << endl;
    cout << "\t -> Sorted by Weight" << endl;
    cout << "\t=======================" << endl;
    print_show_title_S (
        spaces_offset_CNS,
        setw_symbol_CNS,
        symbol_title_CNS,
        setw_weight_CNS,
        weight_title_CNS,
        code_title_CNS
    );

    vector <Cell<SYMBOL, WEIGHT> >  tmp_vectorCell;
    for (const_iterAlphabet = mapAlphabet_.begin();
            !(const_iterAlphabet == mapAlphabet_.end());
            const_iterAlphabet++
        )
    {
        tmp_vectorCell.push_back ((*const_iterAlphabet).second);
    }
    stable_sort (tmp_vectorCell.begin (), tmp_vectorCell.end (), lessCellsCompare<SYMBOL, WEIGHT> ());


    for (the_index = 0; the_index < tmp_vectorCell.size (); the_index++)
    {
        cur_symbol = tmp_vectorCell [the_index].data_symbol_;
        if (!knowSymbolWeight (cur_symbol, cur_weight))
        {
            ASSERT (0);
        }
        if (!encodeOneSymbol (cur_symbol, cur_path))
        {
            ASSERT (0);
        }

        print_show_line_S (
            spaces_offset_CNS,
            setw_symbol_CNS,
            cur_symbol,
            setw_weight_CNS,
            cur_weight,
            cur_path
        );
    } // for (const_iterHuffmanCodes = mapHuffmanCodes_.begin() ...
    //=====================================


    cout << endl;
    cout << "########### showAll (END) #############" << endl;
    cout << endl;

} // void showAll


//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::showHuffmanProcessStatus (
    vector<Node<SYMBOL, WEIGHT>*>&  vectorHuffmanProcess_i,
    int                             cur_stage_i,
    const string&                   msg_i
) const
{
    unsigned int    cur_index;

    cout << endl;
    cout << "########### showHuffmanProcessStatus (BEGIN) ###########" << endl;
    if (!msg_i.empty ())
    {
        cout << "\t" << "===== " << msg_i << " =====" << endl;
        cout << endl;
    }
    cout << endl;
    cout << "Process StageNo    = " << cur_stage_i << endl;
    cout << "ProcessVector Size = " << vectorHuffmanProcess_i.size () << endl;
    cout << endl;

    string                  tmp_string;
    const unsigned int      setw_weight_CNS = 12;
    typename Node<SYMBOL, WEIGHT>::Node_MAP_SYMBOLS::const_iterator  const_iterSymbols;
    const string            linedel_CNS = "--------------------------------------------------";
    for (cur_index = 0; cur_index < vectorHuffmanProcess_i.size (); cur_index++)
    {
        if ((cur_index == 0) || (cur_index == ARY))
        {
            cout << linedel_CNS << endl;
        }
        //---------------------------------------


        if (vectorHuffmanProcess_i [cur_index]->is_TerminalNode_)
        {
            tmp_string = " " + to_str (vectorHuffmanProcess_i [cur_index]->weight_) + " ";
        }
        else
        {
            tmp_string = "[" + to_str (vectorHuffmanProcess_i [cur_index]->weight_) + "]";
        }

        cout.setf (ios::right, ios::adjustfield);
        cout << ""
             << setw (5)
             << (cur_index + 1)
             << ((cur_stage_i == vectorHuffmanProcess_i [cur_index]->creation_stage_) ? "*" : " ")
             << " -> "
             << setw (setw_weight_CNS + 2)
             << tmp_string.c_str ();

        if (!(vectorHuffmanProcess_i [cur_index]->is_TerminalNode_))
        {
            for (const_iterSymbols = vectorHuffmanProcess_i [cur_index]->mapSymbols_.begin();
                    !(const_iterSymbols == vectorHuffmanProcess_i [cur_index]->mapSymbols_.end());
                    const_iterSymbols++
                )
            {
                cout << endl;
                cout << "\t";
                cout << "\t";
                cout << "\t";
                cout << ""
                     << setw (setw_weight_CNS)
                     << (*const_iterSymbols).second
                     << ", "
                     << setw (getLongestSymbolSize ())
                     << (*const_iterSymbols).first
                     << ", ";
                if (vectorHuffmanProcess_i.size () > 1)
                {
                    cout << "...";
                }
                cout <<(*(mapAlphabet_.find ((*const_iterSymbols).first))).second.symbol_path_
                     << "  ";
            } // for (const_iterSymbols = ...
        } // if (!(vectorHuffmanProcess_i [cur_index]->is_TerminalNode_))
        else
        {
            ASSERT (vectorHuffmanProcess_i [cur_index]->mapSymbols_.size () == 1);
            const_iterSymbols = vectorHuffmanProcess_i [cur_index]->mapSymbols_.begin();
            ASSERT ((*const_iterSymbols).second == vectorHuffmanProcess_i [cur_index]->weight_);
            cout << ", " << ((*const_iterSymbols).first);
        }

        cout << endl;
    }
    cout << endl;

    cout << "########### showHuffmanProcessStatus (END) #############" << endl;
    cout << endl;

} // void showHuffmanProcessStatus



//#######################################
// static
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::print_show_line_S (
    const string&   spaces_offset_i,
    unsigned int    setw_symbol_i,
    const SYMBOL&   symbol_i,
    unsigned int    setw_weight_i,
    const WEIGHT&   weight_i,
    const vector<CODE>&     path_i
)
{
    string  tmp_string;

    strstream       tmp_strstream_weight;
    tmp_strstream_weight << weight_i;
    tmp_strstream_weight << ends;
    tmp_string = tmp_strstream_weight.str();
    tmp_strstream_weight.rdbuf()->freeze (0);
    cout.setf (ios::right, ios::adjustfield);
    cout << setw (setw_weight_i)
         << tmp_string.c_str ();


    strstream       tmp_strstream_symbol;
    tmp_strstream_symbol << symbol_i;
    tmp_strstream_symbol << ends;
    tmp_string = tmp_strstream_symbol.str();
    tmp_strstream_symbol.rdbuf()->freeze (0);
    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i
         << setw (setw_symbol_i)
         << tmp_string.c_str ();

    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i
         << gstr_path<ARY> (path_i);

    cout << endl;
} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::print_show_line_S



//#######################################
// static
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::print_show_title_S (
    const string&   spaces_offset_i,
    unsigned int    setw_symbol_i,
    const string&   symbol_title_i,
    unsigned int    setw_weight_i,
    const string&   weight_title_i,
    const string&   code_title_i
)
{
    cout.setf (ios::right, ios::adjustfield);
    cout << setw (setw_weight_i)
         << weight_title_i.c_str ();

    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i
         << setw (setw_symbol_i)
         << symbol_title_i.c_str ();

    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i
         << code_title_i.c_str ();

    cout << endl;

    const char      the_char = '-';
    unsigned int    cur_index;
    cout.setf (ios::right, ios::adjustfield);
    cout << setw (setw_weight_i - weight_title_i.size ()) << string ().c_str ();
    for (cur_index = 0; cur_index < weight_title_i.size (); cur_index++)
    {
        cout << the_char;
    }

    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i;
    for (cur_index = 0; cur_index < symbol_title_i.size (); cur_index++)
    {
        cout << the_char;
    }

    cout.setf (ios::left, ios::adjustfield);
    cout << spaces_offset_i;
    for (cur_index = 0; cur_index < code_title_i.size (); cur_index++)
    {
        cout << the_char;
    }

    cout << endl;

} // void BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::print_show_title_S



//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeOneSymbol (
    const vector<CODE>&     encoded_msg_i,
    unsigned int&           cur_start_position_io,
    unsigned int&           cur_symbol_number_io,
    vector<CODE>&           cur_symbol_code_o,
    SYMBOL&                 cur_symbol_value_o
) const

{
    bool    ret_boolValue = false;
    unsigned int cur_index;

    const InternalNode<SYMBOL, WEIGHT>*     curPtrInternalNode = &rootNode_;
    ASSERT (!(curPtrInternalNode == NULL));
    ASSERT (!(rootNode_.is_TerminalNode_));
    ASSERT (cur_start_position_io < encoded_msg_i.size ());

    CODE            cur_digit;
    //===================
    cur_symbol_code_o = vector<CODE> ();
    for (cur_index = cur_start_position_io;
            cur_index < encoded_msg_i.size ();
            cur_index++
        )
    {
        cur_digit = encoded_msg_i [cur_index];
        if (!((cur_digit >= 0) & (cur_digit < ARY)))
        {
            FATAL_MSG ("Illegal digit in encoded message"
                       << endl
                       << FATAL_SHIFT
                       << "Digit ["
                       << cur_index
                       << "] = "
                       << encoded_msg_i [cur_index]
                       << "; Valid range is ["
                       << 0
                       << " - "
                       << (ARY - 1)
                       << "]"
                       << endl
                       << FATAL_SHIFT
                       << "Note! First digit is 0-th digit"
                       << endl
                       << FATAL_SHIFT
                       << "The encoded message is <"
                       << gstr_path<ARY> (encoded_msg_i)
                       << ">"
                      );
        }
        //======================

        ASSERT (!(curPtrInternalNode->arc_ [cur_digit] == NULL));
        if (curPtrInternalNode->arc_ [cur_digit]->is_TerminalNode_)
        {
            ret_boolValue = true;
            break;
        }

        //======================
        ASSERT (!(curPtrInternalNode->arc_ [cur_digit] == NULL));
        curPtrInternalNode = dynamic_cast<InternalNode<SYMBOL, WEIGHT>*> (curPtrInternalNode->arc_ [cur_digit]);
        ASSERT (!(curPtrInternalNode == NULL));
        //======================
    } // for (cur_index =

    //===================
    if (!ret_boolValue)
    {
        cur_index--;
        ERROR_MSG ("Illegal last code in encoded message"
                   << endl
                   << ERROR_SHIFT
                   << "Digits ["
                   << cur_start_position_io
                   << ", "
                   << cur_index
                   << "] = "
                   << gstr_vector (encoded_msg_i, cur_start_position_io, cur_index)
                   << endl
                   << ERROR_SHIFT
                   << "Note! First digit is 0-th digit"
                   << endl
                   << ERROR_SHIFT
                   << "The encoded message is <"
                   << gstr_path<ARY> (encoded_msg_i)
                   << ">"
                  );
        return ret_boolValue;
    }

    //===================
    copy (encoded_msg_i.begin () + cur_start_position_io, encoded_msg_i.begin () + cur_index + 1, back_inserter (cur_symbol_code_o));

    //===================
    ASSERT (mapHuffmanCodes_.count (cur_symbol_code_o));
    cur_symbol_value_o = (*(mapHuffmanCodes_.find (cur_symbol_code_o))).second;
    cur_start_position_io = cur_index + 1;
    cur_symbol_number_io++;

    //===================

    return ret_boolValue;


} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeOneSymbol



//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeOneSymbol (
    const vector<CODE>&     encoded_symbol_code_i,
    SYMBOL&                 decoded_symbol_value_o
) const

{
    bool    ret_boolValue = false;
    ASSERT (!(encoded_symbol_code_i.empty ()));

    const InternalNode<SYMBOL, WEIGHT>*     curPtrInternalNode = &rootNode_;
    ASSERT (!(curPtrInternalNode == NULL));
    ASSERT (!(rootNode_.is_TerminalNode_));

    unsigned int    cur_index;
    CODE            cur_digit;
    //===================
    for (cur_index = 0; cur_index < (encoded_symbol_code_i.size () - 1); cur_index++)
    {
        cur_digit = encoded_symbol_code_i [cur_index];
        ASSERT (cur_digit >= 0);
        ASSERT (cur_digit < ARY);

        ASSERT (!(curPtrInternalNode->arc_ [cur_digit] == NULL));
        ASSERT (!(curPtrInternalNode->arc_ [cur_digit]->is_TerminalNode_));


        if (curPtrInternalNode->arc_ [cur_digit]->is_TerminalNode_)
        {
            return ret_boolValue;   // false
        }

        curPtrInternalNode = dynamic_cast<InternalNode<SYMBOL, WEIGHT>*> (curPtrInternalNode->arc_ [cur_digit]);
        ASSERT (!(curPtrInternalNode == NULL));
    } // for (cur_index =

    //================================
    cur_index = encoded_symbol_code_i.size () - 1;
    //===================
    cur_digit = encoded_symbol_code_i [cur_index];
    ASSERT (!(curPtrInternalNode->arc_ [cur_digit] == NULL));
    ASSERT (curPtrInternalNode->arc_ [cur_digit]->is_TerminalNode_);
    if ((!curPtrInternalNode->arc_ [cur_digit]->is_TerminalNode_))
    {
        return ret_boolValue;   // false
    }
    ret_boolValue = true;

    //===================
    ASSERT (mapHuffmanCodes_.count (encoded_symbol_code_i));
    decoded_symbol_value_o = (*(mapHuffmanCodes_.find (encoded_symbol_code_i))).second;
    //===================

    return ret_boolValue;


} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeOneSymbol



//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::testAllCodes (bool show_i) const
{
    //======================
    if (show_i)
    {
        showAll ("testAllCodes");
    }
    //======================

    bool    ret_boolValue = false;
    SYMBOL  cur_decoded_symbol_value;
    ASSERT (!(vectorHuffmanCodes_.empty ()));
    for (unsigned int cur_index = 0;
            cur_index < vectorHuffmanCodes_.size ();
            cur_index++
        )
    {
        ret_boolValue = decodeOneSymbol (vectorHuffmanCodes_ [cur_index], cur_decoded_symbol_value);
        if (show_i)
        {
            cout <<"TESTED : Code <"
                 << gstr_path<ARY> (vectorHuffmanCodes_ [cur_index])
                 << "> == Symbol <"
                 << cur_decoded_symbol_value
                 << ">"
                 << endl;
        }
        ASSERT (ret_boolValue);
    }

    return ret_boolValue;
} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::testAllCodes ()



//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeMsg (
    const vector<CODE>&     encoded_msg_i,
    vector<SYMBOL>&         decoded_msg_o
) const

{
    bool    ret_boolValue = false;
    if (encoded_msg_i.empty ())
    {
        FATAL_MSG ("Empty message to be decoded");
    }

    //===================
    decoded_msg_o = vector<SYMBOL> ();
    //===================
    unsigned int    cur_start_position = 0;
    unsigned int    cur_symbol_number = 0;
    SYMBOL          cur_symbol_value;
    vector<CODE>    cur_symbol_code;
    vector<vector<CODE> >   all_symbols_codes;

    while ((ret_boolValue = decodeOneSymbol (
                                encoded_msg_i,
                                cur_start_position,
                                cur_symbol_number,
                                cur_symbol_code,
                                cur_symbol_value
                            )))
    {
        decoded_msg_o.push_back (cur_symbol_value);
        all_symbols_codes.push_back (cur_symbol_code);
        if (!(cur_start_position < encoded_msg_i.size ()))
        {
            break;
        }
    } // while

    //=================
    if (!ret_boolValue)
    {
        FATAL_MSG ("Cannot decode all message -> "
                   << gstr_path<ARY> (encoded_msg_i)
                   << endl
                   << FATAL_SHIFT
                   << "Only "
                   << cur_symbol_number
                   << " symbols decoded"
                   << endl
                   << FATAL_SHIFT
                   << "Decoded codes -> "
                   << gstr_vector(all_symbols_codes, string (" "))
                   << endl
                   << FATAL_SHIFT
                   << "Decoded symbols ->  "
                   << gstr_vector(decoded_msg_o)
                  );
    }

    //===================
    return ret_boolValue;


} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeMsg


//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeMsg (
    const vector<SYMBOL>&   source_msg_i,
    vector<CODE>&           encoded_msg_o
) const

{
    bool    ret_boolValue = false;

    ASSERT (!source_msg_i.empty ());
    encoded_msg_o = vector<CODE> ();

    vector<CODE>    cur_symbol_code;

    for (unsigned int cur_index = 0;
            cur_index < source_msg_i.size ();
            cur_index++
        )
    {
        if (!(ret_boolValue = encodeOneSymbol (source_msg_i [cur_index], cur_symbol_code)))
        {
            break;
        }

        copy (cur_symbol_code.begin (), cur_symbol_code.end (), back_inserter (encoded_msg_o));

    }

    // ================
    if (!ret_boolValue)
    {
        FATAL_MSG ("Cannot encode message"
                   << endl
                   << FATAL_SHIFT
                   << gstr_vector (source_msg_i)
                  );
    }

    return ret_boolValue;


} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeMsg


//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeMsg (
    const basic_string<SYMBOL>&     string_source_msg_i,
    string&                         string_encoded_msg_o
) const

{
    //=================================
    if (!(ARY <= 16))
    {
        FATAL_MSG ("Illegal ARY for THIS FUNCTION : Real = "
                   << ARY
                   << "; must be <= 16"
                  );
    }
    ASSERT (ARY <= 16);     // In this function !!!
    //=================================

    bool    ret_value = false;


    vector <SYMBOL> vector_source_msg (string_source_msg_i.size ());
    for (unsigned int cur_index = 0; cur_index < vector_source_msg.size (); cur_index++)
    {
        vector_source_msg [cur_index] = string_source_msg_i [cur_index];
    }

    vector <CODE>   encoded_msg;
    ret_value = encodeMsg (vector_source_msg, encoded_msg);

    //=====================
    string_encoded_msg_o = string ();
    string_encoded_msg_o.resize (encoded_msg.size ());

    strstream       tmp_strstream;
    for (unsigned int cur_index = 0; cur_index < string_encoded_msg_o.size (); cur_index++)
    {
        tmp_strstream << encoded_msg [cur_index];
    }
    tmp_strstream << ends;
    string_encoded_msg_o = tmp_strstream.str();
    tmp_strstream.rdbuf()->freeze (0);

    //=====================
    return ret_value;
} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::encodeMsg (



//#######################################
template <typename SYMBOL, typename WEIGHT, unsigned int ARY>
bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeMsg (
    const string&           string_encoded_msg_i,
    basic_string<SYMBOL>&   string_decoded_msg_o
) const

{
    //=================================
    if (!(ARY <= 16))
    {
        FATAL_MSG ("Illegal ARY for THIS FUNCTION : Real = "
                   << ARY
                   << "; must be <= 16"
                  );
    }
    ASSERT (ARY <= 16);     // In this function !!!
    //=================================

    bool    ret_value = false;
    vector <CODE>   vector_encoded_msg (string_encoded_msg_i.size ());
    unsigned int    eof_int_test;
    string          cur_string;
    for (unsigned int cur_index = 0; cur_index < vector_encoded_msg.size (); cur_index++)
    {
        ASSERT (isxdigit (string_encoded_msg_i [cur_index]));

        cur_string = string_encoded_msg_i [cur_index];
        istrstream      int_stream (cur_string.c_str ());

        //int_stream.clear ();
        int_stream >> hex >> vector_encoded_msg [cur_index];
        ASSERT (!(int_stream.eof ()));
        ASSERT (!(int_stream.fail ()));
        ASSERT ((int_stream >> hex >> eof_int_test).eof ());

        int_stream.rdbuf()->freeze (0);
    }

    vector <SYMBOL> vector_decoded_msg;
    ret_value = decodeMsg (vector_encoded_msg, vector_decoded_msg);
    //=====================
    string_decoded_msg_o = basic_string<SYMBOL> ();
    string_decoded_msg_o.resize (vector_decoded_msg.size ());
    for (unsigned int cur_index = 0; cur_index < string_decoded_msg_o.size (); cur_index++)
    {
        string_decoded_msg_o [cur_index] = vector_decoded_msg [cur_index];
    }

    //=====================
    return ret_value;
} // bool BasicHuffmanTree<SYMBOL, WEIGHT, ARY>::decodeMsg (





//#######################################################
//##### PART : template class DriedHuffmanTree ##########
//############ Methods ##################################
//#######################################################

//-----------------------
// Constructor-1
template <typename WEIGHT, unsigned int ARY>
DriedHuffmanTree<WEIGHT, ARY>::DriedHuffmanTree (
    const vector<WEIGHT>& weight_vector_i
)
{
    doDriedHuffmanTree (weight_vector_i);
} // DriedHuffmanTree<WEIGHT, ARY>::DriedHuffmanTree ()


//-----------------------
// Constructor-2
template <typename WEIGHT, unsigned int ARY>
DriedHuffmanTree<WEIGHT, ARY>::DriedHuffmanTree (const string& weights_file_name_i)
{
    vector<WEIGHT>  weight_vector;

    ifstream fin (weights_file_name_i.c_str ());

    if (!fin)
    {
        FATAL_MSG ("Cannot open file <"
                   << weights_file_name_i
                   << "> for reading"
                   << endl
                   << FATAL_SHIFT
                   << "The file must contain weights to be Huffman-coded"
                  );
    }

    copy(istream_iterator<WEIGHT>(fin),
         istream_iterator<WEIGHT>(),
         back_inserter(weight_vector));

    doDriedHuffmanTree (weight_vector);
} // DriedHuffmanTree<WEIGHT, ARY>::DriedHuffmanTree ()


//##########################################################################

//-----------------------
template <typename WEIGHT, unsigned int ARY>
void DriedHuffmanTree<WEIGHT, ARY>::doDriedHuffmanTree (
    const vector<WEIGHT>& weight_vector_i
)
{
    vector<Cell<string, WEIGHT> >        data_vector;
    for (unsigned int cur_index = 0;
            cur_index < weight_vector_i.size ();
            cur_index++
        )
    {
        data_vector.push_back (Cell<string, WEIGHT> (
                                   to_str (
                                       cur_index + 1,
                                       get_width (weight_vector_i.size ()),
                                       '0',
                                       "Symbol#"
                                   ),
                                   weight_vector_i [cur_index]
                               ));
    }


    doBasicHuffmanTree (data_vector);

} // DriedHuffmanTree<WEIGHT, ARY>::DriedHuffmanTree ()


#endif  // huf_methods_H

//#######################################################
//################ END OF FILE ##########################
//#######################################################
