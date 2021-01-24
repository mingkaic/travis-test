#include <array>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "gtest/gtest.h"

#include "exam/exam.hpp"


using ::testing::_;
using ::testing::Return;
using ::testing::Throw;


int main (int argc, char** argv)
{

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_EXAM_TEST


TEST(EXAM, ArrEquality)
{
	std::array<size_t,3> arr = {123, 456, 789};
	std::vector<size_t> vec = {123, 456, 789};
	std::list<size_t> lst = {123, 456, 789};

	std::array<size_t,3> arr2 = {123, 456, 789};
	std::vector<size_t> vec2 = {123, 456, 789};
	std::list<size_t> lst2 = {123, 456, 789};

	std::list<size_t> outoforder = {456, 123, 789};
	std::vector<size_t> diff = {0, 2, 1, 22};

	ASSERT_ARREQ(arr, arr2);
	EXPECT_ARREQ(arr, vec2);
	ASSERT_ARREQ(arr, lst2);

	EXPECT_ARREQ(vec, arr2);
	ASSERT_ARREQ(vec, vec2);
	EXPECT_ARREQ(vec, lst2);

	ASSERT_ARREQ(lst, arr2);
	EXPECT_ARREQ(lst, vec2);
	ASSERT_ARREQ(lst, lst2);

	ASSERT_ARRNEQ(arr, outoforder);
	ASSERT_ARRNEQ(vec, outoforder);
	ASSERT_ARRNEQ(lst, outoforder);

	EXPECT_ARRNEQ(arr, diff);
	EXPECT_ARRNEQ(vec, diff);
	EXPECT_ARRNEQ(lst, diff);
}


TEST(EXAM, SetHasality)
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

	ASSERT_HAS(stuff, "abcdef");
	EXPECT_HAS(stuff, "defghi");
	ASSERT_HAS(stuff, "ghijkl");
	ASSERT_HASNOT(stuff, "ghi");
	EXPECT_HASNOT(stuff, "mno");

	EXPECT_HAS(ssets, "abcdef");
	ASSERT_HAS(ssets, "defghi");
	EXPECT_HAS(ssets, "ghijkl");
	EXPECT_HASNOT(ssets, "ghi");
	ASSERT_HASNOT(ssets, "mno");

	ASSERT_HAS(ustuff, "abcdef");
	EXPECT_HAS(ustuff, "defghi");
	ASSERT_HAS(ustuff, "ghijkl");
	ASSERT_HASNOT(ustuff, "ghi");
	EXPECT_HASNOT(ustuff, "mno");

	EXPECT_HAS(ussets, "abcdef");
	ASSERT_HAS(ussets, "defghi");
	EXPECT_HAS(ussets, "ghijkl");
	EXPECT_HASNOT(ussets, "ghi");
	ASSERT_HASNOT(ussets, "mno");
}


TEST(EXAM, ArrHasality)
{
	std::array<size_t,3> arr = {123, 456, 789};
	std::vector<size_t> vec = {123, 456, 789};
	std::list<size_t> lst = {123, 456, 789};

	ASSERT_ARRHAS(arr, 123);
	EXPECT_ARRHAS(vec, 123);
	ASSERT_ARRHAS(lst, 123);

	EXPECT_ARRHAS(arr, 456);
	ASSERT_ARRHAS(vec, 456);
	EXPECT_ARRHAS(lst, 456);

	ASSERT_ARRHAS(arr, 789);
	EXPECT_ARRHAS(vec, 789);
	ASSERT_ARRHAS(lst, 789);

	ASSERT_ARRHASNOT(arr, 270);
	EXPECT_ARRHASNOT(vec, 270);
	ASSERT_ARRHASNOT(lst, 270);
}


TEST(EXAM, Logality)
{
	auto tlogger = std::make_shared<exam::MockLogger>();

	logs::set_logger(std::static_pointer_cast<logs::iLogger>(tlogger));

	std::string log_level = "fatal";
	EXPECT_CALL(*tlogger, set_log_level(log_level)).Times(1);
	logs::get_logger().set_log_level(log_level);
	EXPECT_CALL(*tlogger, get_log_level()).Times(1).WillOnce(Return("trace"));
	EXPECT_STREQ("trace", logs::get_logger().get_log_level().c_str());

	auto fatal_action = [tlogger]
	{
		std::string fatalmsg = "fatal message";
		EXPECT_CALL(*tlogger, log(logs::FATAL, fatalmsg, _)).Times(1).WillOnce(Throw(exam::TestException(fatalmsg)));
		logs::fatal(fatalmsg);
		FAIL() << "fatal should never have gone this far";
	};

	auto fatal_action2 = [tlogger]
	{
		std::string fatalmsg = "fatal message2";
		EXPECT_CALL(*tlogger, log(logs::FATAL, fatalmsg, _)).Times(1).WillOnce(Throw(exam::TestException(fatalmsg)));
		logs::get_logger().log(logs::FATAL, fatalmsg);
		FAIL() << "fatal should never have gone this far";
	};

	auto fatal_action3 = [tlogger]
	{
		std::string fatalmsg = "fatal message3";
		EXPECT_CALL(*tlogger, log(logs::FATAL, fatalmsg, _)).Times(1).WillOnce(Throw(exam::TestException(fatalmsg)));
		logs::get_logger().log(logs::FATAL, fatalmsg);
		logs::get_logger().log(logs::THROW_ERR, fatalmsg);
		FAIL() << "fatal should never have gone this far";
	};

	EXPECT_FATAL(fatal_action(), "fatal message");
	EXPECT_FATAL(fatal_action2(), "fatal message2");
	EXPECT_FATAL(fatal_action3(), "fatal message3");

	logs::set_logger(nullptr);
}


TEST(EXAM, NoSupportLog)
{
	exam::NoSupportLogger log;
	log.set_log_level("123");
	EXPECT_STREQ("", log.get_log_level().c_str());
	EXPECT_FALSE(log.supports_level(0));
	EXPECT_FALSE(log.supports_level("123"));

	EXPECT_FALSE(log.called_);
	log.log(12, "12");
	EXPECT_TRUE(log.called_);

	log.called_ = false;
	log.log("321", "123");
	EXPECT_TRUE(log.called_);
}


#endif // DISABLE_EXAM_TEST
