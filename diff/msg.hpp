///
/// msg.hpp
/// diff
///
/// Purpose:
/// Define diff message generators
///

#ifndef PKG_DIFF_MSG_HPP
#define PKG_DIFF_MSG_HPP

#include <cstring>
#include <istream>

#include "fmts/fmts.hpp"

#include "diff/format.hpp"

namespace diff
{

/// Number of lines to display before lines where differences occured
const uint8_t lines_before = 3;

/// Number of lines to display after lines where differences occured
const uint8_t lines_after = 3;

/// Return diff message of multiple lines
/// Message is empty if the lines are the same
/// Caveat: the product of size of the vectors is roughly limited to 2^32,
/// this function does not perform any optimization to diff long messages
std::string diff_msg (
	const types::StringsT& expected,
	const types::StringsT& got);

/// Same as diff_msg, except it diffs the message in batches
std::string safe_diff_msg (
	const types::StringsT& expected,
	const types::StringsT& got,
	size_t batch_limit = std::numeric_limits<IndexT>::max());

std::string diff_lines (
	std::istream& expect, std::istream& got,
	bool ignore_empty_lines = true,
	bool trim_spaces = true);

}

#endif // PKG_DIFF_MSG_HPP
