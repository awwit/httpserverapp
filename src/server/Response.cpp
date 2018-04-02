
#include "Response.h"

#include "../transfer/http2/HPack.h"

#include <random>

namespace HttpServer
{
	void Response::setStatusCode(
		const Http::StatusCode status
	) noexcept {
		this->status = status;
	}

	bool Response::sendHeaders(
		const std::vector<std::pair<std::string, std::string> > &additional,
		const std::chrono::milliseconds &timeout,
		const bool endStream
	) const {
		std::vector<std::pair<std::string, std::string> > headers;

		headers.reserve(
			this->headers.size() + additional.size()
		);

		headers.insert(
			headers.end(),
			this->headers.cbegin(),
			this->headers.cend()
		);

		headers.insert(
			headers.end(),
			additional.cbegin(),
			additional.cend()
		);

		return this->prot->sendHeaders(
			this->status,
			headers,
			timeout,
			endStream
		);
	}

	long Response::sendData(
		const void *src,
		const size_t size,
		const std::chrono::milliseconds &timeout,
		const bool endStream
	) const noexcept {
		return this->prot->sendData(
			src,
			size,
			timeout,
			endStream
		);
	}
}
