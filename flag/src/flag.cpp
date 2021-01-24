#include <boost/filesystem.hpp>

#include "flag/flag.hpp"

#ifdef PKG_FLAG_HPP

namespace flag
{

namespace fs = boost::filesystem;

static std::string make_usage_string (const std::string& program_name,
	const opt::options_description& flag_desc,
	opt::positional_options_description& pos)
{
	std::stringstream usage;
	usage << "usage: ";
	usage << program_name << ' ';
	if(flag_desc.options().size() > 0)
	{
		usage << "[flags]";
	}
	size_t N = pos.max_total_count();
	if(N == std::numeric_limits<unsigned>::max())
	{
		std::string arg;
		std::string last = pos.name_for_position(
			std::numeric_limits<unsigned>::max()-1);
		for(size_t i = 0; arg != last; ++i)
		{
			arg = pos.name_for_position(i);
			usage << ' ' << arg;
		}
		usage << " [" << arg << "...]";
	}
	else
	{
		for(size_t i = 0; i < N; ++i)
		{
			usage << ' ' << pos.name_for_position(i);
		}
	}
	usage << '\n' << flag_desc;
	return usage.str();
}

FlagSet::FlagSet (const char* prog) :
	flags_(fmts::sprintf("%s flags", prog)),
	args_(fmts::sprintf("%s arguments", prog)) {}

bool FlagSet::parse(int argc, const char** argv, opt::variables_map& vars)
{
	flags_.add_options()
		("help", "Display help message");

	opt::options_description all_options;
	all_options.add(flags_);
	all_options.add(args_);

	try
	{
		opt::store(opt::command_line_parser(argc, argv).
			options(all_options).positional(pos_).run(), vars);

		if (vars.count("help"))
		{
			logs::warn(make_usage_string(boost::filesystem::path(
				argv[0]).stem().string(), flags_, pos_));
			return false;
		}

		opt::notify(vars);
	}
	catch (opt::error& e)
	{
		logs::error(e.what());
		return false;
	}

	return true;
}

}

#endif
