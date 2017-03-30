
#include "ClientHttp2.h"

#include "../../transfer/http2/HPack.h"

#include <random>

namespace HttpClient
{
	ClientHttp2::ClientHttp2(Socket::Adapter *sock, Http2::OutStream *stream)
		: ClientProtocol(sock), stream(stream)
	{

	}

	ClientHttp2::~ClientHttp2() noexcept
	{
		delete this->stream;
	}

	static uint8_t getPaddingSize(const size_t dataSize)
	{
		if (0 == dataSize)
		{
			return 0;
		}

		std::random_device rd;

		uint8_t padding = rd();

		while (dataSize <= padding)
		{
			padding /= 2;
		}

		return padding;
	}

	bool ClientHttp2::sendHeaders(const Http::StatusCode status, std::vector<std::pair<std::string, std::string> > &headers, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		std::vector<char> buf;
		buf.reserve(4096);

		headers.emplace(headers.begin(), ":status", std::to_string(static_cast<int>(status) ) );

		HPack::pack(buf, headers, this->stream->dynamic_table);

		uint32_t data_size = buf.size();

		const uint8_t padding = getPaddingSize(data_size);
		const uint16_t padding_size = padding + sizeof(uint8_t);

		if (padding_size)
		{
			if (data_size + padding_size > this->stream->settings.max_frame_size)
			{
				data_size = this->stream->settings.max_frame_size - padding_size;
			}
		}

		const size_t frame_size = data_size + padding_size;

		Http2::FrameFlag flags = Http2::FrameFlag::END_HEADERS;

		if (endStream)
		{
			flags |= Http2::FrameFlag::END_STREAM;
		}

		if (padding_size)
		{
			flags |= Http2::FrameFlag::PADDED;

			buf.insert(buf.begin(), sizeof(uint8_t), padding);

			if (padding)
			{
				buf.insert(buf.end(), padding, 0);
			}
		}

		buf.insert(buf.begin(), Http2::FRAME_HEADER_SIZE, 0);

		this->stream->setHttp2FrameHeader(reinterpret_cast<uint8_t *>(buf.data() ), frame_size, Http2::FrameType::HEADERS, flags);

		return this->sock->nonblock_send(buf.data(), buf.size(), timeout) > 0;
	}

	void ClientHttp2::sendWindowUpdate(const uint32_t size, const std::chrono::milliseconds &timeout) const
	{
		std::array<uint8_t, Http2::FRAME_HEADER_SIZE + sizeof(uint32_t)> buf;
		uint8_t *addr = buf.data();

		addr = this->stream->setHttp2FrameHeader(addr, sizeof(uint32_t), Http2::FrameType::WINDOW_UPDATE, Http2::FrameFlag::EMPTY);

		*reinterpret_cast<uint32_t *>(addr) = ::htonl(size);

		this->sock->nonblock_send(buf.data(), buf.size(), timeout);
	}

	long ClientHttp2::sendData(const void *src, const size_t size, const std::chrono::milliseconds &timeout, const bool endStream) const
	{
		const uint8_t *data = reinterpret_cast<const uint8_t *>(src);

		std::vector<uint8_t> buf;
		buf.reserve(this->stream->settings.max_frame_size + Http2::FRAME_HEADER_SIZE);

		size_t total = 0;

		while (total < size)
		{
			buf.resize(0);

			size_t data_size = (size - total < this->stream->settings.max_frame_size) ? size - total : this->stream->settings.max_frame_size;

			const uint8_t padding = getPaddingSize(data_size);
			const uint16_t padding_size = padding + sizeof(uint8_t);

			if (padding_size)
			{
				if (data_size + padding_size > this->stream->settings.max_frame_size)
				{
					data_size = this->stream->settings.max_frame_size - padding_size;
				}
			}

			const size_t frame_size = data_size + padding_size;

			if (static_cast<int32_t>(this->stream->window_size_out - this->stream->settings.max_frame_size) <= 0)
			{
				size_t update_size = this->stream->settings.initial_window_size + (size - total) - this->stream->window_size_out;

				if (update_size > Http2::MAX_WINDOW_UPDATE)
				{
					update_size = Http2::MAX_WINDOW_UPDATE;
				}

				this->sendWindowUpdate(static_cast<uint32_t>(update_size), timeout);

				this->stream->window_size_out += update_size;
			}

			Http2::FrameFlag flags = Http2::FrameFlag::EMPTY;

			if (endStream && (total + data_size >= size) )
			{
				flags |= Http2::FrameFlag::END_STREAM;
			}

			if (padding_size)
			{
				flags |= Http2::FrameFlag::PADDED;

				buf.insert(buf.begin(), sizeof(uint8_t), padding);
			}

			buf.insert(buf.begin(), Http2::FRAME_HEADER_SIZE, 0);

			const Http2::FrameType frame_type = Http2::FrameType::DATA;

			this->stream->setHttp2FrameHeader(buf.data(), frame_size, frame_type, flags);

			std::copy(data, data + data_size, std::back_inserter(buf) );

			if (padding)
			{
				buf.insert(buf.end(), padding, 0);
			}

			long sended = this->sock->nonblock_send(buf.data(), buf.size(), timeout);

			if (sended <= 0)
			{
				break;
			}

			this->stream->window_size_out -= frame_size;

			data += data_size;
			total += data_size;
		}

		return total;
	}
};
