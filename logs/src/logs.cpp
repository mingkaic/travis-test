#include "logs/logs.hpp"

#ifdef PKG_LOGS_HPP

namespace logs
{

static std::shared_ptr<iLogger> glogger = std::make_shared<DefLogger>();

std::string name_log (const LOG_LEVEL& level)
{
	if (lognames.size() <= level)
	{
		return "";
	}
	return lognames.at(level);
}

LOG_LEVEL enum_log (const std::string& level)
{
	auto it = names2log.find(level);
	if (names2log.end() == it)
	{
		return NOT_SET;
	}
	return names2log.at(level);
}

void set_logger (std::shared_ptr<iLogger> logger)
{
	glogger = logger;
}

iLogger& get_logger (void)
{
	return *glogger;
}

std::string get_log_level (void)
{
	return get_logger().get_log_level();
}

void set_log_level (const std::string& log_level)
{
	get_logger().set_log_level(log_level);
}

void trace (const std::string& msg)
{
	get_logger().log(TRACE, msg);
}

void debug (const std::string& msg)
{
	get_logger().log(DEBUG, msg);
}

void info (const std::string& msg)
{
	get_logger().log(INFO, msg);
}

void warn (const std::string& msg)
{
	get_logger().log(WARN, msg);
}

void error (const std::string& msg)
{
	get_logger().log(ERROR, msg);
}

void fatal (const std::string& msg)
{
	get_logger().log(FATAL, msg);
}

}

#endif
