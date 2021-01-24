#ifndef FMTS_ISTRINGABLE_HPP
#define FMTS_ISTRINGABLE_HPP

#include <string>

namespace fmts
{

struct iStringable
{
	virtual ~iStringable (void) = default;

	// Return the string representation of this object
	virtual std::string to_string (void) const = 0;
};

}

#endif // FMTS_ISTRINGABLE_HPP
