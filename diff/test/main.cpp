#include "gtest/gtest.h"

#include "diff/diff.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_DIFF_TEST


TEST(DIFF, MyersMinEdit)
{
	EXPECT_EQ(0, diff::myers_diff_min_edit<std::string>(
		"ABCABBA", "ABCABBA"));
	EXPECT_EQ(5, diff::myers_diff_min_edit<std::string>(
		"ABCABBA", "CBABAC"));
	EXPECT_EQ(13, diff::myers_diff_min_edit<std::string>(
		"ABCDEFG", "HIJKLM"));
}


TEST(DIFF, MyersDiff)
{
	std::string s = "ABCABBA";
	auto same = diff::myers_diff<std::string>(s, "ABCABBA");
	for (size_t i = 0, n = same.size(); i < n; ++i)
	{
		EXPECT_EQ(s[i], same[i].val_);
		EXPECT_EQ(i, same[i].orig_);
		EXPECT_EQ(i, same[i].updated_);
		EXPECT_EQ(diff::EQ, same[i].action_);
	}

	std::string similar = "CBABAC";
	auto notsame = diff::myers_diff<std::string>(s, similar);
	types::StringsT expectations = {
		"- 0\t \tA\n",
		"- 1\t \tB\n",
		"  2\t0\tC\n",
		"+  \t1\tB\n",
		"  3\t2\tA\n",
		"  4\t3\tB\n",
		"- 5\t \tB\n",
		"  6\t4\tA\n",
		"+  \t5\tC\n",
	};
	ASSERT_EQ(expectations.size(), notsame.size());
	for (size_t i = 0, n = notsame.size(); i < n; ++i)
	{
		std::stringstream ss;
		diff::diff_line_format(ss, notsame[i].val_,
			notsame[i].action_, notsame[i].orig_, notsame[i].updated_);
		EXPECT_STREQ(expectations[i].c_str(), ss.str().c_str());
	}

	std::string other = "DEFEDFF";
	auto different = diff::myers_diff<std::string>(s, other);
	size_t n = s.size();
	for (size_t i = 0; i < n; ++i)
	{
		EXPECT_EQ(s[i], different[i].val_);
		EXPECT_EQ(i, different[i].orig_);
		EXPECT_EQ(-1, different[i].updated_);
		EXPECT_EQ(diff::DEL, different[i].action_);
	}
	for (size_t i = 0, m = other.size(); i < m; ++i)
	{
		EXPECT_EQ(other[i], different[n + i].val_);
		EXPECT_EQ(-1, different[n + i].orig_);
		EXPECT_EQ(i, different[n + i].updated_);
		EXPECT_EQ(diff::ADD, different[n + i].action_);
	}
}


TEST(DIFF, Msg_CompleteMatch)
{
	auto match = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	});

	EXPECT_STREQ("", match.c_str());
}


TEST(DIFF, Msg_SamePrefix)
{
	auto same_prefix = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"mate",
		"head",
		"wine",
		"new",
		"tremble",
		"hop",
	});

	EXPECT_STREQ(
		"  3\t3\tgreasy\n"
		"  4\t4\ttoy\n"
		"  5\t5\tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t6\tmate\n"
		"+  \t7\thead\n"
		"+  \t8\twine\n"
		"+  \t9\tnew\n"
		"+  \t10\ttremble\n"
		"+  \t11\thop\n",
		same_prefix.c_str());
}


TEST(DIFF, Msg_SamePostfix)
{
	auto same_postfix = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"  7\t7\tjuicy\n"
		"  8\t8\tbright\n"
		"  9\t9\tjam\n",
		same_postfix.c_str());
}


TEST(DIFF, Msg_SporadicSimilar)
{
	auto small_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"obtain",
		"nosy",
		"juicy",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"  5\t4\tobtain\n"
		"  6\t5\tnosy\n"
		"  7\t6\tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t7\tcomplete\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		small_overlap.c_str());
}


