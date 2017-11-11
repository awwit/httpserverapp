
#include "ServerHttp2.h"

#include "../../transfer/http2/HPack.h"

#include <array>
#include <random>

namespace HttpServer
{
	ServerHttp2::ServerHttp2(Socket::Adapter *sock, Http2::OutStream *stream)
		: ServerProtocol(sock), stream(stream)
	{

	}

	ServerHttp2::~ServerHttp2() noexcept {
		delete this->stream;
	}

	static uint8_t getPaddingSize(const size_t dataSize)
	{
		if (0 == dataSize) {
			return 0;
		}

		std::random_device rd;

		uint8_t padding = rd();

		while (dataSize <= padding) {
			padding /= 2;
		}

		return padding;
	}

	bool ServerHttp2::sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		std::vector<char> buf;
		buf.reserve(4096);
		buf.resize(Http2::FRAME_HEADER_SIZE + sizeof(uint8_t) );

		headers.emplace(headers.begin(), ":status", std::to_string(static_cast<int>(status) ) );

		HPack::pack(buf, headers, this->stream->dynamic_table);

		uint32_t data_size = buf.size() - Http2::FRAME_HEADER_SIZE - sizeof(uint8_t);

		const uint8_t padding = getPaddingSize(data_size);
		const uint16_t padding_size = padding + sizeof(uint8_t);

		if (data_size + padding_size > this->stream->settings.max_frame_size) {
			data_size = this->stream->settings.max_frame_size - padding_size;
		}

		const size_t frame_size = data_size + padding_size;

		buf.resize(frame_size + Http2::FRAME_HEADER_SIZE);

		Http2::FrameFlag flags = Http2::FrameFlag::END_HEADERS;

		if (endStream) {
			flags |= Http2::FrameFlag::END_STREAM;
		}

		flags |= Http2::FrameFlag::PADDED;

		buf[Http2::FRAME_HEADER_SIZE] = padding;

		if (padding) {
			std::fill(buf.end() - padding, buf.end(), 0);
		}

		this->stream->setHttp2FrameHeader(reinterpret_cast<uint8_t *>(buf.data() ), frame_size, Http2::FrameType::HEADERS, flags);

		const std::unique_lock<std::mutex> lock(*this->stream->mtx);

		return this->sock->nonblock_send(buf.data(), buf.size(), timeout) > 0;
	}

	void ServerHttp2::sendWindowUpdate(const uint32_t size, const std::chrono::milliseconds &timeout) const
	{
		std::array<uint8_t, Http2::FRAME_HEADER_SIZE + sizeof(uint32_t)> buf;
		uint8_t *addr = buf.data();

		addr = this->stream->setHttp2FrameHeader(addr, sizeof(uint32_t), Http2::FrameType::WINDOW_UPDATE, Http2::FrameFlag::EMPTY);

		*reinterpret_cast<uint32_t *>(addr) = ::htonl(size);

		const std::unique_lock<std::mutex> lock(*this->stream->mtx);

		this->sock->nonblock_send(buf.data(), buf.size(), timeout);
	}

	long ServerHttp2::sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		const uint8_t *data = reinterpret_cast<const uint8_t *>(src);

		std::vector<uint8_t> buf;
		buf.reserve(this->stream->settings.max_frame_size + Http2::FRAME_HEADER_SIZE);

		size_t total = 0;

		while (total < size)
		{
			size_t data_size = (size - total < this->stream->settings.max_frame_size) ? size - total : this->stream->settings.max_frame_size;

			const uint8_t padding = getPaddingSize(data_size);
			const uint16_t padding_size = padding + sizeof(uint8_t);

			if (data_size + padding_size > this->stream->settings.max_frame_size) {
				data_size = this->stream->settings.max_frame_size - padding_size;
			}

			const size_t frame_size = data_size + padding_size;

			buf.resize(frame_size + Http2::FRAME_HEADER_SIZE);

			if (static_cast<int32_t>(this->stream->window_size_out - this->stream->settings.max_frame_size) <= 0)
			{
				size_t update_size = this->stream->settings.initial_window_size + (size - total) - this->stream->window_size_out;

				if (update_size > Http2::MAX_WINDOW_UPDATE) {
					update_size = Http2::MAX_WINDOW_UPDATE;
				}

				this->sendWindowUpdate(static_cast<uint32_t>(update_size), timeout);

				this->stream->window_size_out += update_size;
			}

			Http2::FrameFlag flags = Http2::FrameFlag::EMPTY;

			if (endStream && (total + data_size >= size) ) {
				flags |= Http2::FrameFlag::END_STREAM;
			}

			size_t cur = Http2::FRAME_HEADER_SIZE;

			if (padding_size) {
				flags |= Http2::FrameFlag::PADDED;
				buf[cur] = padding;
				++cur;
			}

			this->stream->setHttp2FrameHeader(buf.data(), frame_size, Http2::FrameType::DATA, flags);

			std::copy(data, data + data_size, buf.begin() + cur);

			if (padding) {
				std::fill(buf.end() - padding, buf.end(), 0);
			}

			this->stream->lock();

			const long sended = this->sock->nonblock_send(buf.data(), buf.size(), timeout);

			this->stream->unlock();

			if (sended <= 0) {
				total = 0;
				break;
			}

			this->stream->window_size_out -= frame_size;

			data += data_size;
			total += data_size;
		}

		return static_cast<long>(total);
	}
}
