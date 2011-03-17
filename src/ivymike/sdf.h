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


#ifndef __ivymike_sdf
#define __ivymike_sdf
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include "boost/array.hpp"

namespace ivy_mike {

class sdf {
class sanity_check : public std::runtime_error {
};
    
public:
    struct atom {
        float   m_x, m_y, m_z;
//        char    m_ele;
        
        typedef  boost::array<char,3> ele_t;
        ele_t m_ele;
        int     m_can_ele;
        //std::string     m_extra;
        
        atom( float x, float y, float z, char *ele, int can_ele ) : m_x(x), m_y(y), m_z(z), m_can_ele(can_ele)
        {
            m_ele = to_ele( ele );
            
      //      printf( "atom: %f %f %f %c\n", x,y,z,ele);
        }
        
        static inline ele_t to_ele( char *ele ) {
         
            ele_t r;
            
            int nns = 0;
            for( uint i = 0; i < ele_t::size(); i++ ) {
             
                if( !isspace(ele[i] ) ) {
                    nns++;
                    
                }
            }
            
            if( nns < 1 || nns > 2 ) {
                std::stringstream ss;
                ss << "nns < 1 || nns > 2: '" << ele[0] << ";" << ele[1] << ";" << ele[2] << "'";
               
                
                throw std::runtime_error( ss.str() );
            }
            
            
            
            std::copy( ele, ele + ele_t::size(), r.begin() );
            return r;
        }
        
    };
    struct bond {
        std::pair <int,int>     m_atoms;
        int                     m_type;
        int                     m_can_type;
       // std::string             m_extra;
        
        
        bond( int first, int second, int type, int can_type ) : m_atoms(first,second), m_type(type), m_can_type(can_type)
        {
        //    printf( "bond: %d %d %d\n", m_atoms.first, m_atoms.second, m_type );
        }
        
    };
    
    struct molecule {
        std::string     m_header;
        std::string     m_comment;
        std::vector<atom> m_atoms;
        std::vector<bond> m_bonds;
        
        inline int size() const {
         
            return int(m_atoms.size());
        }
        
        //std::map<std::string,std::string> m_extra;
        
    };
private:
    std::map<int,int> m_bondtype_map;
    std::map<atom::ele_t,int> m_atomtype_map;
    
    const bool m_allow_hydrogen;
    
    int canonicalize_element( const atom::ele_t &ele ) {
        
        
        std::map< atom::ele_t, int >::iterator it = m_atomtype_map.find( ele );
        if( it == m_atomtype_map.end() ) {
            int nid = m_atomtype_map.size();
            m_atomtype_map[ele] = nid;
            return nid;
        } else {
         
            return it->second;
        }
        
    }
    
    int canonicalize_element(char *ele) {
     
        atom::ele_t et = atom::to_ele(ele);
        return canonicalize_element(et);
        
    }
    
    int canonicalize_bond_type(int type) {
        std::map< int, int >::iterator it = m_bondtype_map.find(type);
        if( it == m_bondtype_map.end() ) {
            int nid = m_bondtype_map.size();
            m_bondtype_map[type] = nid;
            return nid;
        } else {
         
            return it->second;
            
        }
    }
    
