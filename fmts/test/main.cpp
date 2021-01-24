#include <array>
#include <list>
#include <vector>
#include <unordered_map>

#include "gtest/gtest.h"

#include "gmock/gmock.h"

#include "fmts/fmts.hpp"


using ::testing::Return;


struct MockStringable : public fmts::iStringable
{
	virtual ~MockStringable (void) = default;

	MOCK_CONST_METHOD0(to_string, std::string(void));
};


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_FMTS_TEST


TEST(FMTS, StringableFmt)
{
	auto stringable = std::make_shared<MockStringable>();

	EXPECT_CALL(*stringable, to_string()).Times(1).WillOnce(Return("abcdefghijkl"));
	std::stringstream ss;
	fmts::to_stream(ss, stringable.get());
	EXPECT_STREQ("abcdefghijkl", ss.str().c_str());
	ss.str("");

	EXPECT_CALL(*stringable, to_string()).Times(1).WillOnce(Return("lmnopqrstuv"));
	fmts::to_stream(ss, stringable);
	EXPECT_STREQ("lmnopqrstuv", ss.str().c_str());
}


TEST(FMTS, StringFmt)
{
	std::stringstream ss;
	fmts::to_stream(ss, "abcdefghijkl");
	EXPECT_STREQ("abcdefghijkl", ss.str().c_str());
	ss.str("");

	fmts::to_stream(ss, fmts::String("abcd\\efgh\\ijkl\\"));
	EXPECT_STREQ("abcd\\\\efgh\\\\ijkl\\\\", ss.str().c_str());
	ss.str("");

	fmts::to_stream(ss, fmts::String("\\abcd\\efgh\\ijkl"));
	EXPECT_STREQ("\\\\abcd\\\\efgh\\\\ijkl", ss.str().c_str());
}


TEST(FMTS, GenericFmt)
{
	std::stringstream ss;
	fmts::to_stream(ss, -15);
	EXPECT_STREQ("-15", ss.str().c_str());
	ss.str("");

	fmts::to_stream(ss, 16.001);
	EXPECT_STREQ("16.001", ss.str().c_str());
}


TEST(FMTS, StreamIterators)
{
	std::stringstream ss;
	std::vector<int> ivec = {14, 15, 16};
	fmts::to_stream(ss, ivec.begin(), ivec.end());
	EXPECT_STREQ("[14\\15\\16]", ss.str().c_str());
	ss.str("");

	types::StringsT svec = {
		fmts::String("what's\\up\\mybro"),
		fmts::String("nothing\\much\\fam"),
		fmts::String("\\hella\\lit")
	};
	fmts::to_stream(ss, svec.begin(), svec.end());
	EXPECT_STREQ("[what's\\\\up\\\\mybro\\nothing\\\\much\\\\"
		"fam\\\\\\hella\\\\lit]", ss.str().c_str());
	ss.str("");

	std::vector<int> emptyvec;
	fmts::to_stream(ss, emptyvec.begin(), emptyvec.end());
	EXPECT_STREQ("[]", ss.str().c_str());
	ss.str("");

	std::array<uint8_t,4> uarr = {9, 0, 3, 36};
	std::list<int8_t> ilist = {-5, -2, 13, 61};
	fmts::to_stream(ss, uarr.begin(), uarr.end());
	EXPECT_STREQ("[9\\0\\3\\36]", ss.str().c_str());
	ss.str("");

	fmts::to_stream(ss, ilist.begin(), ilist.end());
	EXPECT_STREQ("[-5\\-2\\13\\61]", ss.str().c_str());
}


TEST(FMTS, StringIterators)
{
	std::vector<double> dbs = {1.5, 1, 5.6, 7.8};
	std::array<int,4> iar = {-5, 2, -3, 6};
	types::StrUMapT<size_t> s2i = {
		{"abc", 123},
		{"cde", 345},
		{"efg", 567},
	};

	std::string dbstr = fmts::to_string(dbs.begin(), dbs.end());
	std::string iarstr = fmts::to_string(iar.begin(), iar.end());
	std::string s2istr = fmts::to_string(s2i.begin(), s2i.end());

	EXPECT_STREQ("[1.5\\1\\5.6\\7.8]", dbstr.c_str());
	EXPECT_STREQ("[-5\\2\\-3\\6]", iarstr.c_str());
	EXPECT_STREQ("[efg:567\\cde:345\\abc:123]", s2istr.c_str());
}


TEST(FMTS, Sprintf)
{
	const char* str = "string";
	const char* oth = "other";
	std::string s = fmts::sprintf("%% %s %d %.3f %% %%%% %s %d %.1f %% %%%%",
		str, 123, 5.689, oth, 77, 0.4, 12);
	EXPECT_STREQ("% string 123 5.689 % %% other 77 0.4 % %%", s.c_str());
}


TEST(FMTS, LeftTrim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::ltrim(content);
	EXPECT_STREQ("abce  dfgi hjk lm  ", content.c_str());
}


TEST(FMTS, RightTrim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::rtrim(content);
	EXPECT_STREQ("    abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Trim)
{
	std::string content = "    abce  dfgi hjk lm  ";
	fmts::trim(content);
	EXPECT_STREQ("abce  dfgi hjk lm", content.c_str());
}

TEST(FMTS, LeftStrip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::lstrip(content, std::unordered_set<char>{'_', '*', '('});
	EXPECT_STREQ("abce  dfgi hjk lm)*", content.c_str());
}


TEST(FMTS, RightStrip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::rstrip(content, std::unordered_set<char>{'_', '*', ')'});
	EXPECT_STREQ("_*_(abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Strip)
{
	std::string content = "_*_(abce  dfgi hjk lm)*";
	fmts::strip(content, std::unordered_set<char>{'_', '*', '(', ')'});
	EXPECT_STREQ("abce  dfgi hjk lm", content.c_str());
}


TEST(FMTS, Split)
{
	std::string content = ", ,abce, df,gi, hjk, lm, ";
	auto arrs = fmts::split(content, ", ");
	// expect ["", ",abce", "df,gi", "hjk", "lm", ""]
	ASSERT_EQ(6, arrs.size());
	EXPECT_STREQ("", arrs[0].c_str());
	EXPECT_STREQ(",abce", arrs[1].c_str());
	EXPECT_STREQ("df,gi", arrs[2].c_str());
	EXPECT_STREQ("hjk", arrs[3].c_str());
	EXPECT_STREQ("lm", arrs[4].c_str());
	EXPECT_STREQ("", arrs[5].c_str());
}


#endif // DISABLE_FMTS_TEST
