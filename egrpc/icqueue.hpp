
#include <grpcpp/grpcpp.h>

#ifndef PKG_EGRPC_ICQUEUE_HPP
#define PKG_EGRPC_ICQUEUE_HPP

namespace egrpc
{

struct iCQueue
{
	virtual ~iCQueue (void) = default;

	virtual bool next (void** ptr, bool* ok) = 0;

	virtual void shutdown (void) = 0;

	virtual grpc::CompletionQueue* get_cq (void) = 0;
};

}

#endif // PKG_EGRPC_ICQUEUE_HPP
