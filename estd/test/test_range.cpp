
#ifndef DISABLE_ESTD_RANGE_TEST

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/range.hpp"


TEST(RANGE, Creation)
{
	estd::NumRange<double> def;
	estd::NumRange<size_t> proper(2, 54);
	estd::NumRange<float> backwards(54, 14);

	EXPECT_EQ(0, def.lower_);
	EXPECT_EQ(0, def.upper_);

	EXPECT_EQ(2, proper.lower_);
	EXPECT_EQ(54, proper.upper_);

	EXPECT_EQ(14, backwards.lower_);
	EXPECT_EQ(54, backwards.upper_);
}


TEST(RANGE, Contains)
{
	estd::NumRange<double> def;
	estd::NumRange<size_t> range(54, 14);

	EXPECT_FALSE(def.contains(-1));
	EXPECT_FALSE(def.contains(-0.5));
	EXPECT_TRUE(def.contains(0));
	EXPECT_FALSE(def.contains(0.5));
	EXPECT_FALSE(def.contains(1));

	EXPECT_FALSE(range.contains(13));
	for (size_t i = 14; i < 54; ++i)
	{
		EXPECT_TRUE(range.contains(i));
	}
	EXPECT_FALSE(range.contains(55));
}


#endif // DISABLE_ESTD_RANGE_TEST
