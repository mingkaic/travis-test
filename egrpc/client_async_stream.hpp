
#ifndef PKG_EGRPC_CLIENT_ASYNC_STREAM_HPP
#define PKG_EGRPC_CLIENT_ASYNC_STREAM_HPP

#include "egrpc/iclient_async.hpp"

namespace egrpc
{

template <typename DATA>
struct AsyncClientStreamHandler final : public iClientHandler
{
	using ReadptrT = std::shared_ptr<
		grpc::ClientAsyncReaderInterface<DATA>>;

	using HandlerF = std::function<void(DATA&)>;

	AsyncClientStreamHandler (ErrPromiseptrT promise,
		std::shared_ptr<logs::iLogger> logger, HandlerF handler) :
		iClientHandler(promise), logger_(logger),
		call_status_(STARTUP), handler_(handler) {}

	void handle (bool event_status) override
	{
		assert(nullptr != reader_);
		switch (call_status_)
		{
		case STARTUP:
			if (event_status)
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p created... processing", this));
				call_status_ = PROCESS;
				reader_->Read(&reply_, (void*) this);
			}
			else
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p created... finishing", this));
				call_status_ = FINISH;
				reader_->Finish(&status_, (void*)this);
			}
			break;
		case PROCESS:
			if (event_status)
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p received... handling", this));
				handler_(reply_);
				reader_->Read(&reply_, (void*)this);
			}
			else
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p received... finishing", this));
				call_status_ = FINISH;
				reader_->Finish(&status_, (void*)this);
			}
			break;
		case FINISH:
			if (status_.ok())
			{
				logger_->log(logs::info_level, fmts::sprintf(
					"call %p completed successfully", this));
			}
			else
			{
				error_ = error::error(status_.error_message());
				logger_->log(logs::error_level, fmts::sprintf(
					"call %p failed: %s",
					this, status_.error_message().c_str()));
			}
			delete this;
		}
	}

	ReadptrT reader_;

	grpc::Status status_;

	// ctx_ and reader_ need to be kept in memory
	grpc::ClientContext ctx_;

	DATA reply_;

private:
	enum CallStatus { STARTUP, PROCESS, FINISH };

	std::shared_ptr<logs::iLogger> logger_;

	CallStatus call_status_;

	HandlerF handler_;
};

}

#endif // PKG_EGRPC_CLIENT_ASYNC_STREAM_HPP
