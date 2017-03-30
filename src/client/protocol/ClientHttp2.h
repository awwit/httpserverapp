#pragma once

#include "ClientProtocol.h"

#include "../../transfer/http2/Http2.h"

namespace HttpClient
{
	class ClientHttp2 : public ClientProtocol
	{
	protected:
		Http2::OutStream *stream;

	public:
		ClientHttp2(Socket::Adapter *sock, Http2::OutStream *stream);
		virtual ~ClientHttp2() noexcept;

		void sendWindowUpdate(const uint32_t size, const std::chrono::milliseconds &timeout) const;

		virtual bool sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream = true) const override;
		virtual long sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream = true) const override;
	};
};
