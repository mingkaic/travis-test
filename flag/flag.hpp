///
/// flag.hpp
/// flag
///
/// Purpose:
/// Define flag option parsing with builtin help option
///

#ifndef PKG_FLAG_HPP
#define PKG_FLAG_HPP

#include <boost/program_options.hpp>

#include "logs/logs.hpp"

namespace flag
{

namespace opt = boost::program_options;

/// Set of flag description and variable maps
struct FlagSet
{
	FlagSet (const char* prog);

	/// Return true if arguments are successfully parsed according to args_
	/// --help flag will logs::warn with help message and return false
	/// Any error will logs::error and return false
	bool parse(int argc, const char** argv)
	{
		opt::variables_map vm;
		return parse(argc, argv, vm);
	}

	/// Return true if arguments are successfully parsed according to args_
	/// Dump parsed variables to vars
	bool parse(int argc, const char** argv, opt::variables_map& vars);

	/// Return boost's options description for flags
	/// See Boost Documentation for more information:
	/// https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
	opt::options_description_easy_init add_flags (void)
	{
		return flags_.add_options();
	}

	/// Add positional arguments using boost's options_description_easy_init's
	/// operator() (const char*, const opt::value_semantic*, const char*)
	/// signature, in addition to the option's position
	/// See Boost Documentation for more information:
	/// https://www.boost.org/doc/libs/1_58_0/doc/html/program_options.html
	void add_arg (const char* name, const opt::value_semantic* s,
		const char* description, int maxcount)
	{
		args_.add_options()(name, s, description);
		pos_.add(name, maxcount);
	}

private:
	/// Flag options
	opt::options_description flags_;

	/// Non-flag arguments
	opt::options_description args_;

	/// Position of non-flag arguments
	opt::positional_options_description pos_;
};

}

#endif // PKG_FLAG_HPP
