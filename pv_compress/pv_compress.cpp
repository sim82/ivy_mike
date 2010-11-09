#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <stdint.h>
#include "huffman.h"
#include <lzo/lzo1.h>

int mainx();

int main() {
    //mainx();
    std::ifstream f("/scratch/pv.bin");

    std::vector<std::vector<double> > pvs;

    f.seekg(0, std::ios_base::end);
    size_t fsize = f.tellg();
    f.seekg(0);

    std::vector<int> wv(256);

    std::vector<unsigned char> pv_lzo;
    std::vector<unsigned char> pv_lzod;
    char lzo_wrkmem[LZO1_MEM_COMPRESS];
    
    size_t csize = 0;
    
    while( f.tellg() < fsize ) {
        
       
       int sn = 0;
       f.read((char*)&sn, sizeof(int));
       int bw = 0;
       f.read((char*)&bw, sizeof(int));
    //        printf( "sn: %d %d\n", sn, bw );
       
       pvs.push_back(std::vector<double>());
       
       std::vector< double >& pv = pvs.back();
       
       size_t dw = bw / sizeof(double);
       //printf( "rtesize: %zd\n", bs/sizeof(double));
       pv.resize(dw);
       pv_lzo.resize(bw);
       pv_lzod.resize(bw);
       f.read( (char*)pv.data(), bw );
       
//        uint64_t l = 0;
//        uint64_t *ta;
//        uint64_t stride = 0;
//        uint64_t alast = 0;
//        for( int i = 0; i < pv.size(); i+=16 )  {
//            ta = (uint64_t*) &pv[i];
//            
//            for( int j = 0; j < 16; j++ ) {
//                uint64_t pred = alast + stride;
//                uint64_t x = ta[j] ^ pred;
//                stride = ta[j] - alast;
//                alast = ta[j];
//                
//                ta[j] = x;
//            }
//            
//            
//        }
//    
       
//        unsigned char *bv = (unsigned char*) pv.data();
//        for( int i = 0; i < bw; i++ ) {
//            wv[int(bv[i])]++;
//        }
       
       lzo_uint cbw = bw;
       lzo1_compress( (uint8_t*)pv.data(), lzo_uint(bw), (uint8_t*)pv_lzo.data(), &cbw, lzo_wrkmem );
       csize += cbw;
       
       lzo_uint dbw = cbw;
       lzo1_decompress( (uint8_t*)pv_lzo.data(), cbw, (uint8_t*)pv_lzod.data(), &dbw, lzo_wrkmem );
    }
    printf( "csize: %zd\n", csize );

    std::vector<Cell<unsigned char, size_t> >wvc;

    for( int i = 0; i < wv.size(); i++ ) {
       
       wvc.push_back( Cell<unsigned char, size_t>( (unsigned char)i, wv[i] ) );
    }


//     LoadedHuffmanTree<unsigned char, size_t, 2> ht(wvc);
//     ht.showAll();
   
//     for( vector< std::vector< double > >::const_iterator pv = pvs.begin(); pv != pvs.end(); ++pv ) {
//         ht.encodeMsg()
//     }
        
   
}


