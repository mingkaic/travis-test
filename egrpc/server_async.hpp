
#ifndef PKG_EGRPC_SERVER_ASYNC_HPP
#define PKG_EGRPC_SERVER_ASYNC_HPP

#include "egrpc/iserver_async.hpp"

namespace egrpc
{

template <typename R>
struct GrpcResponder final : public iResponder<R>
{
	GrpcResponder (grpc::ServerContext& ctx) :
		responder_(&ctx) {}

	void finish (const R& res, grpc::Status status, void* tag) override
	{
		responder_.Finish(res, status, tag);
	}

	void finish_with_error (grpc::Status status, void* tag) override
	{
		responder_.FinishWithError(status, tag);
	}

	grpc::ServerAsyncResponseWriter<R> responder_;
};

// Async server request response call
template <typename REQ, typename RES>
struct AsyncServerCall final : public iServerCall
{
	using RequestF = std::function<void(grpc::ServerContext*,REQ*,
		iResponder<RES>&,iCQueue&,void*)>;

	using WriteF = std::function<grpc::Status(const REQ&,RES&)>;

	AsyncServerCall (std::shared_ptr<logs::iLogger> logger,
		RequestF req_call, WriteF write_call, iCQueue& cq,
		BuildResponderF<RES> make_responder =
		[](grpc::ServerContext& ctx) -> RespondptrT<RES>
		{
			return std::make_unique<GrpcResponder<RES>>(ctx);
		}) : logger_(logger),
		cq_(&cq), responder_(make_responder(ctx_)),
		responder_builder_(make_responder), done_(false),
		req_call_(req_call), write_call_(write_call)
	{
		req_call_(&ctx_, &req_, *responder_, *cq_, (void*) this);
		logger_->log(logs::info_level, fmts::sprintf("rpc %p created", this));
	}

	void serve (void) override
	{
		if (done_)
		{
			logger_->log(logs::info_level, fmts::sprintf("rpc %p completed", this));
			shutdown();
		}
		else
		{
			new AsyncServerCall(logger_, req_call_, write_call_, *cq_, responder_builder_);
			logger_->log(logs::info_level, fmts::sprintf("rpc %p writing", this));
			RES reply;
			done_ = true;
			auto status = write_call_(req_, reply);
			if (status.ok())
			{
				responder_->finish(reply, status, this);
			}
			else
			{
				responder_->finish_with_error(status, this);
			}
		}
	}

	void shutdown (void) override
	{
		delete this;
	}

private:
	std::shared_ptr<logs::iLogger> logger_;

	grpc::ServerContext ctx_;

	iCQueue* cq_;

	REQ req_;

	RespondptrT<RES> responder_;

	BuildResponderF<RES> responder_builder_;

	bool done_ = false;

	RequestF req_call_;

	WriteF write_call_;
};

}

#endif // PKG_EGRPC_SERVER_ASYNC_HPP
