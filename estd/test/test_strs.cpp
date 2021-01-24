
#ifndef DISABLE_ESTD_STRS_TEST

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/strs.hpp"


TEST(STRS, HasPrefix)
{
	EXPECT_FALSE(estd::has_prefix("abcdef", "def"));
	EXPECT_FALSE(estd::has_prefix("abcdef", "ghidef"));
	EXPECT_FALSE(estd::has_prefix("abcdef", "ghijkl"));
	EXPECT_FALSE(estd::has_prefix("abcdef", "f"));
	EXPECT_FALSE(estd::has_prefix("abcdef", "abchij"));
	EXPECT_TRUE(estd::has_prefix("abcdef", "abcdef"));
	EXPECT_TRUE(estd::has_prefix("abcdef", "abc"));
	EXPECT_TRUE(estd::has_prefix("abcdef", "a"));
}


TEST(STRS, HasAffix)
{
	EXPECT_FALSE(estd::has_affix("abcdef", "abc"));
	EXPECT_FALSE(estd::has_affix("abcdef", "abcjkl"));
	EXPECT_FALSE(estd::has_affix("abcdef", "ghijkl"));
	EXPECT_FALSE(estd::has_affix("abcdef", "a"));
	EXPECT_FALSE(estd::has_affix("abcdef", "ghidef"));
	EXPECT_TRUE(estd::has_affix("abcdef", "abcdef"));
	EXPECT_TRUE(estd::has_affix("abcdef", "def"));
	EXPECT_TRUE(estd::has_affix("abcdef", "f"));
	EXPECT_FALSE(estd::has_affix("abcdef", "abcdefg"));
}


#endif // DISABLE_ESTD_STRS_TEST