    std::vector<molecule> m_molecules;
    bool parse_molecule( std::vector<molecule> &cont, std::ifstream &is) {
        // parse one molecule block from the stream.
        // implented as a 'free interpretation' of (Arthur Dalby et al., J. Chem. Inf. Comput. Sci, 1992, 32, 244-255)
        
        
        const size_t line_len = 256;
        char line[line_len];    
        line[0] = 0;
        
        while( line[0] == 0 && !is.eof() ) {
            is.getline(line, line_len);
        }
        if( is.eof()) {
            return false;
        }
        
        
        cont.resize( cont.size() + 1 );
        molecule &mol = cont.back();
        
        mol.m_header = line;
        is.getline(line, line_len); assert( !is.eof() );
        
        //mol.m_comment = line;
        is.getline(line, line_len); assert( !is.eof() );
        int natoms, nbonds;
        // parse 'count line'
        
        {
            char nt[5];
            is.getline(line, line_len); assert( !is.eof() );
            
            nt[3] = 0;
            
            std::copy( line, line + 3, nt );
            natoms = atoi(nt);
            
            std::copy( line + 3, line + 3 + 3, nt );
            nbonds = atoi(nt);
            
//             printf( "%d %d\n", natoms, nbonds );
            
            //assert( memcmp( line + 33, " V2000", 6 ) == 0 );
        }
        
        mol.m_atoms.reserve( natoms );
        mol.m_bonds.reserve( nbonds );
        
        // parse 'atom block'
        for( int i = 0; i < natoms; i++ ) {
            is.getline(line, line_len); assert( !is.eof() );
            
            char nt[11];
            nt[10] = 0;
            
            std::copy( line, line + 10, nt );
            const float x = atof( nt );
            
            std::copy( line + 10, line + 10 + 10, nt );
            const float y = atof( nt );
            
            std::copy( line + 20, line + 20 + 10, nt );
            const float z = atof( nt );
            
            std::copy( line + 30, line + 30 + 3, nt );
            nt[3] = 0;

            // find first non-space character, which is the element symbol
//             char *ele = nt;
//             while( *ele != 0 && isspace(*ele)) {
//                 ele++;
//             }
//                
//             char *ele_end = ele;
//             
//             while( *ele_end != 0 && !isspace(*ele_end)) {
//                 ele_end++;
//             }
//             assert( ele_end - ele == 1 || ele_end - ele == 2 );
            
//             char *ele = line + 30 + 3;
            
           //  printf( "ele: '%c' '%c' '%c'\n", nt[0], nt[1], nt[2] );
            
            if( !m_allow_hydrogen ) {
             
                int nns = 0;
                bool is_h = false;
                
                for( int i = 0; i < 3; i++ ) {
                    if( !isspace(nt[i]) ) {
                        nns++;
                    }
                    if( nt[i] == 'h' || nt[i] == 'H' ) {
                        is_h = true;
                    }
                }
                if( nns == 1 && is_h ) {
                    throw std::runtime_error( "molecule in sdf contains hydrogen, while allow_hydrogen is not set" );   
                }
                
            }
            
            
            mol.m_atoms.push_back(atom(x,y,z,nt,canonicalize_element(nt)));
        }
        
        // parse 'bond block'
        for( int i = 0; i < nbonds; i++ ) {
            is.getline(line, line_len); assert( !is.eof() );
            char nt[4];
            nt[3] = 0;
            std::copy( line, line + 3, nt );
            int first = atoi(nt);
            
            std::copy( line + 3, line + 3 + 3, nt );
            int second = atoi(nt);
            
            std::copy( line + 6, line + 6 + 3, nt );
            int type = atoi(nt);
            
            mol.m_bonds.push_back(bond(first,second,type, canonicalize_bond_type(type)));
        }
        
        // just ignore the rest for now:
        // consume input up to and including the next occurence of '$$$$' + newline
        {
            // there seems to be some weird stuff in the 'associated data' section of some sdf files, which confuses the ifstream::getline ...
            char c;
            while(!is.eof()) {
                // (1) find the next '$'
                do {
                    is.get(c); 
                } while( c != '$' && !is.eof() );
                                
                
                // (2) match '$$$' or go back to (1)
                is.get(c); if( c != '$' ) {continue;}
                is.get(c); if( c != '$' ) {continue;}
                is.get(c); if( c != '$' ) {continue;}
                // it should be save to use getline to skip the newline now (TODO: is ifstream::getline actually windows line end safe?)
                is.getline( line, line_len );
//                 printf( "line: '%s'\n", line );
                
                break;
            }
        }
//         while( memcmp( line, "$$$$", 4 ) != 0 && !is.eof() ) {
//             is.getline(line, line_len);
//         }
        //printf( "mol: %zd %zd\n", mol.m_atoms.size(), mol.m_bonds.size() );
        return true;
        
    }
    
public:
    sdf( std::ifstream &is, bool allow_hydrogen = false ) : m_allow_hydrogen(allow_hydrogen) {
        while( parse_molecule( m_molecules, is ) ) {
           // printf( "mol: %zd\n", m_molecules.size() );
        }
//         getchar();
    }
    

    const std::vector<molecule> &get_molecules() {
     
        return m_molecules;
    }
    
    std::vector<const molecule *> get_molecule_ptrs() {
        std::vector<const molecule *> molptr;//( m_molecules.size() );
        
        for( std::vector< molecule >::const_iterator it = m_molecules.begin(); it != m_molecules.end(); ++it ) {
//             printf( "put: %p\n", &(*it));
            if( it->m_header != "20637" && it->m_header != "20715" ) {
                molptr.push_back(&(*it));
            }
        }
        
        return molptr;
    }
    
    int num_can_atom_types() {
        return m_atomtype_map.size();
    }
};
} // namespace ivy_mike
#endif

