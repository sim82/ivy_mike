/*
 * Copyright (C) 2009 Simon A. Berger
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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
// #include <cstdint>
#include <malloc.h>
#include <boost/concept_check.hpp>
#include <auto_ptr.h>
#include "LargePhylip.h"
#include <boost/timer.hpp>
#include <boost/multi_array.hpp>
typedef unsigned char u1_t;
typedef unsigned int u4_t;
#define BT_STAY 		(0x1)
#define BT_STAY_L 		(0x2)
#define BT_TOUCH 	(0x4)
#define BT_DIAG 	(0x8)
#define LARGE_VALUE  (10000000)

#define GAP_OPEN (1)
#define GAP_EXTEND (1)

#define MISMATCH_PENALTY (4)


// riskyvector aims to be the thinnest possible wrapper around a c-style array
// with some basic std::vector compatibility (mainly iterators).
// It provides absolutely no range checking and initialization!
template <typename T>
class riskyvector {
public:    
    typedef T* iterator;  
    typedef T&	reference;
    T* m_data;
    size_t m_size;

    riskyvector( size_t size ) : m_size(size) {
	m_data = new T[size];
    }

    riskyvector() {
	m_data = 0;
    }

    ~riskyvector() {
	if( m_data != 0 ) {
	    delete[] m_data;
	}
	m_data = 0;
    }

    void reserve( size_t size ) {
	if( m_data != 0 ) {
	    delete[] m_data;
	}
	m_size = size;
	m_data = new T[m_size];
	
    }
    
    void resize( size_t size ) {
	reserve(size);
    }
    
    inline iterator begin() {
	return m_data;
    }
    inline iterator end() {
	return m_data + m_size;
    }
    reference operator[](size_t n) { 
	return *(m_data + n); 
    }
//     }
};

class Aligner {
public:
#if 0
    typedef  std::vector<char> seq_t;
    typedef std::vector<float> sv_t;
    typedef std::vector<char> dv_t;
#else
    typedef  std::vector<char> seq_t;
    typedef riskyvector<float> sv_t;
    typedef riskyvector<char> dv_t;
#endif
private:
	
    seq_t m_seqA;
    seq_t m_seqB;
    
    sv_t m_score;
    sv_t m_scoreL;
    
    
    dv_t m_dir;
    
    int m_tbStartA;
    int m_tbStartB;
    
    int m_W;
    int m_H;
    
    u1_t *cloneU1( u1_t *src, int len ) {
	u1_t *tmp = new u1_t[len];
	std::copy( src, src + len, tmp );
	
	return tmp;
    }
    
    inline size_t addr( int a, int b ) {
// 	size_t add = b * m_W + a;
// 	assert( add < m_W * m_H );
	return b * m_W + a;
    }
    
    inline size_t saddr( int a, int b ) {
	return addr( a + 1, b + 1 );
    }
public:
//     Aligner( u1_t *seqA, u1_t *seqB, int lenA, int lenB ) 
//     :
//     m_seqA( seq_t::iterator(seqA), seq_t::iterator(seqA + lenA) ),
//     m_seqB( seq_t::iterator(seqB), seq_t::iterator(seqB + lenB) ),
//     m_W( lenA + 1 ),
//     m_H( lenB + 1 )
//     {
// 	
// 	
// 	
// 	size_t size = m_W * m_H;
// 	m_score.reserve(size);
// 	m_scoreL.reserve(size);
// 	m_dir.reserve(size);
//     }
    
    Aligner( const seq_t::iterator &bseqA, const seq_t::iterator &eseqA, const seq_t::iterator &bseqB, const seq_t::iterator &eseqB ) 
    :
    m_seqA( bseqA, eseqA ),
    m_seqB( bseqB, eseqB ),
    m_W( eseqA - bseqA + 1),
    m_H( eseqB - bseqB + 1 )
    {
	
	
	
	size_t size = m_W * m_H;
	m_score.reserve(size);
	m_scoreL.reserve(size);
	m_dir.reserve(size);
    }
//     ~Aligner() {
// 	
//     }
    
#if 0  
    float alignFreeshift() {
	const size_t lenA = m_seqA.size();
	const size_t lenB = m_seqB.size();
	const int band_width = lenA - lenB;
	
	m_scoreL.push_back(0);
	m_score.push_back(0);
	m_dir.push_back(0);

	assert( lenA > lenB );
	for( size_t ia = 0; ia <= lenA - lenB - 1; ia++ ) {
// 	    m_scoreL[saddr ( ia, -1 ) ] = 0;
// 	    m_score[saddr ( ia, -1 ) ] = 0;
// 
// 	    m_dir[saddr ( ia, -1 ) ] = BT_STAY_L; 
	    m_scoreL.push_back(0);
	    m_score.push_back(0);
	    m_dir.push_back(BT_STAY_L);
	}
// 	m_score[saddr ( -1, -1 ) ] = 0;
// 	m_scoreL[saddr ( -1, -1 ) ] = 0;
// 	
// 	printf( "align freeshift\n" );
	for( size_t ib = 0; ib < lenB; ++ib ) {
	    const unsigned char cb = m_seqB[ib];
	    size_t saddr_0_0 = saddr ( ib, ib );
	    size_t saddr_1_1 = saddr ( ib-1, ib-1 );
	    size_t saddr_1_0 = saddr ( ib - 1, ib );
	    
	    assert( saddr_0_0 > m_score.size() );
	    m_score.resize(saddr_0_0);
	    m_scoreL.resize(saddr_0_0);
	    m_dir.resize(saddr_0_0);
	    sv_t::iterator sp_1_1 = m_score.begin() + saddr_1_1;
	    sv_t::iterator sp_1_0 = m_score.begin() + saddr_1_0;
	    sv_t::iterator sLp_1_0 = m_scoreL.begin() + saddr_1_0;
	    for ( size_t ia = ib; ia <= ib + band_width; ++ia, ++sp_1_1, ++sp_1_0, ++sLp_1_0 ) {
		const unsigned char ca = m_seqA[ia];
		
		float sd = *sp_1_1;
		float sl;
		char dir = 0;
		
		if( ca == '-' ) {
		    sd+=10;
		} else if( ca != cb ) {
		    sd += MISMATCH_PENALTY;
		}
		
		if( ia > ib ) {
		    float penOpen = GAP_OPEN + GAP_EXTEND;
		    float penExtend = GAP_EXTEND;
		    if( ca == '-' ) {
			penOpen = 0;
			penExtend = 0;
		    }
		    
		    
		    float scoreExt;
		    float scoreOpen;
		    
		    if( ia > ib + 1 ) {
			scoreExt = (*sLp_1_0) + penExtend;
		    } else {
			scoreExt = LARGE_VALUE;
		    }
		    
		    scoreOpen = (*sp_1_0) + penOpen;
		    
		    if( scoreExt < scoreOpen ) {
			sl = scoreExt;
			dir = BT_STAY_L;
		    } else {
			sl = scoreOpen;
		    }
		} else {
		    sl = LARGE_VALUE;
		}
		m_scoreL.push_back(sl);
		
		if( sl < sd && ia > ib ) {
		    m_score.push_back(sl);
		    m_dir.push_back(dir);
		} else {
		    m_score.push_back(sd);
		    m_dir.push_back(dir | BT_STAY);
		}
		
// 		printf( "sd: %zd %zd %f %f\n", ia, ib, sd, sl );
	    } // ia loop
	} // ib loop
	
	m_tbStartB = lenB - 1;
	
	float best = LARGE_VALUE;
	for( size_t a = lenA - 1; a >= lenB - 1; --a ) {
	    float s = m_score[saddr( a, m_tbStartB )];
	    
// 	    printf( "s: %f\n", s );
	    if( s < best ) {
		best = s;
		m_tbStartA = a;
	    }
		
	}
// 	printf( "start: %d %d\n", m_tbStartA, m_tbStartB );
	return m_score[saddr(m_tbStartA, m_tbStartB)];
    }

    float alignFreeshift2() {
	const size_t lenA = m_seqA.size();
	const size_t lenB = m_seqB.size();
	const int band_width = lenA - lenB;
	
	m_scoreL.push_back(0);
	m_score.push_back(0);
	m_dir.push_back(0);


	
	{
	    size_t size = m_W * m_H;
	    m_score.resize(size, 0.0f);
	    m_scoreL.resize(size, 0.0f);
	    m_dir.resize(size, 0);
    
	}
	for( size_t ia = 0; ia <= lenA - lenB - 1; ia++ ) {
 	    m_dir[saddr ( ia, -1 ) ] = BT_STAY_L; 
 	}
	
//  	printf( "align freeshift2\n" );
	for( size_t ib = 0; ib < lenB; ++ib ) {
	    const unsigned char cb = m_seqB[ib];
	    size_t saddr_0_0 = saddr ( ib, ib );
	    size_t saddr_1_1 = saddr ( ib-1, ib-1 );
	    size_t saddr_1_0 = saddr ( ib - 1, ib );
	    
	 
	   
	    sv_t::iterator sp_0_0 = m_score.begin() + saddr_0_0;
	    sv_t::iterator sLp_0_0 = m_scoreL.begin() + saddr_0_0;
	    sv_t::iterator sp_1_1 = m_score.begin() + saddr_1_1;
	    sv_t::iterator sp_1_0 = m_score.begin() + saddr_1_0;
	    sv_t::iterator sLp_1_0 = m_scoreL.begin() + saddr_1_0;
	    dv_t::iterator dp_0_0 = m_dir.begin() + saddr_0_0;
	    
	    for ( size_t ia = ib; ia <= ib + band_width; ++ia, ++sp_0_0, ++sLp_0_0, ++sp_1_1, ++sp_1_0, ++sLp_1_0, ++dp_0_0 ) {
		const unsigned char ca = m_seqA[ia];
		
		float sd = *sp_1_1;
		float sl;
		char dir = 0;
		
		if( ca == '-' ) {
		    sd+=10;
		} else if( ca != cb ) {
		    sd += MISMATCH_PENALTY;
		}
		
		if( ia > ib ) {
		    float penOpen = GAP_OPEN + GAP_EXTEND;
		    float penExtend = GAP_EXTEND;
		    if( ca == '-' ) {
			penOpen = 0;
			penExtend = 0;
		    }
		    
		    
		    float scoreExt;
		    float scoreOpen;
		    
		    if( ia > ib + 1 ) {
			scoreExt = (*sLp_1_0) + penExtend;
		    } else {
			scoreExt = LARGE_VALUE;
		    }
		    
		    scoreOpen = (*sp_1_0) + penOpen;
		    
		    if( scoreExt < scoreOpen ) {
			sl = scoreExt;
			*dp_0_0 = BT_STAY_L;
		    } else {
			sl = scoreOpen;
		    }
		} else {
		    sl = LARGE_VALUE;
		}
		*sLp_0_0 = sl;
		
		if( sl < sd && ia > ib ) {
		    *sp_0_0 = sl;
		    *dp_0_0 = dir;
		} else {
		    *sp_0_0 = sd;
		    *dp_0_0 |= BT_STAY;
		    
		}
		
// 		printf( "sd: %zd %zd %f %f\n", ia, ib, sd, sl );
	    } // ia loop
	} // ib loop
	
	m_tbStartB = lenB - 1;
	
	float best = LARGE_VALUE;
	for( size_t a = lenA - 1; a >= lenB - 1; --a ) {
	    float s = m_score[saddr( a, m_tbStartB )];
	    
// 	    printf( "s: %f\n", s );
	    if( s < best ) {
		best = s;
		m_tbStartA = a;
	    }
		
	}
// 	printf( "start: %d %d\n", m_tbStartA, m_tbStartB );
	return m_score[saddr(m_tbStartA, m_tbStartB)];
    }
#endif    
   
    float alignFreeshift3() {
	const size_t lenA = m_seqA.size();
	const size_t lenB = m_seqB.size();
	const int band_width = lenA - lenB;
	
	
	
	
	{
	    size_t size = m_W * m_H;
	    m_score.resize(size);
	    m_scoreL.resize(size);
	    m_dir.resize(size);
    
	}
	
	m_score[0] = 0;
	m_scoreL[0] = 0;
	m_dir[0] = 0;
	for( size_t ia = 0; ia <= lenA - lenB - 1; ia++ ) {
 	    m_score[saddr ( ia, -1 ) ] = 0.0; 
	    m_scoreL[saddr ( ia, -1 ) ] = 0.0; 
	    m_dir[saddr ( ia, -1 ) ] = BT_STAY_L; 
 	}
	
//  	printf( "align freeshift2\n" );
	for( size_t ib = 0; ib < lenB; ++ib ) {
	    const unsigned char cb = m_seqB[ib];
	    size_t saddr_0_0 = saddr ( ib, ib );
	    size_t saddr_1_1 = saddr ( ib-1, ib-1 );
	    size_t saddr_1_0 = saddr ( ib - 1, ib );
	    
	 
	   
	    sv_t::iterator sp_0_0 = m_score.begin() + saddr_0_0;
	    sv_t::iterator sLp_0_0 = m_scoreL.begin() + saddr_0_0;
	    sv_t::iterator sp_1_1 = m_score.begin() + saddr_1_1;
	    sv_t::iterator sp_1_0 = m_score.begin() + saddr_1_0;
	    sv_t::iterator sLp_1_0 = m_scoreL.begin() + saddr_1_0;
	    dv_t::iterator dp_0_0 = m_dir.begin() + saddr_0_0;
	    size_t ia_end = ib + band_width + 1;
	    for ( size_t ia = ib; ia != ia_end; ++ia, ++sp_0_0, ++sLp_0_0, ++sp_1_1, ++sp_1_0, ++sLp_1_0, ++dp_0_0 ) {
		const unsigned char ca = m_seqA[ia];
		
		float sd = *sp_1_1;
		float sl;
		char dir = 0;
		
		if( ca == '-' ) {
		    sd+=10;
		} else if( ca != cb ) {
		    sd += MISMATCH_PENALTY;
		}
		
		if( ia > ib ) {
		    float penOpen = GAP_OPEN + GAP_EXTEND;
		    float penExtend = GAP_EXTEND;
		    if( ca == '-' ) {
			penOpen = 0;
			penExtend = 0;
		    }
		    
		    
		    float scoreExt;
		    float scoreOpen;
		    
		    if( ia > ib + 1 ) {
			scoreExt = (*sLp_1_0) + penExtend;
		    } else {
			scoreExt = LARGE_VALUE;
		    }
		    
		    scoreOpen = (*sp_1_0) + penOpen;
		    
		    if( scoreExt < scoreOpen ) {
			sl = scoreExt;
			*dp_0_0 = BT_STAY_L;
		    } else {
			sl = scoreOpen;
		    }
		} else {
		    sl = LARGE_VALUE;
		}
		*sLp_0_0 = sl;
		
		if( sl < sd && ia > ib ) {
		    *sp_0_0 = sl;
		    *dp_0_0 = dir;
		} else {
		    *sp_0_0 = sd;
		    *dp_0_0 |= BT_STAY;
		    
		}
		
// 		printf( "sd: %zd %zd %f %f\n", ia, ib, sd, sl );
	    } // ia loop
	} // ib loop
	
	m_tbStartB = lenB - 1;
	
	float best = LARGE_VALUE;
	for( size_t a = lenA - 1; a >= lenB - 1; --a ) {
	    float s = m_score[saddr( a, m_tbStartB )];
	    
// 	    printf( "s: %f\n", s );
	    if( s < best ) {
		best = s;
		m_tbStartA = a;
	    }
		
	}
// 	printf( "start: %d %d\n", m_tbStartA, m_tbStartB );
	return m_score[saddr(m_tbStartA, m_tbStartB)];
    }
    void traceback( seq_t &out ) {
	int lenA = m_seqA.size();
	int idx_b = lenA - 1;
	int ba = m_tbStartA;
	int bb = m_tbStartB;
	int inL = 0;

	
	const seq_t &in = m_seqB;
	out.resize(m_seqA.size());

	if ( m_tbStartA < lenA - 1 ) {
	    for ( int ba = lenA - 1; ba > m_tbStartA; ba-- ) {
// 		memcpy ( &out[idx_b * elen * outstride], gap, elen );
		out[idx_b] = '-';
		idx_b--;
// 		printf( "pregap\n" );
	    }
	}

	// traceback

	while ( ( ba >= 0 || bb >= 0 ) ) {
	    if ( !inL ) {

		// we are currently not in a gap. see if we should match ('go diagonal') or open a gap ('jump to gap-matrix')
		if ( ( m_dir[saddr ( ba, bb ) ] & BT_STAY ) != 0 ) {
// 		    memcpy ( &out[idx_b * outbytestride], &in[bb * inbytestride], elen );
		    out[idx_b] = in[bb];
		    idx_b--;
		    ba--;
		    bb--;
		} else {
		    // open gap. keep position and switch to gap-matrix
		    inL = 1;
		}
	    } else {
		// we are in a gap

		// output gap
// 		memcpy ( &out[idx_b * outbytestride], gap, elen );
		out[idx_b] = '-';
		idx_b--;
		// check if we should stay in the gap-matrix
		inL = ( m_dir[saddr ( ba, bb ) ] & BT_STAY_L ) != 0;
		ba--;
	    }
	}
    }
    
    void printDir() {
	
	dv_t::iterator it = m_dir.begin();
// 	for( int ib = 0; ib < m_H; ib++ ) {
// 	    for( int ia = 0; ia < m_W; ia++, ++it ) {
// 		printf( "%d  ", *it );
// 	    }
// 	    printf( "\n" );
// 	}
	
	sv_t::iterator its = m_score.begin();
	sv_t::iterator itsl = m_scoreL.begin();
	for( int ib = 0; ib < m_H; ib++ ) {
	    for( int ia = 0; ia < m_W; ia++, ++it, ++its, ++itsl ) {
		printf( "%d|%d|%d\t", int(*its), std::min(int(*itsl),99), *it );
	    }
	    printf( "\n" );
	}
	
    }
};




// class Test {
//     std::vector<std::string> m_vt;
//     
//     static Test *load() {
// 	Test *t = new Test();
// 	
// 	t->m_vt.push_back( "bla1" );
// 	t->m_vt.push_back( "bla2" );
// 	t->m_vt.push_back( "bla3" );
// 	t->m_vt.push_back( "bla4" );
// 	t->m_vt.push_back( "bla5" );
// 	
// 	return t;
//     }
//     
// }

static inline bool isNonGap( char c ) {
    c = toupper(c);
    return (c == 'A' || c == 'C' || c == 'G' || c == 'T');
}
static inline std::string removeGaps( const std::string &s ) {
    int nng = 0;
    for( std::basic_string< char, std::char_traits< char > >::const_iterator it = s.begin(); it != s.end(); ++it ) {
	if( isNonGap(*it) ) {
	    nng++;
	}
    }
    std::string res;
    res.reserve(nng);
    for( std::basic_string< char, std::char_traits< char > >::const_iterator it = s.begin(); it < s.end(); ++it ) {
	if( isNonGap(*it) ) {
	    res += *it;
	}
    }
    
    return res;
}

class Bla {
public:    
    int a, b, c;
    Bla() : a(0), b(0), c(0) {}
};
int main(int argc, char **argv) {
   
//     std::auto_ptr<MultipleAlignment> ma(MultipleAlignment::loadPhylip( "/space/newPNAS/PNAS.phylip" ));
    
//     Bla bla1;
//     bla1 = 0;
//     printf( "bla: %d %d %d\n", bla1.a, bla1.b, bla1.c );
//     exit(-1);
    
    mallopt(M_MMAP_MAX, 0);
    mallopt(M_TRIM_THRESHOLD, -1);
    typedef Aligner::seq_t::iterator asi_t;
    if( false ) {
	std::string sA("actgac---ga--ctgacgt");
	std::string sB("aatgactgacgt");

	//     std::vector<char>seqA( sA.begin(), sA.end() );
	//     std::vector<char>seqB( sB.begin(), sB.end() );
	//     Aligner a( seqA.begin(), seqA.end(), seqB.begin(), seqB.end() );

	

	Aligner a( asi_t( &(*sA.begin())), asi_t(&(*sA.end())), asi_t(&(*sB.begin())), asi_t(&(*sB.end())) );

	float score = a.alignFreeshift3();

	printf( "score: %f\n", score );

	a.printDir();

	std::vector<char>outA;

	a.traceback(outA);

	std::string outAs( outA.begin(), outA.end() );
	printf( "%s\n%s\n%s\n", sA.c_str(), sB.c_str(), outAs.c_str() );
	// //    getchar();

	//     printf( "%s\n", map.getName(0).c_str() );
	//     {
	// 	std::vector<std::string> vt;
	// 	vt.push_back("bla1");
	// 	vt.push_back("bla2");	
	// 	vt.push_back("bla3");
	// 	vt.push_back("bla4");
	// 	
	// 	printf( "%s\n", vt.back().c_str() );
	//     }
	//     int *guard = 0;
    }
    
    const char *refname;
    const char *qsname;
    if( argc >= 3 ) {
	refname = argv[1];
	qsname = argv[2];
    } else {
	// FIXME: is this legal? The string constants should be (permanent) pointers into the ".rodata" section of the binary?

#if 0
	refname = "/space/newPNAS/PNAS.phylip.in";
 	qsname = "/space/newPNAS/PNAS.phylip.out";
#else
	refname = "/space/pa_testcase/1604_half_short/all.phy.in";
	qsname = "/space/pa_testcase/1604_half_short/all.phy.out";
#endif
    }
    LargePhylip ref(refname);
    LargePhylip qs(qsname);
    
    for( int j = 0, jm = qs.size(); j < jm; j++ ) {
// 	    printf( "j: %d\n", j );
	std::string sJ = removeGaps(qs.getSequence(j));
    
// 	printf( "qs: %d\n", j );
	size_t refCompl = 0;
	boost::timer t1;
	
	float bestScore = LARGE_VALUE;
	Aligner::seq_t bestAli(ref.getSeqLen(), 0 );
	
	for( int i = 0, im = ref.size(); i < im; i++ ) {
	    std::string sI = ref.getSequence(i);
	
	    refCompl += sI.size();
	    
	    Aligner a( asi_t( &(*sI.begin())), asi_t(&(*sI.end())), asi_t(&(*sJ.begin())), asi_t(&(*sJ.end())) );
// 	    if( guard == 0 ) {
// 		guard = new int[10000];
// 	    }
	    
// 	   float score = a.alignFreeshift();
	   float score = a.alignFreeshift3();
	   
	   if( score < bestScore ) {
		a.traceback(bestAli);
		
	   }
	   //float score3 = a.alignFreeshift3();
//   	   printf( "score: %f\n", score );
//  	   printf( "align %d %d: %f\n", i, j, score );
	}
	
	double time = t1.elapsed();
	size_t qsCompl = sJ.size();

	size_t allCompl = qsCompl * refCompl;
	double clockRate = 2.40 * 1000 * 1000 * 1000; // 2.4 ghz
	double numClocks = clockRate * time;
	
	printf( "%s  %s\n", qs.getName(j).c_str(), std::string(bestAli.begin(), bestAli.end()).c_str() );
	
// 	printf( "elapsed for %zd * %zd = %zd: %f (%f clks/cell)\n", refCompl, qsCompl, allCompl, time, numClocks / allCompl );
    }
    
    return 0;
}
