#include "ivymike/demangle.h"
#include "ivymike/stupid_ptr.h"


#ifndef WIN32
#include <cxxabi.h>
#endif

namespace ivy_mike {


    
#ifndef WIN32


std::string demangle( const char *tname ) {
    int status;
    char *realname = abi::__cxa_demangle(tname, 0, 0, &status);
    freeer f(realname);
    
    return std::string( realname );
}
#else

#endif
}