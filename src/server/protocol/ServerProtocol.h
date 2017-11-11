#pragma once

#include "../../socket/Adapter.h"
#include "../../transfer/HttpStatusCode.h"

namespace HttpServer
{
	class ServerProtocol
	{
	protected:
		Socket::Adapter *sock;

	public:
		ServerProtocol(Socket::Adapter *sock);
		virtual ~ServerProtocol() noexcept = default;

		Socket::Adapter *getSocket() noexcept;

		virtual bool sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream = true) const = 0;
		virtual long sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream = true) const = 0;

		virtual void close() noexcept;
	};
}
