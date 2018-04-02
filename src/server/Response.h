#pragma once

#include "protocol/ServerProtocol.h"
#include "../transfer/ProtocolVariant.h"
#include "../transfer/HttpStatusCode.h"

#include <unordered_map>
#include <string>

namespace HttpServer
{
	struct Response
	{
		ServerProtocol *prot;
		Transfer::ProtocolVariant protocol_variant;
		std::unordered_map<std::string, std::string> headers;
		Http::StatusCode status;

	public:
		void setStatusCode(const Http::StatusCode status) noexcept;

		bool sendHeaders(
			const std::vector<std::pair<std::string, std::string> > &additional,
			const std::chrono::milliseconds &timeout,
			const bool endStream = true
		) const;

		long sendData(
			const void *src,
			const size_t size,
			const std::chrono::milliseconds &timeout,
			const bool endStream = true
		) const noexcept;
	};
}
