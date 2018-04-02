#pragma once

#include "ServerProtocol.h"

namespace HttpServer
{
	class ServerHttp1 : public ServerProtocol
	{
	public:
		ServerHttp1(Socket::Adapter *sock) noexcept;

		virtual bool sendHeaders(
			const Http::StatusCode status,
			std::vector<std::pair<std::string, std::string> > &headers,
			const std::chrono::milliseconds &timeout,
			const bool endStream = true
		) const override;

		virtual long sendData(
			const void *src,
			const size_t size,
			const std::chrono::milliseconds &timeout,
			const bool endStream = true
		) const noexcept override;
	};
}
