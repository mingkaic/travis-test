
#ifndef PKG_EGRPC_CLIENT_ASYNC_HPP
#define PKG_EGRPC_CLIENT_ASYNC_HPP

#include "egrpc/iclient_async.hpp"

namespace egrpc
{

template <typename REQ, typename RES>
struct AsyncClientHandler final : public iClientHandler
{
	using ReadptrT = std::shared_ptr<
		grpc::ClientAsyncResponseReaderInterface<RES>>;

	using HandleResF = std::function<void(RES&)>;

	using InitF = std::function<void(REQ&,AsyncClientHandler<REQ,RES>*)>;

	AsyncClientHandler (ErrPromiseptrT promise,
		std::shared_ptr<logs::iLogger> logger,
		HandleResF cb, InitF init, size_t nretries) :
		iClientHandler(promise), logger_(logger),
		nretries_(nretries), init_(init), cb_(cb)
	{
		init_(request_, this);
	}

	void handle (bool) override
	{
		if (status_.ok())
		{
			logger_->log(logs::info_level, fmts::sprintf(
				"call %p completed successfully", this));
			if (cb_)
			{
				cb_(reply_);
			}
		}
		else
		{
			if (nretries_ > 0)
			{
				logger_->log(logs::error_level, fmts::sprintf(
					"call %p (%d attempts remaining) failed: %s",
					this, nretries_, status_.error_message().c_str()));
				new AsyncClientHandler<REQ,RES>(
					complete_promise_, logger_, cb_, init_, nretries_ - 1);
				complete_promise_ = nullptr;
			}
			else
			{
				error_ = error::error(status_.error_message());
				logger_->log(logs::error_level, fmts::sprintf(
					"call %p failed: %s",
					this, status_.error_message().c_str()));
			}
		}
		delete this;
	}

	ReadptrT reader_;

	grpc::Status status_;

	// ctx_ and reader_ need to be kept in memory
	grpc::ClientContext ctx_;

	RES reply_;

private:
	std::shared_ptr<logs::iLogger> logger_;

	REQ request_;

	size_t nretries_;

	InitF init_;

	HandleResF cb_;
};

}

#endif // PKG_EGRPC_CLIENT_ASYNC_HPP
