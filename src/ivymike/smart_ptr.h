
#ifndef __ivy_mike__smart_ptr_h
#define __ivy_mike__smart_ptr_h

//
// fear not, this is not yet another smart_ptr implementation, just a workaround until tr1 is standard...
//


#if WIN32
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/scoped_ptr.hpp>

namespace sptr = boost;
#else
#include <tr1/memory>
#include <memory>
namespace sptr = std::tr1;

#endif

#endif
