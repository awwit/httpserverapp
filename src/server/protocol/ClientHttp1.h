#pragma once

#include "ClientProtocol.h"

namespace HttpClient
{
	class ClientHttp1 : public ClientProtocol
	{
	public:
		ClientHttp1(Socket::Adapter *sock);

		virtual bool sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream = true) const override;
		virtual long sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream = true) const override;
	};
}