// ==============================================================
//
//  Copyright (c) 1999-2001 by Alex Vinokur.  This work and all works
//  derived from it may be copied and modified without any
//  restrictions other than that a copy of this copyright notice
//  must be included in any copy of this work or any derived work.
//
// ==============================================================
static char id [] = "@(#)## n-ary Huffman Template Algorithm ## Author : Alex Vinokur ## "__FILE__;

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
//  FILE : huf_main.C
//
//  AUTHOR : Alex Vinokur
//
//  DESCRIPTION :
//
//         Definition and implementation of the following test classes :
//         ----------------------------------------------
//         - AAA        (non-char) symbol type
//         - BBB        (non-numerical) weight type
//         ----------------------------------------------
//
//         Running the following tests :
//         ----------------------------------------------
//         Test#1.1.  Creating Loaded 5-ary Huffman Tree
//                    from data vector
//                    with char-symbols and int-weights
//
//         Test#1.2.  Encoding and Decoding vector-message
//                    using 5-ary Huffman Tree
//
//         Test#1.3.  Encoding and Decoding string-message
//                    using 5-ary Huffman Tree
//
//         Test#2.    Creating Loaded 24-ary Huffman Tree
//                    from data vector
//                    with char-symbols and int-weights
//
//         Test#3.1.  Creating Loaded Binary Huffman Tree
//                    from data vector
//                    with char-symbols and int-weights
//
//         Test#3.2.  Encoding and Decoding vector-message
//                    using Binary Huffman Tree
//
//         Test#3.3.  Encoding and Decoding string-message
//                    using Binary Huffman Tree
//
//         Test#4.    Creating Dried (Unload) Binary Huffman Tree
//                    from data vector
//                    with int-weights
//                    Note. This vector contains Fibonacci sequence.
//
//         Test#5.    Creating Dried (Unload) Binary Huffman Tree
//                    from data file
//                    with int-weights
//
//         Test#6.    Creating Loaded Binary Huffman Tree
//                    from data file
//                    with char-symbols and int-weights
//
//         Test#7.    Creating Loaded Binary Huffman Tree
//                    from data vector
//                    with string-symbols and float-weights
//
//         Test#8.    Creating Loaded Binary Huffman Tree
//                    from data vector
//                    with AAA-symbols and BBB-weights
//         ----------------------------------------------
//
//  DATE           VERSION
//  ----           -------
//  Aug-26-1999    NHTA 1.0
//  Jul-05-2001    NHTA 1.1
//  Sep-11-2001    NHTA 1.2
//
// ##############################################################


//=======================
// #include "huf_methods.H"
//=======================



//###################################################
//############## "Symbols" Test Class ###############
//###################################################
class AAA
{
friend bool operator< (const AAA& inst1_i, const AAA& inst2_i);
friend ostream& operator<< (ostream& o, const AAA& instance_i);
        private :
                static unsigned int     counter_s;
                unsigned int            counter_;
        public :
                AAA () {counter_ = ++counter_s;}
                ~AAA () {}
};

//=========================
ostream& operator<< (ostream& o, const AAA& instance_i)
{
        return o << "AAA_" << instance_i.counter_;
}
//----------------------
bool operator< (const AAA& inst1_i, const AAA& inst2_i)
{
        return (inst1_i.counter_ < inst2_i.counter_);
}




//###################################################
//############## "Weight" Test Class ################
//###################################################
class BBB
{
friend ostream& operator<< (ostream& o, const BBB& instance_i);
friend bool operator< (const BBB& inst1_i, const BBB& inst2_i);
friend bool operator== (const BBB& inst1_i, const BBB& inst2_i);
friend BBB operator* (const BBB& inst1_i, unsigned int int_value_i);
friend BBB operator/ (const BBB& inst1_i, unsigned int int_value_i);
        private :
                int                     value_;
                static unsigned int     counter_s;
                unsigned int            counter_;

        public :
                BBB () {counter_ = ++counter_s; value_ = rand ();}
                ~BBB () {}

                BBB& operator+= (const BBB& inst_i)
                {
                        value_ += inst_i.value_;
                        return (*this);
                }

};

//=========================
ostream& operator<< (ostream& o, const BBB& instance_i)
{
        return o << "BBB_" << instance_i.counter_;
        //#########################################
        // Note! This operator shows bogus weight.
        // Real value (instance_i.value_) hidden.
        //#########################################
}
//----------------------
bool operator< (const BBB& inst1_i, const BBB& inst2_i)
{
        return (inst1_i.value_ < inst2_i.value_);
}
//----------------------
bool operator== (const BBB& inst1_i, const BBB& inst2_i)
{
        return (inst1_i.value_ == inst2_i.value_);
}
//----------------------
BBB operator* (const BBB& inst1_i, unsigned int int_value_i)
{
BBB bbb;
        bbb.value_ = bbb.value_*int_value_i;
        return bbb;
}
//----------------------
BBB operator/ (const BBB& inst1_i, unsigned int int_value_i)
{
BBB bbb;
        bbb.value_ = bbb.value_/int_value_i;
        return bbb;
}



