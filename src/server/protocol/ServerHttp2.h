#pragma once

#include "ServerProtocol.h"

#include "../../transfer/http2/Http2.h"

namespace HttpServer
{
	class ServerHttp2 : public ServerProtocol
	{
	protected:
		Http2::OutStream *stream;

	public:
		ServerHttp2(
			Socket::Adapter *sock,
			Http2::OutStream *stream
		) noexcept;

		virtual ~ServerHttp2() noexcept override;

		void sendWindowUpdate(
			const uint32_t size,
			const std::chrono::milliseconds &timeout
		) const;

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
