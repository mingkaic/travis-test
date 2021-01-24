
#ifndef PKG_ESTD_ALGORITHM_HPP
#define PKG_ESTD_ALGORITHM_HPP

#include <algorithm>

namespace estd
{

template <typename VEC, typename UNARY>
void remove_if (VEC& vec, UNARY pred)
{
	auto pend = std::remove_if(vec.begin(), vec.end(), pred);
	vec.erase(pend, vec.end());
}

}

#endif // PKG_ESTD_ALGORITHM_HPP
