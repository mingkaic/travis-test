///
/// range.hpp
/// estd
///
/// Purpose:
/// Define range utility structure
///

#ifndef PKG_ESTD_RANGE_HPP
#define PKG_ESTD_RANGE_HPP

#include <algorithm>
#include <type_traits>

namespace estd
{

/// Order two values such that higher value goes to upper_ and
/// lower value goes to lower_
template <typename T, typename = typename std::enable_if<
	std::is_arithmetic<T>::value, T>::type>
struct NumRange final
{
	NumRange (void) : lower_(0), upper_(0) {}

	NumRange (T bound1, T bound2) :
		lower_(std::min(bound1, bound2)),
		upper_(std::max(bound1, bound2)) {}

	/// Return true if val is in range [lower, upper] inclusively
	bool contains (T val) const
	{
		return lower_ <= val && val <= upper_;
	}

	/// Lower value of the range
	T lower_;

	/// Higher value of the range
	T upper_;
};

}

#endif // PKG_ESTD_RANGE_HPP
