#include "estd/strs.hpp"

#ifdef PKG_ESTD_STR_HPP

namespace estd
{

bool has_prefix (const std::string& str, const std::string& prefix)
{
	size_t n = str.size();
	size_t nprefix = prefix.size();
	return std::equal(str.begin(),
		str.begin() + std::min(n, nprefix), prefix.begin());
}

bool has_affix (const std::string& str, const std::string& affix)
{
	size_t n = str.size();
	size_t naffix = affix.size();
	if (n < naffix)
	{
		return false;
	}
	return std::equal(str.begin() + n - naffix,
		str.end(), affix.begin());
}

}

#endif
