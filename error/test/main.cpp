#include "gtest/gtest.h"

#include "error/error.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_ERROR_TEST


TEST(ERROR, ErrMessage)
{
	auto err = error::error("bad zzzz 23");
	ASSERT_NE(nullptr, err);
	EXPECT_STREQ("bad zzzz 23", err->to_string().c_str());

	auto err2 = error::errorf("bad %s %d", "zzzz", 23);
	ASSERT_NE(nullptr, err2);
	EXPECT_STREQ("bad zzzz 23", err2->to_string().c_str());
}


#endif // DISABLE_ERROR_TEST
