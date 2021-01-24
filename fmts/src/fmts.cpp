#include "fmts/fmts.hpp"

#ifdef PKG_FMTS_HPP

namespace fmts
{

std::ostream& operator << (std::ostream& os, String sstr)
{
	os << (std::string) sstr;
	return os;
}

std::ostream& operator << (std::ostream& os, const iStringable* sstr)
{
	os << sstr->to_string();
	return os;
}

std::ostream& operator << (std::ostream& os, std::shared_ptr<iStringable> sstr)
{
	os << sstr->to_string();
	return os;
}

void to_stream (std::ostream& s, const char* str)
{
	s << std::string(str);
}

void to_stream (std::ostream& s, int8_t c)
{
	s << (int) c;
}

void to_stream (std::ostream& s, uint8_t c)
{
	s << (unsigned) c;
}

void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		[](char c) { return !std::isspace(c); }));
}

void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[](char c) { return !std::isspace(c); }).base(), s.end());
}

void trim(std::string& s)
{
	ltrim(s);
	rtrim(s);
}

void lstrip (std::string& s, const std::unordered_set<char>& cset)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		[&](char c) { return cset.end() == cset.find(c); }));
}

void rstrip (std::string& s, const std::unordered_set<char>& cset)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[&](char c) { return cset.end() == cset.find(c); }).base(), s.end());
}

void strip (std::string& s, const std::unordered_set<char>& cset)
{
	lstrip(s, cset);
	rstrip(s, cset);
}

types::StringsT split (std::string s, std::string delim)
{
	types::StringsT out;

	size_t i = 0;
	while ((i = s.find(delim, 0)) != std::string::npos)
	{
		out.push_back(s.substr(0, i));
		s = s.substr(i + delim.size());
	}
	out.push_back(s);

	return out;
}

}

#endif
