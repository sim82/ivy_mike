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



#ifndef __ivy_mike__getopt_h
#define __ivy_mike__getopt_h

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>

namespace ivy_mike {
    
    
struct delete_object {
  
    template<typename T>
    void operator()(const T* ptr ) {
        delete( ptr );
    }
};
    
namespace getopt {


    
template <typename iter_t>    
class pinput {
    typedef typename iter_t::value_type value_type;
    
    iter_t m_begin;
    iter_t m_end;
    
    
    pinput();
    
public:
    pinput( const iter_t &beg, const iter_t &end ) : m_begin(beg), m_end(end) {}
    
    void skip_whitespace() {
//         while( m_begin != m_end && std::isspace(*m_begin) ) { // FIXME: do iterators generally allow to dereference iter.end()?
//             ++m_begin;
//         }

        m_begin = std::find_if( m_begin, m_end, std::not1(std::ptr_fun<int,int>(std::isspace)) );
    }
    
    
    value_type peek() {
        if( m_begin >= m_end ) {
            throw std::runtime_error( "m_begin >= m_end" );
        }
        
        return *m_begin;
    }
    
    value_type next() {
        if( m_begin >= m_end ) {
            throw std::runtime_error( "m_begin >= m_end" );
        }
        
        return *(m_begin++);
    }
    
    bool has_next() {
        return m_begin < m_end;
    }
    
    const iter_t &begin() { return m_begin; }
    const iter_t &end() { return m_end; }
};
    

template <typename pinput_t, int X = 0>
class pisaver {
    pinput_t m_pis;
    pinput_t &m_pi_ref;
    bool     m_commit;
    
public:
    pisaver( pinput_t &pi ) : m_pis(pi), m_pi_ref(pi), m_commit(false) {}
    ~pisaver() {
        if( !m_commit ) {
            std::cout << "pis rollback " << X << " at " << (m_pi_ref.end() - m_pi_ref.begin());
            
            if( m_pi_ref.has_next() ) {
                std::cout << " at char: " << m_pi_ref.peek() << "\n";
            } else {
                std::cout << " at end\n";
            }
            
            m_pi_ref = m_pis;
            
        }
    }
    
    void commit() {
        m_commit = true;
    }
    
};

template <typename T>
static T literal_cast( const std::string &str );

template <>
inline int literal_cast( const std::string &str ) {
    return atoi( str.c_str() );
}

template <>
inline std::string literal_cast( const std::string &str ) {
    return str.c_str();
}


class value {
public:
    virtual ~value() {}
    virtual void operator()( const std::string &str ) = 0;
};

class value_string : public value {
    std::string &m_string_ref;
public:
    
    value_string( std::string &str ) : m_string_ref(str) {}
    
    virtual void operator()(const std::string &str ) {
        m_string_ref = str;
    }
    
};

class value_int : public value {
    int &m_int_ref;
public:
    
    value_int( int &i ) : m_int_ref(i) {}
    
    virtual void operator()(const std::string &str ) {
        m_int_ref = atoi( str.c_str() );
    }
    
};



class parser {
    typedef pinput<std::string::iterator> my_pinput;
    
    std::vector<bool> m_options;
    
    std::vector<bool> m_opt_type;
    
    std::vector<int> m_option_count;
    std::map <char,std::string> m_opt_strings;
    
    std::vector<value *> m_opt_values;
    
    parser( const parser & );
    parser &operator=(const parser &);
    
    class token {
    public:
        virtual ~token() {}
        
        virtual std::string to_string() = 0;
    };
    
    class option : public token {
        const unsigned char m_option;
    public:
        option( unsigned char opt ) : m_option(opt) {}
        
        virtual ~option() {}
        
        virtual std::string to_string() {
            std::stringstream ss;
            
            ss << "option: " << m_option;
            
            return ss.str();
        }
        
        unsigned char get_opt() {
            return m_option;
        }
        
    };
    
    class string : public token {
        std::string m_string;
        
    public:
        
        string( std::string &s ) : m_string(s) {}
        
        virtual ~string() {}
        
        virtual std::string to_string() {
            std::stringstream ss;
            
            ss << "string: " << m_string;
            
            return ss.str();
        }
        
        const std::string &get_string() {
            return m_string;
        }
    };
    
    
    std::list <token *> m_tokenstream;
    
public:
    parser() : m_options(256), m_opt_type(256),  m_option_count(256), m_opt_values(256) {}
    
    ~parser() ;
    
    
    void add_opt( unsigned char c, bool argument );
    
    template<typename T>
    void add_opt( unsigned char c, T &) {
        
    }
    
    token *parse_argument( my_pinput &pi ) ;
    
    token *parse_bare_string( my_pinput &pi ) ;
    token *parse_quoted_string( my_pinput &pi ) ;
    token *parse_string( my_pinput &pi ) ;
    token *parse( my_pinput &pi ) ;
    void parse_main( my_pinput &pi ) ;
    
    
    void parse( int argc, char **argv ) ;
 
    const std::string &get_string( char opt ) {
        //it = m_opt_strings.find( opt );
        
        return m_opt_strings[opt];
    }
    
    int get_int( char opt ) {
        return atoi( m_opt_strings[opt].c_str() );
    }
    
    void get_int_if_present( char opt, int &i ) {
        if( opt_count(opt) != 0 ) {
            i = get_int(opt);
        }
    }


    int opt_count( char opt ) {
        return m_option_count[opt];
    }

    
    
    
};

} // namespace getopt
} // namespace ivy_mike



#endif