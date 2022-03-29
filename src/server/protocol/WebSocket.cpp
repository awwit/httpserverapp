
#include "WebSocket.h"

#include "../../utils/Utils.h"
#include "../../socket/Socket.h"

#ifdef POSIX
	#include <arpa/inet.h>
#endif

namespace HttpClient
{
	WebSocket::WebSocket(const WebSocket &obj) noexcept : sock(obj.sock) {}

	WebSocket::WebSocket(Socket::Adapter *adapter) noexcept : sock(adapter) {}

	std::vector<char> WebSocket::packDataToMessageFrame(const void *data, const size_t size)
	{
		std::vector<char> frame;

		if (0 == size) {
			return frame;
		}

		constexpr size_t header_max_size = 14;
		frame.reserve(size + header_max_size);
		frame.resize(header_max_size);

		frame[0] = 0x81;

		size_t cur_pos = sizeof(uint8_t) * 2;

		if (size <= 125) {
			frame[1] = size;
		}
		else if (size <= 65536) {
			frame[1] = 126;

			*reinterpret_cast<uint16_t *>(&frame[2]) = htons(size);

			cur_pos += sizeof(uint16_t);
		} else { // More
			frame[1] = 127;

			*reinterpret_cast<uint64_t *>(&frame[2]) = Utils::hton64(size);

			cur_pos += sizeof(uint64_t);
		}

		frame.erase(frame.cbegin() + cur_pos, frame.cend() );

		frame.insert(frame.cend(), reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);

		return frame;
	}

	Socket::Adapter *WebSocket::getSocket() noexcept {
		return this->sock;
	}

	const Socket::Adapter *WebSocket::getSocket() const noexcept {
		return this->sock;
	}

	long WebSocket::nonblock_recv(std::vector<char> &frame, const std::chrono::milliseconds &timeout) const
	{
		std::vector<char> buf(4096);

		const long recv_size = this->sock->nonblock_recv(buf, timeout);

		if (recv_size <= 0) {
			return recv_size;
		}

		// See @link: http://tools.ietf.org/html/rfc6455#page-28

		const uint8_t info_frame = buf[0];

		if ( (info_frame & 0x08) == 0x08) { // opcode 0x08 — close connection
			return -1;
		}

		uint8_t info_size = buf[1];

		const bool is_mask_set = info_size & uint8_t(1 << 7);

		info_size &= ~uint8_t(1 << 7); // Unset first bit

		size_t cur_pos = sizeof(uint8_t) * 2;

		uint64_t frame_size;

		if (info_size <= 125) {
			frame_size = info_size;
		}
		else if (info_size == 126) {
			frame_size = ntohs(*reinterpret_cast<uint16_t *>(&buf[cur_pos]) );
			cur_pos += sizeof(uint16_t);
		} else { // if (info_size == 127)
			frame_size = Utils::ntoh64(*reinterpret_cast<uint64_t *>(&buf[cur_pos]) );
			cur_pos += sizeof(uint64_t);
		}

		if (frame_size > (buf.size() - cur_pos) ) {
			return -1; // Close connection
		}

		uint32_t mask;

		if (is_mask_set) {
			mask = *reinterpret_cast<uint32_t *>(&buf[cur_pos]);
			cur_pos += sizeof(uint32_t);
		}

		frame.reserve(recv_size - cur_pos);

		frame.assign(buf.cbegin() + cur_pos, buf.cbegin() + recv_size);

		if (is_mask_set) {
			const size_t aligned = frame.size() - (frame.size() % sizeof(mask));

			uint32_t * const addr = reinterpret_cast<uint32_t *>(frame.data() );

			for (size_t i = 0; i < aligned / sizeof(uint32_t); ++i) {
				addr[i] ^= mask;
			}

			for (size_t i = aligned; i < frame.size(); ++i) {
				frame[i] ^= reinterpret_cast<char *>(&mask)[i % sizeof(mask)];
			}
		}

		return recv_size - cur_pos;
	}

	long WebSocket::nonblock_send(const void *data, const size_t length, const std::chrono::milliseconds &timeout) const
	{
		const std::vector<char> frame = WebSocket::packDataToMessageFrame(data, length);

		if (frame.empty() ) {
			return 0;
		}

		return this->sock->nonblock_send(frame.data(), frame.size(), timeout);
	}

	long WebSocket::nonblock_send(const std::string &str, const std::chrono::milliseconds &timeout) const
	{
		const std::vector<char> frame = WebSocket::packDataToMessageFrame(str.data(), str.length() );

		if (frame.empty() ) {
			return 0;
		}

		return this->sock->nonblock_send(frame.data(), frame.size(), timeout);
	}

	void WebSocket::close() noexcept {
		this->sock->close();
	}
}
