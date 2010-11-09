#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/typeof/typeof.hpp>
#include <iostream>
#include <string>
#include <map>
#include <stdio.h>
#define BOOST_SPIRIT_AUTO(domain_, name, expr)                                  \
    typedef boost::proto::result_of::                                           \
        deep_copy<BOOST_TYPEOF(expr)>::type name##_expr_type;                   \
    BOOST_SPIRIT_ASSERT_MATCH(                                                  \
        boost::spirit::domain_::domain, name##_expr_type);                      \
    BOOST_AUTO(name, boost::proto::deep_copy(expr));                            \


namespace qi = boost::spirit::qi;
 
template <typename Iterator>
struct keys_and_values
  : qi::grammar<Iterator, std::map<std::string, std::string>()>
{
    keys_and_values()
      : keys_and_values::base_type(query)
    {
        query =  pair >> *((qi::lit(';') | '&') >> pair);
        pair  =  key >> -('=' >> value);
        key   =  qi::char_("a-zA-Z_") >> *qi::char_("a-zA-Z_0-9");
        value = +qi::char_("a-zA-Z_0-9");
    }
    qi::rule<Iterator, std::map<std::string, std::string>()> query;
    qi::rule<Iterator, std::pair<std::string, std::string>()> pair;
    qi::rule<Iterator, std::string()> key, value;
};




int main() {
    using boost::spirit::ascii::space;
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::parse;
    typedef std::string::const_iterator iterator_type;
    
    BOOST_SPIRIT_AUTO(qi, comment, *(space) >> "/*" >> *(char_ - "*/") >> "*/");

    std::string str = "  /*This is a comment**/";
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool r = parse(iter, end, comment);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
    }
    else
    {
        std::string rest(iter, end);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \": " << rest << "\"\n";
        std::cout << "-------------------------\n";
    }


    {
        std::string input("key1=value1;key2;key3=value3");  // input to parse
        std::string::iterator begin = input.begin();
        std::string::iterator end = input.end();
         
        keys_and_values<std::string::iterator> p;    // create instance of parser
        std::map<std::string, std::string> m;        // map to receive results
        bool result = qi::parse(begin, end, p, m);   // returns true if successful
        printf( "res: %s\n", result ? "true" : "false" );
    }
    return 0;
}

