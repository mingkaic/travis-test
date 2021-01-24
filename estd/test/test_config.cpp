
#ifndef DISABLE_ESTD_CONFIG_TEST

#include <array>
#include <list>
#include <vector>

#include "gtest/gtest.h"

#include "exam/exam.hpp"

#include "estd/config.hpp"


TEST(CONFIG, HasKey)
{
	estd::ConfigMap<> stuff;
	stuff.add_entry<size_t>("abcdef",
		[](){ return new size_t(123); });
	stuff.add_entry<size_t>("defghi",
		[](){ return new size_t(456); });
	stuff.add_entry<size_t>("ghijkl",
		[](){ return new size_t(789); });
	stuff.add_entry<std::string>("mno",
		[](){ return new std::string("mno"); });
	stuff.add_entry<std::string>("ghi",
		[](){ return new std::string("ghi"); });

	EXPECT_TRUE(stuff.has_key("abcdef"));
	EXPECT_TRUE(stuff.has_key("defghi"));
	EXPECT_TRUE(stuff.has_key("ghijkl"));
	EXPECT_TRUE(stuff.has_key("ghi"));
	EXPECT_TRUE(stuff.has_key("mno"));

	EXPECT_EQ(5, stuff.get_keys().size());
}


TEST(CONFIG, Get)
{
	estd::ConfigMap<> stuff;
	stuff.add_entry<size_t>("abcdef",
		[](){ return new size_t(123); });
	stuff.add_entry<size_t>("defghi",
		[](){ return new size_t(456); });
	stuff.add_entry<size_t>("ghijkl",
		[](){ return new size_t(789); });
	stuff.add_entry<std::string>("mno",
		[](){ return new std::string("mno"); });
	stuff.add_entry<std::string>("ghi",
		[](){ return new std::string("ghi"); });

	EXPECT_EQ(123, *static_cast<size_t*>(stuff.get_obj("abcdef")));
	EXPECT_EQ(456, *static_cast<size_t*>(stuff.get_obj("defghi")));
	EXPECT_EQ(789, *static_cast<size_t*>(stuff.get_obj("ghijkl")));
	EXPECT_STREQ("mno", static_cast<std::string*>(stuff.get_obj("mno"))->c_str());
	EXPECT_STREQ("ghi", static_cast<std::string*>(stuff.get_obj("ghi"))->c_str());
}


TEST(CONFIG, Removal)
{
	estd::ConfigMap<> stuff;
	stuff.add_entry<size_t>("abcdef",
		[](){ return new size_t(123); });
	stuff.add_entry<size_t>("defghi",
		[](){ return new size_t(456); });
	stuff.add_entry<size_t>("ghijkl",
		[](){ return new size_t(789); });
	stuff.add_entry<std::string>("mno",
		[](){ return new std::string("mno"); });
	stuff.add_entry<std::string>("ghi",
		[](){ return new std::string("ghi"); });

	stuff.rm_entry("defghi");
	stuff.rm_entry("ghi");

	EXPECT_EQ(3, stuff.get_keys().size());

	EXPECT_TRUE(stuff.has_key("abcdef"));
	EXPECT_FALSE(stuff.has_key("defghi"));
	EXPECT_TRUE(stuff.has_key("ghijkl"));
	EXPECT_FALSE(stuff.has_key("ghi"));
	EXPECT_TRUE(stuff.has_key("mno"));

	EXPECT_NE(nullptr, stuff.get_obj("abcdef"));
	EXPECT_EQ(nullptr, stuff.get_obj("defghi"));
	EXPECT_NE(nullptr, stuff.get_obj("ghijkl"));
	EXPECT_EQ(nullptr, stuff.get_obj("ghi"));
	EXPECT_NE(nullptr, stuff.get_obj("mno"));
}


TEST(CONFIG, Copy)
{
	size_t dels = 0;
	auto del1 = [&dels](void* ptr) { delete static_cast<size_t*>(ptr); dels |= 1; };
	auto del2 = [&dels](void* ptr) { delete static_cast<size_t*>(ptr); dels |= 2; };
	auto del3 = [&dels](void* ptr) { delete static_cast<size_t*>(ptr); dels |= 4; };
	auto del4 = [&dels](void* ptr) { delete static_cast<std::string*>(ptr); dels |= 8; };
	auto del5 = [&dels](void* ptr) { delete static_cast<std::string*>(ptr); dels |= 16; };

	{
		estd::ConfigMap<> outer;
		{
			estd::ConfigMap<>* stuff = new estd::ConfigMap<>();
			stuff->add_entry<size_t>("abcdef", [](){ return new size_t(123); }, del1);
			stuff->add_entry<size_t>("defghi", [](){ return new size_t(456); }, del2);
			stuff->add_entry<size_t>("ghijkl", [](){ return new size_t(789); }, del3);
			stuff->add_entry<std::string>("mno", [](){ return new std::string("mno"); }, del4);
			stuff->add_entry<std::string>("ghi", [](){ return new std::string("ghi"); }, del5);

			estd::ConfigMap<> other(*stuff); // share ownership with other
			EXPECT_EQ(stuff->get_keys().size(), other.get_keys().size()); // check that stuff still has the entries

			delete stuff;
			EXPECT_EQ(0, dels); // no deletion should have occurred since other now owns the entries

			outer = other; // share ownership with outer
			EXPECT_EQ(other.get_keys().size(), outer.get_keys().size()); // check that stuff still has the entries
		}
		EXPECT_EQ(0, dels); // no deletion should have occurred since outer now owns the entries
	}
	EXPECT_EQ(0b11111, dels); // deletion now occurs because of outer exited scope
}


#endif // DISABLE_ESTD_CONFIG_TEST