TEST(DIFF, Msg_MoreSimilar)
{
	auto bigger_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"  3\t2\tgreasy\n"
		"  4\t3\ttoy\n"
		"  5\t4\tobtain\n"
		"  7\t6\tjuicy\n"
		"  8\t7\tbright\n"
		"  9\t8\tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		bigger_overlap.c_str());
}


TEST(DIFF, Msg_CompleteDiff)
{
	auto no_overlap = diff::diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	});

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"+  \t7\tcomplete\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		no_overlap.c_str());
}


TEST(DIFF, SafeMsg_CompleteMatch)
{
	auto match = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, 8);

	EXPECT_STREQ("", match.c_str());

	auto match2 = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	});

	EXPECT_STREQ("", match2.c_str());
}


TEST(DIFF, SafeMsg_SamePrefix)
{
	auto same_prefix = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"mate",
		"head",
		"wine",
		"new",
		"tremble",
		"hop",
	}, 8);

	EXPECT_STREQ(
		"  3\t3\tgreasy\n"
		"  4\t4\ttoy\n"
		"  5\t5\tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t6\tmate\n"
		"+  \t7\thead\n"
		"+  \t8\twine\n"
		"+  \t9\tnew\n"
		"+  \t10\ttremble\n"
		"+  \t11\thop\n",
		same_prefix.c_str());

	auto same_prefix2 = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"mate",
		"head",
		"wine",
		"new",
		"tremble",
		"hop",
	});

	EXPECT_STREQ(
		"  3\t3\tgreasy\n"
		"  4\t4\ttoy\n"
		"  5\t5\tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t6\tmate\n"
		"+  \t7\thead\n"
		"+  \t8\twine\n"
		"+  \t9\tnew\n"
		"+  \t10\ttremble\n"
		"+  \t11\thop\n",
		same_prefix2.c_str());
}


TEST(DIFF, SafeMsg_SamePostfix)
{
	auto same_postfix = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, 8);

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"  7\t7\tjuicy\n"
		"  8\t8\tbright\n"
		"  9\t9\tjam\n",
		same_postfix.c_str());
}


TEST(DIFF, SafeMsg_SporadicSimilar)
{
	auto small_overlap = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"obtain",
		"nosy",
		"juicy",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	}, 8);

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"  5\t4\tobtain\n"
		"  6\t5\tnosy\n"
		"  7\t6\tjuicy\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t7\tcomplete\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		small_overlap.c_str());
}


TEST(DIFF, SafeMsg_MoreSimilar)
{
	auto bigger_overlap = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"tremble",
		"bed",
		"hop",
	}, 8);

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"  3\t2\tgreasy\n"
		"  4\t3\ttoy\n"
		"  5\t4\tobtain\n"
		"  7\t6\tjuicy\n"
		"  8\t7\tbright\n"
		"  9\t8\tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		bigger_overlap.c_str());
}


TEST(DIFF, SafeMsg_CompleteDiff)
{
	auto no_overlap = diff::safe_diff_msg({
		"advise",
		"apathetic",
		"disappear",
		"greasy",
		"toy",
		"obtain",
		"nosy",
		"juicy",
		"bright",
		"jam",
		"dust",
		"silent",
	}, {
		"mate",
		"head",
		"wine",
		"abrupt",
		"whistle",
		"new",
		"sneeze",
		"complete",
		"spiky",
		"tremble",
		"bed",
		"hop",
	}, 8);

	EXPECT_STREQ(
		"- 0\t \tadvise\n"
		"- 1\t \tapathetic\n"
		"- 2\t \tdisappear\n"
		"- 3\t \tgreasy\n"
		"- 4\t \ttoy\n"
		"- 5\t \tobtain\n"
		"- 6\t \tnosy\n"
		"- 7\t \tjuicy\n"
		"+  \t0\tmate\n"
		"+  \t1\thead\n"
		"+  \t2\twine\n"
		"+  \t3\tabrupt\n"
		"+  \t4\twhistle\n"
		"+  \t5\tnew\n"
		"+  \t6\tsneeze\n"
		"+  \t7\tcomplete\n"
		"- 8\t \tbright\n"
		"- 9\t \tjam\n"
		"- 10\t \tdust\n"
		"- 11\t \tsilent\n"
		"+  \t8\tspiky\n"
		"+  \t9\ttremble\n"
		"+  \t10\tbed\n"
		"+  \t11\thop\n",
		no_overlap.c_str());
}


