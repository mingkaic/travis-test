///
/// scope_guard.hpp
/// jobs
///
/// Purpose:
/// Simulate golang defer statement
///

#ifndef PKG_JOBS_SCOPE_GUARD_HPP
#define PKG_JOBS_SCOPE_GUARD_HPP

#include <functional>

namespace jobs
{

using GuardOpF = std::function<void(void)>;

/// Invoke held function upon destruction
/// Operates as C++ style of Golang's defer
struct ScopeGuard // todo: replace with a better option
{
	ScopeGuard (GuardOpF f) : term_(f) {}

	virtual ~ScopeGuard (void)
	{
		if (term_)
		{
			term_();
		}
	}

	ScopeGuard (const ScopeGuard&) = delete;

	ScopeGuard (ScopeGuard&& other) :
		term_(std::move(other.term_)) {}

	ScopeGuard& operator = (const ScopeGuard&) = delete;

	ScopeGuard& operator = (ScopeGuard&& other)
	{
		if (this != &other)
		{
			if (term_)
			{
				term_();
			}
			term_ = std::move(other.term_);
		}
		return *this;
	}

private:
	GuardOpF term_;
};

}

#endif // PKG_JOBS_SCOPE_GUARD_HPP
