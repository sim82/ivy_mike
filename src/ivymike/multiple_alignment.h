#ifndef __multiple_alignment_h
#define __multiple_alignment_h

#include <string>
#include <vector>
#include <iosfwd>
#include <stdint.h>

namespace ivy_mike {
	
struct multiple_alignment {
	std::vector <std::string > names;
	std::vector <std::vector<uint8_t> > data;
	
	bool load_phylip( std::istream &is );
	bool load_phylip( const char *name );
	
};
	


}
#endif