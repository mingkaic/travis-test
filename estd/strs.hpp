
#ifndef PKG_ESTD_STR_HPP
#define PKG_ESTD_STR_HPP

#include <string>

namespace estd
{

bool has_prefix (const std::string& str, const std::string& prefix);

bool has_affix (const std::string& str, const std::string& affix);

}

#endif // PKG_ESTD_STR_HPP