TEST(DIFF, DiffLines)
{
	std::stringstream left;
	left << " advise  \n"
		"  apathetic\n\n"
		" disappear   \n"
		"greasy \n"
		"\ttoy\n"
		"\t\t  obtain\n"
		"nosy\n"
		"juicy\t\n"
		"bright\n\n"
		"jam\n"
		"dust\n\n"
		"silent\n";

	std::stringstream right;
	right << "advise\n"
		"  apathetic\n"
		" disappear   \n"
		"greasy\t\n"
		"  toy\n"
		"\tobtain\n"
		"nosy\n\n"
		"juicy\n"
		"bright\n"
		"jam\n"
		"  dust\n"
		"silent";

	std::stringstream left2(left.str());
	std::stringstream right2(right.str());
	std::stringstream left3(left.str());
	std::stringstream right3(right.str());
	std::stringstream left4(left.str());
	std::stringstream right4(right.str());

	auto diff = diff::diff_lines(left, right, false, false); // compare as is
	EXPECT_STREQ(
		"- 0\t \t advise  \n"
		"+  \t0\tadvise\n"
		"  1\t1\t  apathetic\n"
		"- 2\t \t\n"
		"  3\t2\t disappear   \n"
		"- 4\t \tgreasy \n"
		"- 5\t \t\ttoy\n"
		"- 6\t \t\t\t  obtain\n"
		"+  \t3\tgreasy\t\n"
		"+  \t4\t  toy\n"
		"+  \t5\t\tobtain\n"
		"  7\t6\tnosy\n"
		"- 8\t \tjuicy\t\n"
		"- 9\t \tbright\n"
		"  10\t7\t\n"
		"+  \t8\tjuicy\n"
		"+  \t9\tbright\n"
		"  11\t10\tjam\n"
		"- 12\t \tdust\n"
		"- 13\t \t\n"
		"+  \t11\t  dust\n"
		"  14\t12\tsilent\n", diff.c_str());

	auto diff2 = diff::diff_lines(left2, right2, true, false); // ignore empty lines
	EXPECT_STREQ(
		"- 0\t \t advise  \n"
		"+  \t0\tadvise\n"
		"  1\t1\t  apathetic\n"
		"  2\t2\t disappear   \n"
		"- 3\t \tgreasy \n"
		"- 4\t \t\ttoy\n"
		"- 5\t \t\t\t  obtain\n"
		"+  \t3\tgreasy\t\n"
		"+  \t4\t  toy\n"
		"+  \t5\t\tobtain\n"
		"  6\t6\tnosy\n"
		"- 7\t \tjuicy\t\n"
		"+  \t7\tjuicy\n"
		"  8\t8\tbright\n"
		"  9\t9\tjam\n"
		"- 10\t \tdust\n"
		"+  \t10\t  dust\n"
		"  11\t11\tsilent\n", diff2.c_str());

	auto diff3 = diff::diff_lines(left3, right3, false, true); // trim space
	EXPECT_STREQ(
		"  0\t0\tadvise\n"
		"  1\t1\tapathetic\n"
		"- 2\t \t\n"
		"  3\t2\tdisappear\n"
		"  4\t3\tgreasy\n"
		"  5\t4\ttoy\n"
		"  6\t5\tobtain\n"
		"  7\t6\tnosy\n"
		"+  \t7\t\n"
		"  8\t8\tjuicy\n"
		"  9\t9\tbright\n"
		"- 10\t \t\n"
		"  11\t10\tjam\n"
		"  12\t11\tdust\n"
		"- 13\t \t\n"
		"  14\t12\tsilent\n", diff3.c_str());

	auto diff4 = diff::diff_lines(left4, right4, true, true); // full cleanup
	EXPECT_STREQ("", diff4.c_str());
}


#endif // DISABLE_DIFF_TEST