//#####################################################
//=========================
unsigned int    AAA::counter_s (0);
unsigned int    BBB::counter_s (0);
//=========================
#if 1

//#####################################################
//############# main ##################################
//#####################################################
//==============================
int mainx ()
{

        //===========================================
vector<Cell<char, int> >        data_vector_01;
        for (unsigned char cur_char = 0x20; cur_char < 0x7d; cur_char++)
        {
                data_vector_01.push_back (Cell<char, int> (cur_char, ((cur_char%19 + 3)) * 7));
        }


LoadedHuffmanTree<char, int, 5>         tree_01 (data_vector_01);
        tree_01.showAll ("Test#1.1 : Creating Loaded 5-ary Huffman Tree from <char, int>-data vector");

LoadedHuffmanTree<char, int, 24>        tree_02 (data_vector_01);
        tree_02.showAll ("Test#2 : Creating Loaded 24-ary Huffman Tree from <char, int>-data vector");

LoadedHuffmanTree<char, int>            tree_03 (data_vector_01);
        tree_03.showAll ("Test#3.1 : Creating Loaded Binary Huffman Tree from <char, int>-data vector");

        //=======================================================
vector<char>    vector_source_msg;
vector<CODE>    vector_encoded_msg;
vector<char>    vector_decoded_msg;

string          string_source_msg;
string          string_encoded_msg;
string          string_decoded_msg;


        //============ vector msg ===============================
        cout << endl << "\tTest#1.2 : Encoding and Decoding vector-message using 5-ary Huffman Tree" << endl;

        fill_vector (vector_source_msg, string ("Hi, people! This is vector message from 5-ary Huffman Tree"));

        cout << "Source Message  : " << gstr_vector (vector_source_msg) << endl;

        if (tree_01.encodeMsg (vector_source_msg, vector_encoded_msg))
        {
                cout << "Encoded Message : " << gstr_vector (vector_encoded_msg) << endl;
        }
        else
        {
                cout << "Cannot encode Message <" << gstr_vector (vector_source_msg) << ">" << endl;
        }
        if (tree_01.decodeMsg (vector_encoded_msg, vector_decoded_msg))
        {
                cout << "Decoded Message : " << gstr_vector (vector_decoded_msg) << endl;
        }
        else
        {
                cout << "Cannot decode encoded Message <" << gstr_vector (vector_encoded_msg) << ">" << endl;
        }



        //============ string msg ===============================
        cout << endl << "\tTest#1.3 : Encoding and Decoding string-message using 5-ary Huffman Tree" << endl;

        string_source_msg = "Hi, people! This is string message from 5-ary Huffman Tree";

        cout << "Source Message  : " << string_source_msg << endl;

        if (tree_01.encodeMsg (string_source_msg, string_encoded_msg))
        {
                cout << "Encoded Message : " << string_encoded_msg << endl;
        }
        else
        {
                cout << "Cannot encode Message <" << string_source_msg << ">" << endl;
        }

        if (tree_01.decodeMsg (string_encoded_msg, string_decoded_msg))
        {
                cout << "Decoded Message : " << string_decoded_msg << endl;
        }
        else
        {
                cout << "Cannot decode encoded Message <" << string_encoded_msg << ">" << endl;
        }


        //======================================================
        //============ vector msg ===============================
        cout << endl << "\tTest#3.2 : Encoding and Decoding vector-message using Binary Huffman Tree" << endl;

        fill_vector (vector_source_msg, string ("Hi, people! This is vector message from Binary Huffman Tree"));

        cout << "Source Message  : " << gstr_vector (vector_source_msg) << endl;

        if (tree_03.encodeMsg (vector_source_msg, vector_encoded_msg))
        {
                cout << "Encoded Message : " << gstr_vector (vector_encoded_msg) << endl;
        }
        else
        {
                cout << "Cannot encode Message <" << gstr_vector (vector_source_msg) << ">" << endl;
        }
        if (tree_03.decodeMsg (vector_encoded_msg, vector_decoded_msg))
        {
                cout << "Decoded Message : " << gstr_vector (vector_decoded_msg) << endl;
        }
        else
        {
                cout << "Cannot decode encoded Message <" << gstr_vector (vector_encoded_msg) << ">" << endl;
        }



        //============ string msg ===============================
        cout << endl << "\tTest#3.3 : Encoding and Decoding string-message using Binary Huffman Tree" << endl;

        string_source_msg = "Hi, people! This is string message from Binary Huffman Tree";

        cout << "Source Message  : " << string_source_msg << endl;

        if (tree_03.encodeMsg (string_source_msg, string_encoded_msg))
        {
                cout << "Encoded Message : " << string_encoded_msg << endl;
        }
        else
        {
                cout << "Cannot encode Message <" << string_source_msg << ">" << endl;
        }

        if (tree_03.decodeMsg (string_encoded_msg, string_decoded_msg))
        {
                cout << "Decoded Message : " << string_decoded_msg << endl;
        }
        else
        {
                cout << "Cannot decode encoded Message <" << string_encoded_msg << ">" << endl;
        }




        //==============================================
        //==============================================
        //==============================================
vector<int>     weights_vector_01;
        weights_vector_01.push_back (1);
        weights_vector_01.push_back (1);
const unsigned int      this_start_index = weights_vector_01.size ();
        for (unsigned int the_index = this_start_index;
                          the_index < (this_start_index + 21);
                          the_index++)
        {
                weights_vector_01.push_back (weights_vector_01 [the_index - 2] + weights_vector_01 [the_index - 1]);
        }

DriedHuffmanTree<int>   tree_04 (weights_vector_01);
        tree_04.showAll ("Test#4 : Creating Dried Binary Huffman Tree from <int>-weights vector (Fibonacci sequence)");

DriedHuffmanTree<int>   tree_05 ("weights_file_01");
        tree_05.showAll ("Test#5 : Creating Dried Binary Huffman Tree from <int>-weights file");

LoadedHuffmanTree<char, int>    tree_06 ("data_file_01");
        tree_06.showAll ("Test#6 : Creating Loaded Binary Huffman Tree from <char, int>-data file");


        //===========================================
vector<Cell<string, float> >    data_vector_02;
        data_vector_02.push_back (Cell<string, float> ("a1", 0.0022));
        data_vector_02.push_back (Cell<string, float> ("aa22", 0.002));
        data_vector_02.push_back (Cell<string, float> ("aaa333", 0.02));
        data_vector_02.push_back (Cell<string, float> ("bx1", 0.0001));
        data_vector_02.push_back (Cell<string, float> ("bbyy22", 0.007));
        data_vector_02.push_back (Cell<string, float> ("czzz1", 0.0013));
        data_vector_02.push_back (Cell<string, float> ("cczzzzz22", 0.003));
        data_vector_02.push_back (Cell<string, float> ("ccczzzzzzzz333", 0.023));
LoadedHuffmanTree<string, float>                tree_07 (data_vector_02);
        tree_07.showAll ("Test#7 : Creating Loaded Binary Huffman Tree from <string, float>-data vector");


        //===========================================
vector<Cell<AAA, BBB> > data_vector_03;
        for (unsigned int the_index = 0;
                          the_index < 5;
                          the_index++)
        {
                data_vector_03.push_back (Cell<AAA, BBB> ());
        }
LoadedHuffmanTree<AAA, BBB>             tree_08 (data_vector_03);
        tree_08.showAll ("Test#8 : Creating Loaded Binary Huffman Tree from <AAA, BBB>-data vector");

        return 0;

} // main


//#######################################################
//################ END OF FILE ##########################
//#######################################################
#endif