
#ifndef PKG_ERROR_IERROR_HPP
#define PKG_ERROR_IERROR_HPP

#include <memory>

#include "fmts/fmts.hpp"

namespace error
{

struct iError
{
	virtual ~iError (void) = default;

	virtual std::string to_string (void) const = 0;
};

struct ErrMsg final : public iError
{
	ErrMsg (const std::string& msg) : msg_(msg) {}

	template <typename ...ARGS>
	ErrMsg (const std::string& format, ARGS... args) :
		ErrMsg(fmts::sprintf(format, args...)) {}

	std::string to_string (void) const override
	{
		return msg_;
	}

private:
	std::string msg_;
};

using ErrptrT = std::shared_ptr<iError>;

ErrptrT error (const std::string& msg);

template <typename... ARGS>
ErrptrT errorf (const std::string& format, ARGS... args)
{
	return std::make_shared<ErrMsg>(format, args...);
}

}

#endif // PKG_ERROR_IERROR_HPP
