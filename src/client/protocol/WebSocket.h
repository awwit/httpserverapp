#pragma once

#include "../../socket/Adapter.h"

namespace HttpClient
{
	class WebSocket
	{
	private:
		Socket::Adapter *sock;

	public:
		static std::vector<char> packDataToMessageFrame(const void *data, const size_t size);

	public:
		WebSocket() = delete;
		WebSocket(const WebSocket &obj) noexcept;
		WebSocket(Socket::Adapter *adapter) noexcept;

		Socket::Adapter *getSocket() noexcept;
		const Socket::Adapter *getSocket() const noexcept;

		long nonblock_recv(std::vector<char> &frame, const std::chrono::milliseconds &timeout) const;

		long nonblock_send(const void *data, const size_t length, const std::chrono::milliseconds &timeout) const;
		long nonblock_send(const std::string &str, const std::chrono::milliseconds &timeout) const;

		void close() noexcept;
	};
};
