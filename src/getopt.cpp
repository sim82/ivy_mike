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
#include "ivymike/getopt.h"

namespace ivy_mike {
namespace getopt {
ivy_mike::getopt::parser::token* parser::parse_argument(ivy_mike::getopt::parser::my_pinput& pi) {
    pisaver<my_pinput> pis(pi);


    pi.skip_whitespace();

    if ( pi.peek() == '-' ) {
        pi.next();

        unsigned char optid = pi.peek();

        if ( std::isalnum(optid) ) {
            pi.next();
            pis.commit();
            return new option( optid );
        }

    }


    return 0;

}
parser::token* parser::parse_bare_string(parser::my_pinput& pi) {
    pisaver<my_pinput,1> pis(pi);

    pi.skip_whitespace();


    if ( !pi.has_next()) {

        return 0;
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
        return 0;
    }


    string *s = new string( str );
    pis.commit();

    return s;

}
parser::token* parser::parse_quoted_string(parser::my_pinput& pi) {
    pisaver<my_pinput,2> pis(pi);

    pi.skip_whitespace();


    if ( !pi.has_next()) {
        return 0;
    }


    if ( !pi.peek() == '"' ) {
        return 0;
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
        return 0;
    }


    string *s = new string( str );
    pis.commit();

    return s;

}
parser::token* parser::parse_string(parser::my_pinput& pi) {
    // the two string parser do the pi state reconstruction
    token *t = parse_bare_string(pi);

    if ( t == 0 ) {
        t = parse_quoted_string(pi);
    }

    return t;

}
parser::token* parser::parse(parser::my_pinput& pi) {
    token *t;

    if ( !pi.has_next() ) {
        return 0;
    }

    t = parse_argument( pi );

    if ( t != 0 ) {
        return t;
//             m_tokenstream.push_back( t );
    }

    t = parse_string( pi );

    if ( t != 0 ) {
        return t;
//             m_tokenstream.push_back( t );
    }

    return 0;

}
void parser::parse_main(parser::my_pinput& pi) {

    token *t = 0;
    while ( (t = parse(pi)) != 0 ) {
        m_tokenstream.push_back( t );
        std::cout << "add token: " << ptrdiff_t(t) << std::endl;
        std::cout << "add token: " << t->to_string() << "\n";
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
            std::cout << "quoting\n";
            ss << "\"" << argv[i] << "\"";
        } else {
            ss << argv[i];
        }

    }
    std::string s = ss.str();

    std::cout << "cmdline: '" << s << "'\n" << std::endl;
    my_pinput pi( s.begin(), s.end() );

    parse_main( pi );

    for ( std::list< token* >::iterator it = m_tokenstream.begin(); it != m_tokenstream.end(); ++it ) {
        token *t = *it;


        if ( typeid(*t) == typeid( option ) ) {
            option *opt = static_cast<option *>(t);

            std::cout << "option\n";

            if ( m_options[opt->get_opt()] ) {
                m_option_count[opt->get_opt()]++;

                if ( m_opt_type[opt->get_opt()] ) {
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
        m_opt_type[c] = true;
    }
}
parser::~parser() {
    std::for_each( m_tokenstream.begin(), m_tokenstream.end(), delete_object() );
}

} // namespace getopt
} // namespace ivy_mike

