
#ifndef PKG_TYPES_STRS_HPP
#define PKG_TYPES_STRS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace types
{

using StringsT = std::vector<std::string>;

using StrSetT = std::set<std::string>;

using StrUSetT = std::unordered_set<std::string>;

template <typename VAL>
using StrMapT = std::map<std::string,VAL>;

template <typename VAL>
using StrUMapT = std::unordered_map<std::string,VAL>;

}

#endif // PKG_TYPES_STRS_HPP
