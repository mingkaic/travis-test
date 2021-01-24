
#ifndef DISABLE_HASHLIST_TEST

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/hashlist.hpp"


TEST(ESTD, Iterators)
{
	std::vector<size_t> vec = {2, 1, 3, 1, 4, 5};
	estd::HashList<size_t> a(vec.begin(), vec.end());

	EXPECT_EQ(2, a.front());
	EXPECT_EQ(5, a.back());

	[](const estd::HashList<size_t>& a)
	{
		EXPECT_EQ(2, a.front());
		EXPECT_EQ(5, a.back());

		std::list<size_t> got;
		for (auto it = a.cbegin(), et = a.cend(); it != et; ++it)
		{
			got.push_back(*it);
		}
		for (auto rit = a.crbegin(), ret = a.crend(); rit != ret; ++rit)
		{
			EXPECT_EQ(got.back(), *rit);
			got.pop_back();
		}
	}(a);

	std::list<size_t> lst = {2, 1, 3, 4, 5};
	for (auto rit = a.rbegin(), ret = a.rend(); rit != ret; ++rit)
	{
		EXPECT_EQ(lst.back(), *rit);
		lst.pop_back();
	}
}


TEST(ESTD, HashListUniqueness)
{
	std::vector<size_t> vec = {2, 1, 3, 1, 4, 5};
	estd::HashList<size_t> a(vec.begin(), vec.end());

	std::vector<size_t> exout = {2, 1, 3, 4, 5};
	ASSERT_ARREQ(exout, a);
	EXPECT_EQ(5, a.size());
	EXPECT_FALSE(a.empty());

	EXPECT_EQ(2, a.front());
	EXPECT_EQ(5, a.back());

	estd::HashList<size_t> b;
	EXPECT_EQ(0, b.size());
	EXPECT_TRUE(b.empty());
}


TEST(ESTD, HashListInsertion)
{
	estd::HashList<size_t> a;
	a.push_back(5);
	a.insert(a.begin(), 2);
	a.push_back(3);
	a.push_back(4);

	std::vector<size_t> exout = {2, 5, 3, 4};
	ASSERT_ARREQ(exout, a);

	auto it = a.begin();
	++it;
	++it;
	a.insert(it, 6);
	++it;
	a.insert(it, 3);
	a.push_back(3);

	std::vector<size_t> exout2 = {2, 5, 6, 3, 4};
	ASSERT_ARREQ(exout2, a);
}


TEST(ESTD, HashListCleanup)
{
	std::vector<size_t> vec = {2, 5, 6, 3, 4};
	estd::HashList<size_t> a(vec.begin(), vec.end());

	a.erase(5);

	std::vector<size_t> exout = {2, 6, 3, 4};
	ASSERT_ARREQ(exout, a);

	a.erase(2);
	a.erase(4);
	a.erase(7);

	std::vector<size_t> exout2 = {6, 3};
	ASSERT_ARREQ(exout2, a);

	a.pop_back();
	std::vector<size_t> exout3 = {6};
	ASSERT_ARREQ(exout3, a);

	a.clear();
	EXPECT_EQ(0, a.size());
	EXPECT_TRUE(a.empty());

	a.pop_back();
	EXPECT_EQ(0, a.size());
	EXPECT_TRUE(a.empty());
}


#endif // DISABLE_HASHLIST_TEST
