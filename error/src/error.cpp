
#include "error/error.hpp"

#ifdef PKG_ERROR_IERROR_HPP

namespace error
{

ErrptrT error (const std::string& msg)
{
	return std::make_shared<ErrMsg>(msg);
}

}

#endif
