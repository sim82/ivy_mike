#ifndef __ParsVecCluster_h
#define __ParsVecCluster_h

#include <cfloat>
#include <vector>
#include <map>
#include <fstream>
#include <auto_ptr.h>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/multi_array.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#include <boost/gil/extension/io/png_io.hpp>
//#include <boost/gil/






// static void initBits(int *b, int n ) {
//     for( int i = 0; i < n; i++ ) {
// 	int t = i;
// 	int bc = 0;
//
// 	while( t != 0 ) {
// 	    bc += (t & 1);
// 	    t >>= 1;
// 	}
// 	bits[i] = bc;
//
// 	printf( "bits %d: %d\n", i, bits[i] );
//     }
//
// }

class ParsVecCluster {
public:
    typedef std::vector<char> pv_t;

private:
    
    static void writePng( const boost::multi_array<float,2> &ma, const char *name ) {
    //     boost::multi_array<float,2>ma = ma_;
    //     int dim = ma.shape()[0];
    //
    //     ma[0][0] = ma[0][1];
    //     for( int i = 1; i < dim; i++ ) {
    // 	ma[i][i] = ma[i][i-1];
    //     }


	int dim = ma.shape()[0];

	using namespace boost::gil;
	gray8c_view_t v;

	printf( "ne: %zd %d\n", ma.num_elements(), dim );
	ptrdiff_t ne = ma.num_elements();


	float *data = (float*)ma.data();
	float min = 10000000;
	float max = 0;
	for ( float *dp = data, *end = data + ne; dp != end; ++dp ) {
	    min = std::min( min, *dp );
	    max = std::max( max, *dp );
	}

	float rng = max - min;

	unsigned char *tmp = new unsigned char[ma.num_elements()];
	unsigned char *tp = tmp;


	for ( float *dp = data, *end = data + ne; dp != end; ++dp, ++tp ) {
	    *tp = (unsigned char)((*dp - min) / rng * 255);

	}
	gray8c_view_t src = interleaved_view(dim, dim, (const gray8_pixel_t*)tmp,dim);
	png_write_view( name, src);


	delete[] tmp;
	//printf( "minmax: %f %f\n", min, max );

	//interleaved_view<>v();

    }
    
    
    typedef std::vector<uint8_t> cpv_t;
    
    static const uint32_t NBITS_SIZE = 256;
    uint8_t m_nBits[NBITS_SIZE];
    void initNBits() {
	for( uint32_t i = 0; i < NBITS_SIZE; i++ ) {
	    m_nBits[i] = 0;
	    uint32_t t = i;
	    
	    
	    while( t != 0 ) {
		m_nBits[i]+=(t & 1);
		
		t >>= 1;
	    }
	    
	    
	    printf( "%d: %d\n", i, m_nBits[i] );
	}
    }
    
    void compressPV( const pv_t &in, cpv_t &out ) {
	size_t l = in.size();
	size_t lc = (l / 2) + ((l % 2));
	size_t lcpad = lc;
	
	
	if( (lcpad % 8) != 0 ) {
	    lcpad += 8 - (lcpad % 8);
	}
	
	assert( (lcpad % 8) == 0 );
	
	out.resize(0);
	out.reserve(lc);
	
	uint8_t acc = 0;
	
	for( int i = 0; i < l; i++ ) {
	    if( (i % 2) == 0 ) {
		acc = in[i];
	    } else {
		acc |= (in[i] << 4);
		out.push_back(acc);
	    }
	}
	if( (l % 2) == 1 ) {
	    out.push_back(acc);
	}
	
	assert( out.size() == lc );
	
	while( out.size() < lcpad ) {
	    out.push_back(0);
	}
	
    }
    
public:

    
    
// static int bits[16];

    static inline float parsDist( const pv_t &a, const pv_t &b ) {

        assert( a.size() == b.size() );

        static int bits[] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };

