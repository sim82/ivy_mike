#include "ivymike/demangle.h"
#include "ivymike/stupid_ptr.h"


#ifndef WIN32
#include <cxxabi.h>
#else
//#include <Windows.h>
//#include <DbgHelp.h>
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
std::string demangle( const char *tname ) {
#if 0
	// doing this acually requires linking an extra library with the
	// stupid name 'Dbghelp.dll'. It's not worth it...

	const DWORD len = 256;
	char name[len];
	
	UnDecorateSymbolName( tname, name, len, 0 );

	return std::string( name );
#else

	return std::string( tname );
#endif
}
#endif
}