
#include "ClientHttp1.h"

namespace HttpClient
{
	ClientHttp1::ClientHttp1(Socket::Adapter *sock) : ClientProtocol(sock)
	{

	}

	bool ClientHttp1::sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		std::string out = "HTTP/1.1 " + std::to_string(static_cast<int>(status) ) + "\r\n";

		for (auto const &h : headers)
		{
			out += h.first + ": " + h.second + "\r\n";
		}

		out += "\r\n";

		return this->sock->nonblock_send(out, timeout) > 0;
	}

	long ClientHttp1::sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		return this->sock->nonblock_send(src, size, timeout);
	}
};
