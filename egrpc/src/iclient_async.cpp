#include "egrpc/iclient_async.hpp"

#ifdef PKG_EGRPC_ICLIENT_ASYNC_HPP

namespace egrpc
{

void wait_for (ErrPromiseT& promise, HandleErrF err_handle)
{
	auto done = promise.get_future();
	while (done.valid() && done.wait_for(
		std::chrono::milliseconds(1)) ==
		std::future_status::timeout);
	if (done.valid())
	{
		auto err = done.get();
		if (nullptr != err && err_handle)
		{
			err_handle(err);
		}
	}
}

void wait_for (ErrPromisesT& promises, HandleErrF err_handle)
{
	while (false == promises.empty())
	{
		auto done = promises.front();
		wait_for(*done, err_handle);
		promises.pop_front();
	}
}

}

#endif
