#include <memory>

#include "gtest/gtest.h"

#include "exam/exam.hpp"

int main (int argc, char** argv)
{
	auto tlogger = std::make_shared<exam::MockLogger>();
	logs::set_logger(std::static_pointer_cast<logs::iLogger>(tlogger));

	::testing::InitGoogleTest(&argc, argv);
	auto ret = RUN_ALL_TESTS();
	logs::set_logger(nullptr);
	return ret;
}
