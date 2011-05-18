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
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <typeinfo>
#include <cctype>
#include <algorithm>
#include "ivymike/getopt.h"

// OK, the parser is a bit overengineered for cmdline parsing. it's turned out to be some kind of backtracking recursive descent parser...
// maybe I can use the bits for other parsers in the future...

namespace ivy_mike {
namespace getopt {
    
template <typename pinput_t, int X = 0>
class pisaver {
    pinput_t m_pis;
    pinput_t &m_pi_ref;
    bool     m_commit;

public:
    pisaver( pinput_t &pi ) : m_pis(pi), m_pi_ref(pi), m_commit(false) {}
    ~pisaver() {
        if ( !m_commit ) {
            if( false ) {
                std::cout << "pis rollback " << X << " at " << (m_pi_ref.end() - m_pi_ref.begin());
                
                if ( m_pi_ref.has_next() ) {
                    std::cout << " at char: " << m_pi_ref.peek() << "\n";
                } else {
                    std::cout << " at end\n";
                }
            }
            
            // do the rollback
            m_pi_ref = m_pis;

        }
    }

    void commit() {
        m_commit = true;
    }
};
    
parser::auto_token parser::parse_argument(ivy_mike::getopt::parser::my_pinput& pi) {
    pisaver<my_pinput> pis(pi);


    pi.skip_whitespace();

    if ( pi.peek() == '-' ) {
        pi.next();

        unsigned char optid = pi.peek();

        if ( std::isalnum(optid) ) {
            pi.next();
            pis.commit();
            return auto_token(new option( optid ));
        }

    }


    return std::auto_ptr<token>();

}

parser::auto_token parser::parse_bare_string(parser::my_pinput& pi) {
    pisaver<my_pinput,1> pis(pi);

    pi.skip_whitespace();


    if ( !pi.has_next()) {

        return auto_token();
    }



    std::string str;

    while ( pi.has_next() ) {

        if ( std::isprint( pi.peek() ) && !std::isspace(pi.peek()) && pi.peek() != '"' ) {
            str.push_back(pi.peek());

            pi.next();
        } else {
            pi.next();
            break;
        }

    }

    if ( str.empty() ) {
        return auto_token();
    }


    auto_token s(new string( str ));
    pis.commit();

    return s;

}
parser::auto_token parser::parse_quoted_string(parser::my_pinput& pi) {
    pisaver<my_pinput,2> pis(pi);

    pi.skip_whitespace();


    if ( !pi.has_next()) {
        return auto_token();
    }


    if ( !pi.peek() == '"' ) {
        return auto_token();
    }
    pi.next();

    std::string str;
    bool closed = false;
    while ( pi.has_next() ) {

        if ( pi.peek() == '"' ) {
            closed = true;
            pi.next();
            break;
        } else {
            str.push_back(pi.peek());
            pi.next();
        }

    }

    if ( str.empty() || !closed) {
        return auto_token();
    }


    auto_token s(new string( str ));
    pis.commit();

    return s;

}
parser::auto_token parser::parse_string(parser::my_pinput& pi) {
    // the two string parsers do the pi state reconstruction
    auto_token t(parse_bare_string(pi));

    if ( t.get() == 0 ) {
        t = parse_quoted_string(pi);
    }

    return t;

}
parser::auto_token parser::parse(parser::my_pinput& pi) {
    if ( !pi.has_next() ) {
        return auto_token();
    }

    auto_token t = parse_argument( pi );

    if ( t.get() != 0 ) {
        return t;
    }

    t = parse_string( pi );

    if ( t.get() != 0 ) {
        return t;
    }

    return auto_token();

}
void parser::parse_main(parser::my_pinput& pi) {
    
    while ( true ) {
        auto_token t = parse(pi);
        
        if( t.get() == 0 ) {
            break;
        }
        
//         std::cout << "add token: " << t->to_string() << "\n";
        m_tokenstream.push_back( t.get() );
        t.release();
    }

    if ( pi.has_next() ) {
        std::cerr << "WARNING: parser not finished\n";
    }

}
void parser::parse(int argc, char** argv) {
    std::stringstream ss;

    for ( int i = 1; i < argc; i++ ) {
        if ( i > 1 ) {
            ss << " ";
        }


        if ( std::count( argv[i], argv[i] + strlen(argv[i]), ' ' ) != 0 ) {
//             std::cout << "quoting\n";
            ss << "\"" << argv[i] << "\"";
        } else {
            ss << argv[i];
        }

    }
    std::string s = ss.str();

//     std::cout << "cmdline: '" << s << "'\n" << std::endl;
    my_pinput pi( s.begin(), s.end() );

    parse_main( pi );

    for ( std::list< token* >::iterator it = m_tokenstream.begin(); it != m_tokenstream.end(); ++it ) {
        token *t = *it;

        if ( typeid(*t) == typeid( option ) ) {
            option *opt = static_cast<option *>(t);

//             std::cout << "option\n";

            if ( m_options[opt->get_opt()] ) {
                m_option_count[opt->get_opt()]++;

                if ( m_opt_has_argument[opt->get_opt()] ) {
                    ++it;
                    if ( it == m_tokenstream.end() ) {
                        std::cout << "missing option argument (end of input)\n";
                        break;
                    }

                    t = *it;

                    if ( typeid( *t ) != typeid(string)) {
                        std::cout << "missing option argument\n";
                        break;
                    }

                    m_opt_strings[opt->get_opt()] = static_cast<string*>(t)->get_string();
                    
                    if( m_opt_values[opt->get_opt()] != 0 ) {
                        m_opt_values[opt->get_opt()]->set(m_opt_strings[opt->get_opt()]);
                    }
                }
            }

        } else {
            std::cout << "unexpected token: " << typeid(*t).name() << " " << t->to_string() << "\n";
            break;
        }
    }


}
void parser::add_opt(unsigned char c, bool argument) {
    m_options[c] = true;
    if ( argument ) {
        m_opt_has_argument[c] = true;
    }
}

void parser::add_opt_proxy( unsigned char c, std::auto_ptr<base_value> bv ) {
    if( m_opt_values[c] != 0 ) {
        std::stringstream ss;
        ss << "there is already a value proxy registered for option " << c;
        throw std::runtime_error( ss.str() );
    }
    m_options[c] = true;
    m_opt_has_argument[c] = true;
    m_opt_values[c] = bv.get(); // hmm, is vector::operator[] officially allowed to throw exceptions? using sequencial get+release should make this code exception safe anyway!?
    bv.release();
}

parser::~parser() {
    std::for_each( m_tokenstream.begin(), m_tokenstream.end(), delete_object() );
    std::for_each( m_opt_values.begin(), m_opt_values.end(), delete_object() );
}

} // namespace getopt
} // namespace ivy_mike