        int n = 0;
        for ( pv_t::const_iterator ia = a.begin(), ib = b.begin(), ea = a.end(); ia != ea; ++ia, ++ib ) {

#if 0
            if ( *ia != *ib ) {
                n++;
            }
#else
            int d = (((*ia) ^ (*ib)) & 15);
            //assert( d >= 0 && d <= 15 );

            n += bits[d];
#endif
        }
        return n;
    }
    inline float parsDistComp( const cpv_t &a, const cpv_t &b ) {

	// this version of the pars-dist function expects compressed parsimony vectors.
	// 
	
        assert( a.size() == b.size() );
	assert( (a.size() % 8) == 0 );
	
	// 32bit blocks seem to be the best choice on a core 2 duo
        typedef uint32_t le_t;
	const le_t *adat = (le_t *)a.data();
	const le_t *adat_end = (le_t *)(a.data() + a.size());
	const le_t *bdat = (le_t *)b.data();
	
        int n = 0;
        for ( ; adat != adat_end; ++adat, ++bdat ) {
            le_t d = ((*adat) ^ (*bdat));

            for( int i = 0; i < sizeof(le_t); i++ ) {
		// TODO: on core2duo this method seems to be a bit faster than progressively
		// shifting down d or accesing it as an array. This might be highly dependent on the cpu type (p4 had no barrel shifter?).
		n += m_nBits[(d >> (i*8)) & 255];
//		d >>= 8;
	    }
        }
        return n;
    }
    
    inline float parsDistComp1( const cpv_t &a, const cpv_t &b ) {

        assert( a.size() == b.size() );
	
        
        int n = 0;
        for ( cpv_t::const_iterator ia = a.begin(), ib = b.begin(), ea = a.end(); ia != ea; ++ia, ++ib ) {

            int d = ((*ia) ^ (*ib));
            //assert( d >= 0 && d <= 15 );
// 	    assert( d >= 0 );
// 	    assert( d < NBITS_SIZE );
            n += m_nBits[d];

        }
        return n;
    }
    
    struct ClusterStats {
        int c;
        float meandist;
        float maxdist;
        float meancdist;
        float maxcdist;
        int med;

    };

    std::vector <ClusterStats> m_stats;
    std::vector <std::vector<int> > m_assign;

    void buildDistanceMatrix( boost::multi_array<float,2> &dm, const std::map<int, std::vector<char> > &pvs, const boost::multi_array<float,2> &bbd, off_t nSeqs ) 
    {
    	std::vector<cpv_t> pvsComp( pvs.size() );
	
	printf( "calculating distance matrix\n" );
	float meandist = 0;
	int numdist = 0;
	int numall = 0;
	int numfiltered = 0;
	
	
	for ( off_t i = 0; i < nSeqs; i++ ) {
	    const std::vector<char> &sI = pvs.at(i);
	    
	    cpv_t &scI = pvsComp.at(i);
	    compressPV( sI, scI );
	    
	    
	    
	    // 	pvsv.push_back(sI);
	    for ( off_t j = 0; j < i; j++ ) {
		numall++;
#if 0
		if ( bbd[i][j] > 20 ) {
		    dm[i][j] = sI.size() * 4;
		    dm[j][i] = sI.size() * 4;
		    numfiltered++;
		} else
#endif
		{

		    const std::vector<char> &sJ = pvs.at(j);
		    cpv_t &scJ = pvsComp.at(j);
//                         float d = parsDist( sI, sJ );
		    
		    float d = parsDistComp(scI, scJ);
// // 			printf( "%f %f\n", d, cd );
// // 			assert( d == cd );
//                         
//                         if( d != cd ) {
// 			    printf( "%f %f\n", d, cd );
// 			}
		    
		    dm[i][j] = d;
		    dm[j][i] = d;
		    meandist += d;
		    numdist++;
		}

		//printf( "%d %d %f %\n", i, j, dm[i][j],  );
	    }

	    //printf( "ds for %d\n", i );
	}

	printf( "mean dist: %f filtered: %d of %d (%f)\n", meandist / numdist, numfiltered, numall, float(numfiltered) / numall );
// 	exit(0);
// 	if ( false && dname.size() > 0 )
// 	{
// 	    printf( "writing distance matrix\n" );
// 	    std::ofstream dos(dname.c_str());
// 	    dos.write((char*)dm.data(), dm.num_elements() * sizeof(float));
// 	}
//         } else {
//             std::ifstream dos(dname.c_str());
//             dos.read((char*)dm.data(), dm.num_elements() * sizeof(float));
//         }
    }
    
    void chooseRandomMedoids( const int K, const off_t nSeqs, std::vector<int> &cent ) {
	boost::mt19937 re;
        boost::uniform_int<int> rd( 0, nSeqs - 1 );
        boost::variate_generator<boost::mt19937&, boost::uniform_int<> > r(re, rd);


        for ( int i = 0; i < K; i++ ) {
            int rnd = r();
            while ( std::find( cent.begin(), cent.end(), rnd) != cent.end() ) {
                rnd = r();
            }
            cent.push_back(rnd);
            printf( "rnd: %d\n", rnd );
        }
    }
    ParsVecCluster( const std::map<int, std::vector<char> > &pvs, const boost::multi_array<float,2> &bbd, const int K, std::string dname = std::string() ) {
	initNBits();
	
	off_t nSeqs = pvs.size();

	assert( K <= nSeqs );
	
        boost::multi_array<float,2> dm(boost::extents[nSeqs][nSeqs]);
	buildDistanceMatrix( dm, pvs, bbd, nSeqs );


//     writePng( dm, "/space/test.png" );
        
        // this stuff is a bit hysteric

        std::vector<int> cent;
        cent.reserve(K);

	chooseRandomMedoids( K, nSeqs, cent );
	

	//
	// iterations of the K-medoid clustering
	//
        const int MAX_ITER = 20;
        int iter = 0;
        boolean breakIter = false;


        m_stats.resize(K);
        while ( iter < MAX_ITER && !breakIter ) {
            printf( "iter: %d\n", iter );

            // assign keeps the list of assigned instances for each of the K clusters

            m_assign.clear();
            m_assign.resize(K);

            // put the current medoids into their assign lists

            for ( int c = 0; c < K; c++ ) {
                m_assign.at(c).push_back( cent[c] );

            }

            // assign instances to clusters based on current medoids
            for ( int i = 0; i < nSeqs; i++ ) {
                if ( std::find( cent.begin(), cent.end(), i) != cent.end() ) {
                    continue;
                }

                float mind = FLT_MAX;
                int bestc = -1;
                for ( int c = 0; c < K; c++ ) {
                    int co = cent[c];

                    if ( dm[co][i] < mind ) {
                        mind = dm[co][i];
                        bestc = c;
                    }
                }
                //printf("assign: %d to %d\n", i, bestc );
                m_assign.at(bestc).push_back(i);
            }

            //     for( std::vector< int >::iterator it = cent.begin(); it != cent.end(); ++it ) {
            // 	printf( "c: %zd: %d\n", it - cent.begin(), *it );
            //     }




            // find new medoids
            int numChanges = 0;


            for ( int c = 0; c < K; c++ ) {
                ClusterStats &curStats = m_stats[c];
                // cass: list of instances assiged to current cluster
                std::vector<int> &cass = m_assign.at(c);
                int casss = cass.size();

                float minsum = FLT_MAX;
                int mini = -1;

                curStats.maxdist = 0;
                curStats.meandist = 0;
                int n = 0;
                curStats.maxcdist = 0;
                curStats.meancdist = 0;

                int centroid = cent[c];

                for ( int i = 0; i < casss; i++ ) {
                    float sum = 0.0;
                    int ce = cass[i];



                    for ( std::vector< int >::iterator it = cass.begin(); it != cass.end(); ++it ) {
                        sum += dm[ce][*it];
                        curStats.maxdist = std::max( curStats.maxdist, dm[ce][*it] );

                    }
                    // 	    printf( "sum: %d %d: %f\n", c, ce, sum );

                    curStats.meandist += sum;
                    n += cass.size();

                    curStats.maxcdist = std::max( curStats.maxcdist, dm[centroid][ce] );
                    curStats.meancdist += dm[centroid][ce];

                    if ( sum < minsum ) {
                        mini = i;
                        minsum = sum;
                    }
                }
                curStats.meandist /= n;
                curStats.meancdist /= casss;
                curStats.med = centroid;
//  	    printf( "mini %d: %d %d\t%f %zd\n", c, mini, cass[mini], minsum, cass.size() );
// 	    printf( "cluster %d: max: %f mean: %f maxc: %f meanc: %f\n", c, maxdist, meandist / n, maxcdist, meancdist / casss );


                if ( cent[c] != cass[mini] ) {
                    numChanges++;
                }
                cent[c] = cass[mini];
            }

            printf( "num changes: %d %d\n", numChanges, iter );
            breakIter = numChanges == 0;

            iter++;
        }

        FILE *of = fopen( "clusters.txt", "wb" );
        float meancentdist = 0;
        float mincentdist = FLT_MAX;
        int n = 0;
        for ( int i = 0; i < K; i++ ) {
            for ( int j = 0; j < i; j++ ) {
                meancentdist += dm[cent[i]][cent[j]];
                n++;
                mincentdist = std::min( mincentdist, dm[cent[i]][cent[j]] );
            }


            printf( "c %d: ", i );
            ClusterStats &curStats = m_stats[i];
            printf( "maxc: %f, meanc: %f cent: %d | ", curStats.maxcdist, curStats.meancdist, curStats.med );


            fprintf( of, "c %d:\n", i );

            for ( std::vector< int >::iterator it = m_assign.at(i).begin(), end = m_assign.at(i).end(); it != end; ++it ) {
                printf( "%d ", *it );

                assert( pvs.find(*it) != pvs.end() );
                const std::vector< char > pvec = pvs.at(*it);

                for ( std::vector< char >::const_iterator pvit = pvec.begin(); pvit != pvec.end(); ++pvit ) {
                    fprintf( of, "%x", *pvit );
                }
                fprintf( of, "\n" );
            }
            printf( "\n" );




        }

        printf( "meancentdist: %f mincentdist: %f\n", meancentdist / n, mincentdist );
	
	exit(0);
    }
    
    bool isMedoid( int branch ) {
	for( std::vector< ClusterStats >::const_iterator it = m_stats.begin(); it != m_stats.end(); ++it ) {
	    if( it->med == branch ) {
		return true;
	    }
	}
	
	return false;
	
    }
    
};
#endif