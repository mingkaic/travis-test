///
/// fmts.hpp
/// fmts
///
/// Purpose:
/// Define string formatting for various types
///

#ifndef PKG_FMTS_HPP
#define PKG_FMTS_HPP

#include <algorithm>
#include <sstream>
#include <memory>

#include "types/strs.hpp"

#include "fmts/istringable.hpp"

namespace fmts
{

/// Symbol for the start of an array as string
const char arr_begin = '[';

/// Symbol for the end of an array as string
const char arr_end = ']';

/// Symbol for the delimiter between elements of an array as string
const char arr_delim = '\\';

const char pair_delim = ':';

/// Wrap std::string so that array symbols are prefixed with escaped symbol
struct String final
{
	String (const char* cstr) : val_(cstr) {}

	String (const std::string& sstr) : val_(sstr) {}

	/// Return string representation by breaking array/pair symbols
	operator std::string()
	{
		std::string modified = val_;
		for (size_t i = 0, n = modified.size(); i < n; ++i)
		{
			switch (modified[i])
			{
				case arr_begin:
				case arr_end:
				case arr_delim:
				case pair_delim:
					modified.insert(modified.begin() + i, arr_delim);
					++i;
					++n;
			}
		}
		return modified;
	}

	/// Raw string containing unbroken array/pair symbols
	std::string val_;
};

/// Override fmts::String stream into out stream
std::ostream& operator << (std::ostream& os, String sstr);

/// Override fmts::iStringable stream into outstream
std::ostream& operator << (std::ostream& os, const iStringable* sstr);

std::ostream& operator << (std::ostream& os, std::shared_ptr<iStringable> sstr);

/// Stream C-style strings to s
void to_stream (std::ostream& s, const char* str);

/// Stream byte-size integers and display as numbers to s
void to_stream (std::ostream& s, int8_t c);

/// Stream byte-size unsigned integers and display as numbers to s
void to_stream (std::ostream& s, uint8_t c);

/// Stream generic value to s
template <typename T, typename std::enable_if<!std::is_array<T>::value>::type* = nullptr>
void to_stream (std::ostream& s, T val)
{
	s << val;
}

/// Stream pair p to s given specified delim between first and second elements
template <typename PLEFT, typename PRIGHT>
void pair_to_stream (std::ostream& s, std::pair<PLEFT,PRIGHT> p,
	std::string delim = std::string(1, pair_delim))
{
	s << p.first << delim << p.second;
}

/// Stream pair using default delim
template <typename PLEFT, typename PRIGHT>
void to_stream (std::ostream& s, std::pair<PLEFT,PRIGHT> p)
{
	pair_to_stream(s, p);
}

/// Stream values between iterators as an array delimited by delim input
template <typename Iterator>
void arr_to_stream (std::ostream& s, Iterator begin, Iterator end,
	std::string delim = std::string(1, arr_delim))
{
	if (begin != end)
	{
		to_stream(s, *(begin++));
		while (begin != end)
		{
			s << delim;
			to_stream(s, *(begin++));
		}
	}
}

/// Stream values between iterators as an array
template <typename Iterator>
void to_stream (std::ostream& s, Iterator begin, Iterator end)
{
	s << arr_begin;
	arr_to_stream(s, begin, end);
	s << arr_end;
}

/// Stream generic value to s applied to array types
template <typename T, typename std::enable_if<std::is_array<T>::value>::type* = nullptr>
void to_stream (std::ostream& s, T val)
{
	to_stream(s, std::begin(val), std::end(val));
}

/// Return string representation for common arguments
template <typename T>
std::string to_string (T arg)
{
	std::stringstream ss;
	to_stream(ss, arg);
	return ss.str();
}

/// Return string representation of values between iterators
template <typename Iterator>
std::string to_string (Iterator begin, Iterator end)
{
	std::stringstream ss;
	to_stream(ss, begin, end);
	return ss.str();
}

/// Stream generic value to s applied to array types
template <typename Iterator>
std::string join (std::string delim, Iterator begin, Iterator end)
{
	std::stringstream ss;
	arr_to_stream(ss, begin, end, delim);
	return ss.str();
}

/// Return std::string with snprintf formatting
template <typename... ARGS>
std::string sprintf (std::string format, ARGS... args)
{
	size_t n = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
	char buf[n];
	std::snprintf(buf, n, format.c_str(), args...);
	return std::string(buf, buf + n - 1);
}

/// Trim all white-space symbols on the left side of string s
void ltrim(std::string& s);

/// Trim all white-space symbols on the right side of string s
void rtrim(std::string& s);

/// Trim all white-space symbols surrounding string s
void trim(std::string& s);

void lstrip (std::string& s, const std::unordered_set<char>& cset);

void rstrip (std::string& s, const std::unordered_set<char>& cset);

void strip (std::string& s, const std::unordered_set<char>& cset);

/// Return string s split into all substrings separated by delim as a vector
types::StringsT split (std::string s, std::string delim);

}

#endif // PKG_FMTS_HPP
