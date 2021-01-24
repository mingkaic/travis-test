
#ifndef DISABLE_ESTD_CONTAIN_TEST

#include <array>
#include <list>
#include <vector>

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/contain.hpp"


using ::testing::_;
using ::testing::Return;
using ::testing::Throw;


TEST(CONTAIN, Has)
{
	types::StrMapT<size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	types::StrSetT ssets = {
		"abcdef", "defghi", "ghijkl",
	};

	types::StrUMapT<size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::unordered_set<std::string> ussets = {
		"abcdef", "defghi", "ghijkl",
	};

	EXPECT_TRUE(estd::has(stuff, "abcdef"));
	EXPECT_TRUE(estd::has(stuff, "defghi"));
	EXPECT_TRUE(estd::has(stuff, "ghijkl"));
	EXPECT_FALSE(estd::has(stuff, "ghi"));
	EXPECT_FALSE(estd::has(stuff, "mno"));

	EXPECT_TRUE(estd::has(ssets, "abcdef"));
	EXPECT_TRUE(estd::has(ssets, "defghi"));
	EXPECT_TRUE(estd::has(ssets, "ghijkl"));
	EXPECT_FALSE(estd::has(ssets, "ghi"));
	EXPECT_FALSE(estd::has(ssets, "mno"));

	EXPECT_TRUE(estd::has(ustuff, "abcdef"));
	EXPECT_TRUE(estd::has(ustuff, "defghi"));
	EXPECT_TRUE(estd::has(ustuff, "ghijkl"));
	EXPECT_FALSE(estd::has(ustuff, "ghi"));
	EXPECT_FALSE(estd::has(ustuff, "mno"));

	EXPECT_TRUE(estd::has(ussets, "abcdef"));
	EXPECT_TRUE(estd::has(ussets, "defghi"));
	EXPECT_TRUE(estd::has(ussets, "ghijkl"));
	EXPECT_FALSE(estd::has(ussets, "ghi"));
	EXPECT_FALSE(estd::has(ussets, "mno"));
}


TEST(CONTAIN, Get)
{
	types::StrMapT<size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	types::StrUMapT<size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	size_t id;
	ASSERT_TRUE(estd::get(id, stuff, "abcdef"));
	EXPECT_EQ(123, id);
	ASSERT_TRUE(estd::get(id, stuff, "defghi"));
	EXPECT_EQ(456, id);
	ASSERT_TRUE(estd::get(id, stuff, "ghijkl"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, stuff, "ghi"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, stuff, "mno"));

	ASSERT_TRUE(estd::get(id, ustuff, "abcdef"));
	EXPECT_EQ(123, id);
	ASSERT_TRUE(estd::get(id, ustuff, "defghi"));
	EXPECT_EQ(456, id);
	ASSERT_TRUE(estd::get(id, ustuff, "ghijkl"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, ustuff, "ghi"));
	EXPECT_EQ(789, id);
	EXPECT_FALSE(estd::get(id, ustuff, "mno"));
	EXPECT_EQ(789, id);
}


TEST(CONTAIN, TryGet)
{
	types::StrMapT<size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	types::StrUMapT<size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	EXPECT_EQ(123, estd::try_get(stuff, "abcdef", 147));
	EXPECT_EQ(456, estd::try_get(stuff, "defghi", 147));
	EXPECT_EQ(789, estd::try_get(stuff, "ghijkl", 147));
	EXPECT_EQ(147, estd::try_get(stuff, "ghi", 147));
	EXPECT_EQ(147, estd::try_get(stuff, "mno", 147));

	EXPECT_EQ(123, estd::try_get(ustuff, "abcdef", 147));
	EXPECT_EQ(456, estd::try_get(ustuff, "defghi", 147));
	EXPECT_EQ(789, estd::try_get(ustuff, "ghijkl", 147));
	EXPECT_EQ(147, estd::try_get(ustuff, "ghi", 147));
	EXPECT_EQ(147, estd::try_get(ustuff, "mno", 147));
}


TEST(CONTAIN, MustGet)
{
	auto& logger = static_cast<exam::MockLogger&>(logs::get_logger());

	types::StrMapT<size_t> stuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	types::StrUMapT<size_t> ustuff = {
		{"abcdef", 123},
		{"defghi", 456},
		{"ghijkl", 789},
	};

	std::string ohnos = "oh nos %d";

	EXPECT_EQ(123, estd::must_getf(stuff, "abcdef", ohnos, 1));
	EXPECT_EQ(456, estd::must_getf(stuff, "defghi", ohnos, 2));
	EXPECT_EQ(789, estd::must_getf(stuff, "ghijkl", ohnos, 3));

	EXPECT_EQ(123, estd::must_getf(ustuff, "abcdef", ohnos, 4));
	EXPECT_EQ(456, estd::must_getf(ustuff, "defghi", ohnos, 5));
	EXPECT_EQ(789, estd::must_getf(ustuff, "ghijkl", ohnos, 6));

	std::string expect_msg1 = fmts::sprintf(ohnos, 7);
	std::string expect_msg2 = fmts::sprintf(ohnos, 8);
	std::string expect_msg3 = fmts::sprintf(ohnos, 9);
	std::string expect_msg4 = fmts::sprintf(ohnos, 0);
	EXPECT_CALL(logger, log(logs::FATAL, expect_msg1, _)).Times(1).WillOnce(Throw(exam::TestException(expect_msg1)));
	EXPECT_CALL(logger, log(logs::FATAL, expect_msg2, _)).Times(1).WillOnce(Throw(exam::TestException(expect_msg2)));
	EXPECT_CALL(logger, log(logs::FATAL, expect_msg3, _)).Times(1).WillOnce(Throw(exam::TestException(expect_msg3)));
	EXPECT_CALL(logger, log(logs::FATAL, expect_msg4, _)).Times(1).WillOnce(Throw(exam::TestException(expect_msg4)));
	EXPECT_FATAL(estd::must_getf(stuff, "ghi", ohnos, 7), expect_msg1.c_str());
	EXPECT_FATAL(estd::must_getf(stuff, "mno", ohnos, 8), expect_msg2.c_str());
	EXPECT_FATAL(estd::must_getf(ustuff, "ghi", ohnos, 9), expect_msg3.c_str());
	EXPECT_FATAL(estd::must_getf(ustuff, "mno", ohnos, 0), expect_msg4.c_str());
}


TEST(CONTAIN, ArrHas)
{
	std::array<size_t,3> arr = {123, 456, 789};
	std::vector<size_t> vec = {123, 456, 789};
	std::list<size_t> lst = {123, 456, 789};

	EXPECT_TRUE(estd::arr_has(arr, 123));
	EXPECT_TRUE(estd::arr_has(vec, 123));
	EXPECT_TRUE(estd::arr_has(lst, 123));

	EXPECT_TRUE(estd::arr_has(arr, 456));
	EXPECT_TRUE(estd::arr_has(vec, 456));
	EXPECT_TRUE(estd::arr_has(lst, 456));

	EXPECT_TRUE(estd::arr_has(arr, 789));
	EXPECT_TRUE(estd::arr_has(vec, 789));
	EXPECT_TRUE(estd::arr_has(lst, 789));

	EXPECT_FALSE(estd::arr_has(arr, 270));
	EXPECT_FALSE(estd::arr_has(vec, 270));
	EXPECT_FALSE(estd::arr_has(lst, 270));
}


#endif // DISABLE_ESTD_CONTAIN_TEST
