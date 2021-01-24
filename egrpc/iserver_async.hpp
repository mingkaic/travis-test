
#ifndef PKG_EGRPC_ISERVER_ASYNC_HPP
#define PKG_EGRPC_ISERVER_ASYNC_HPP

#include <future>

#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/async_stream.h>

#include "logs/logs.hpp"

#include "egrpc/icqueue.hpp"

namespace egrpc
{

struct GrpcCQueue final : public iCQueue
{
	bool next (void** ptr, bool* ok) override
	{
		return cq_.Next(ptr, ok);
	}

	void shutdown (void) override
	{
		cq_.Shutdown();
	}

	grpc::CompletionQueue* get_cq (void) override
	{
		return &cq_;
	}

	grpc::CompletionQueue cq_;
};

struct GrpcServerCQueue final : public iCQueue
{
	GrpcServerCQueue (std::unique_ptr<grpc::ServerCompletionQueue>&& cq) :
		cq_(std::move(cq)) {}

	bool next (void** ptr, bool* ok) override
	{
		return cq_->Next(ptr, ok);
	}

	void shutdown (void) override
	{
		cq_->Shutdown();
	}

	grpc::CompletionQueue* get_cq (void) override
	{
		return cq_.get();
	}

	std::unique_ptr<grpc::ServerCompletionQueue> cq_;
};

// Detached server calls
struct iServerCall
{
	virtual ~iServerCall (void) = default;

	virtual void serve (void) = 0;

	virtual void shutdown (void) = 0;
};

template <typename R>
struct iResponder
{
	virtual ~iResponder (void) = default;

	virtual void finish (const R& res, grpc::Status status, void* tag) = 0;

	virtual void finish_with_error (grpc::Status status, void* tag) = 0;
};

template <typename R>
struct iWriter
{
	virtual ~iWriter (void) = default;

	virtual void write (const R& res, void* tag) = 0;

	virtual void finish (grpc::Status status, void* tag) = 0;
};

template <typename R>
using RespondptrT = std::unique_ptr<iResponder<R>>;

template <typename R>
using WriterptrT = std::unique_ptr<iWriter<R>>;

template <typename R>
using BuildResponderF = std::function<RespondptrT<R>(grpc::ServerContext&)>;

template <typename R>
using BuildWriterF = std::function<WriterptrT<R>(grpc::ServerContext&)>;

}

#endif // PKG_EGRPC_ISERVER_ASYNC_HPP
