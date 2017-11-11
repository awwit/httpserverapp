
#include "Response.h"

#include "../transfer/http2/HPack.h"

#include <random>

namespace HttpServer
{
	void Response::setStatusCode(const Http::StatusCode status) {
		this->status = status;
	}

	bool Response::sendHeaders(const std::vector<std::pair<std::string, std::string> > &additional, const std::chrono::milliseconds &timeout, const bool endStream)
	{
		std::vector<std::pair<std::string, std::string> > headers;

		std::copy(this->headers.begin(), this->headers.end(), std::back_inserter(headers) );
		std::copy(additional.begin(), additional.end(), std::back_inserter(headers) );

		return this->prot->sendHeaders(this->status, headers, timeout, endStream);
	}

	long Response::sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream) const {
		return this->prot->sendData(src, size, timeout, endStream);
	}
}
