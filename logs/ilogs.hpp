
#ifndef PKG_LOGS_ILOGS_HPP
#define PKG_LOGS_ILOGS_HPP

#include <string>

namespace std
{

struct source_location
{
	static source_location current (void) { return source_location(); }

	const char* file_name (void) const noexcept { return ""; }

	std::uint32_t line (void) const noexcept { return 0; }

	const char* function_name (void) const noexcept { return ""; }
};

}

namespace logs
{

using SrcLocT = std::source_location;

/// Interface of logger
struct iLogger
{
	virtual ~iLogger (void) = default;

	/// Get log level encoding specifying verbosity
	virtual std::string get_log_level (void) const = 0;

	/// Set log level encoding specifying verbosity
	virtual void set_log_level (const std::string& log_level) = 0;

	/// Return true if string level is supported
	virtual bool supports_level (size_t msg_level) const = 0;

	/// Return true if string level is supported
	virtual bool supports_level (const std::string& msg_level) const = 0;

	/// Log message at any specified enum level of verbosity
	virtual void log (size_t msg_level, const std::string& msg,
		const SrcLocT& location = SrcLocT::current()) = 0;

	/// Log message at any specified string level of verbosity
	virtual void log (const std::string& msg_level, const std::string& msg,
		const SrcLocT& location = SrcLocT::current()) = 0;
};

}

#endif // PKG_LOGS_ILOGS_HPP
