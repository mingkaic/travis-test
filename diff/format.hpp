///
/// format.hpp
/// diff
///
/// Purpose:
/// Define diff structure (defined in myers_diff.hpp) string representation
///

#ifndef PKG_DIFF_FORMAT_HPP
#define PKG_DIFF_FORMAT_HPP

#include <ostream>

#include "diff/myers_diff.hpp"

namespace diff
{

/// Symbol for added lines
const char add_token = '+';

/// Symbol for removed lines
const char del_token = '-';

/// Format specified difference lines and stream to out
template <typename T>
void diff_line_format (std::ostream& out, T val,
	Action action, int64_t orig, int64_t updated)
{
	switch (action)
	{
		case EQ:
			out << "  " << orig << "\t" << updated;
			break;
		case ADD:
			out << add_token << "  \t" << updated;
			break;
		case DEL:
			out << del_token << " " << orig << "\t ";
			break;
	}
	out << "\t" << val << "\n";
}

}

#endif // PKG_DIFF_FORMAT_HPP
