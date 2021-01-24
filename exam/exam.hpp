///
/// exam.hpp
/// exam
///
/// Purpose:
/// Define test utility functions
///

#ifndef PKG_EXAM_HPP
#define PKG_EXAM_HPP

#include "fmts/fmts.hpp"

#include "exam/mock_log.hpp"
#include "exam/nosupport_log.hpp"

namespace exam
{

struct TestException final : public std::exception
{
	TestException (const std::string& msg) : msg_(msg) {}

	const char* what () const throw ()
	{
		return msg_.c_str();
	}

	std::string msg_;
};

#define _ARRCHECK(ARR, ARR2, GBOOL) { std::stringstream arrs, arrs2;\
	fmts::to_stream(arrs, ARR.begin(), ARR.end());\
	fmts::to_stream(arrs2, ARR2.begin(), ARR2.end());\
	GBOOL(std::equal(ARR.begin(), ARR.end(), ARR2.begin())) <<\
		"expect list " << arrs.str() << ", got " << arrs2.str() << " instead"; }
#define _VECCHECK(VEC, VEC2, GBOOL) { std::stringstream vecs, vecs2;\
	fmts::to_stream(vecs, VEC.begin(), VEC.end());\
	fmts::to_stream(vecs2, VEC2.begin(), VEC2.end());\
	GBOOL(VEC.size() == VEC2.size() &&\
		std::equal(VEC.begin(), VEC.end(), VEC2.begin())) <<\
		"expect list " << vecs.str() << ", got " << vecs2.str() << " instead"; }
#define _INSET(SET, CONTENT, GBOOL, PREFIX_MSG) {\
	GBOOL(SET.end() != SET.find(CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #SET; }
#define _INARR(ARR, CONTENT, GBOOL, PREFIX_MSG) { auto et = ARR.end();\
	GBOOL(et != std::find(ARR.begin(), et, CONTENT)) <<\
		PREFIX_MSG << " find " << #CONTENT << " in " << #ARR; }

#define ASSERT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_TRUE)
#define EXPECT_ARREQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_TRUE)
#define ASSERT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, ASSERT_FALSE)
#define EXPECT_ARRNEQ(ARR, ARR2) _ARRCHECK(ARR, ARR2, EXPECT_FALSE)

#define ASSERT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_TRUE)
#define EXPECT_VECEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_TRUE)
#define ASSERT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, ASSERT_FALSE)
#define EXPECT_VECNEQ(VEC, VEC2) _VECCHECK(VEC, VEC2, EXPECT_FALSE)

#define ASSERT_HAS(SET, KEY) _INSET(SET, KEY, ASSERT_TRUE, "expecting to")
#define EXPECT_HAS(SET, KEY) _INSET(SET, KEY, EXPECT_TRUE, "expecting to")
#define ASSERT_HASNOT(SET, KEY) _INSET(SET, KEY, ASSERT_FALSE, "cannot")
#define EXPECT_HASNOT(SET, KEY) _INSET(SET, KEY, EXPECT_FALSE, "cannot")

#define ASSERT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_TRUE, "expecting to")
#define EXPECT_ARRHAS(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_TRUE, "expecting to")
#define ASSERT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, ASSERT_FALSE, "cannot")
#define EXPECT_ARRHASNOT(ARR, CONTENT) _INARR(ARR, CONTENT, EXPECT_FALSE, "cannot")

#define EXPECT_FATAL(EVENT, MSG) try { EVENT; FAIL() <<\
	"did not expect " << #EVENT << " to succeed"; }\
	catch (exam::TestException& e) { EXPECT_STREQ(MSG, e.what()); }\
	catch (std::exception& e) { FAIL() << "unexpected throw " << e.what(); }

}

#endif // PKG_EXAM_HPP
