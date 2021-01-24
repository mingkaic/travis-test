
#ifndef PKG_EXAM_MOCK_LOG_HPP
#define PKG_EXAM_MOCK_LOG_HPP

#include "logs/logs.hpp"

#include "gmock/gmock.h"

namespace exam
{

struct MockLogger final : public logs::iLogger
{
	MOCK_CONST_METHOD0(get_log_level, std::string(void));

	MOCK_METHOD1(set_log_level, void(const std::string& log_level));

	MOCK_CONST_METHOD1(supports_level, bool(size_t));

	MOCK_CONST_METHOD1(supports_level, bool(const std::string&));

	MOCK_METHOD3(log, void(const std::string&,const std::string&,const logs::SrcLocT&));

	MOCK_METHOD3(log, void(size_t,const std::string&,const logs::SrcLocT&));
};

}

#endif // PKG_EXAM_MOCK_LOG_HPP
