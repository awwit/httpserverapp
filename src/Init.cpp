
#include "Init.h"

#include "socket/AdapterDefault.h"

#include "transfer/FileIncoming.h"
#include "transfer/http2/Http2.h"
#include "utils/Utils.h"

#include "client/protocol/ClientHttp1.h"
#include "client/protocol/ClientHttp2.h"

Socket::Adapter *createSocketAdapter(Transfer::app_request *request, void *addr)
{
	if (request->tls_session)
	{
		return new (addr) Socket::AdapterTls(request->tls_session);
	}

	return new (addr) Socket::AdapterDefault(request->socket);
}

void destroySocketAdapter(Socket::Adapter *adapter)
{
	if (adapter)
	{
		adapter->~Adapter();
	}
}

static void getIncomingVars(std::unordered_multimap<std::string, std::string> &params, const std::string &uri)
{
	const size_t start = uri.find('?');

	if (std::string::npos == start)
	{
		return;
	}

	for (size_t var_pos = start + 1, var_end = 0; std::string::npos != var_end; var_pos = var_end + 1)
	{
		var_end = uri.find('&', var_pos);

		size_t delimiter = uri.find('=', var_pos);

		if (delimiter >= var_end)
		{
			std::string var_name = Utils::urlDecode(uri.substr(var_pos, std::string::npos != var_end ? var_end - var_pos : std::string::npos) );

			params.emplace(std::move(var_name), std::string() );
		}
		else
		{
			std::string var_name = Utils::urlDecode(uri.substr(var_pos, delimiter - var_pos) );

			++delimiter;

			std::string var_value = Utils::urlDecode(uri.substr(delimiter, std::string::npos != var_end ? var_end - delimiter : std::string::npos) );

			params.emplace(std::move(var_name), std::move(var_value) );
		}
	}
}

bool initServerObjects(HttpClient::Request *procRequest, HttpClient::Response *procResponse, const Transfer::app_request *request, Socket::Adapter *socket_adapter)
{
	const uint8_t *src = reinterpret_cast<const uint8_t *>(request->request_data);

	size_t protocol_number;
	src = Utils::unpackNumber(&protocol_number, src);
	Transfer::ProtocolVariant protocol_variant = static_cast<Transfer::ProtocolVariant>(protocol_number);
	HttpClient::ClientProtocol *prot = nullptr;

	std::string document_root;
	std::string host;
	std::string path;
	std::string method;
	std::unordered_multimap<std::string, std::string> params;
	std::unordered_multimap<std::string, std::string> headers;
	std::unordered_multimap<std::string, std::string> data;
	std::unordered_multimap<std::string, Transfer::FileIncoming> files;
	std::unordered_multimap<std::string, std::string> cookies;

	bool success = true;

	switch (protocol_variant)
	{
		case Transfer::ProtocolVariant::HTTP_1:
		{
			src = Utils::unpackString(document_root, src);
			src = Utils::unpackString(host, src);
			src = Utils::unpackString(path, src);
			src = Utils::unpackString(method, src);
			src = Utils::unpackContainer(headers, src);
			src = Utils::unpackContainer(data, src);
			src = Utils::unpackFilesIncoming(files, src);

			auto const it_cookie = headers.find("cookie");

			if (headers.cend() != it_cookie)
			{
				Utils::parseCookies(it_cookie->second, cookies);
			}

			getIncomingVars(params, path);

			prot = new HttpClient::ClientHttp1(socket_adapter);

			break;
		}

		case Transfer::ProtocolVariant::HTTP_2:
		{
			src = Utils::unpackString(document_root, src);
			src = Utils::unpackString(host, src);
			src = Utils::unpackString(path, src);
			src = Utils::unpackString(method, src);

			size_t stream_id;

			src = Utils::unpackNumber(&stream_id, src);

			Http2::ConnectionSettings settings;

			size_t number;

			src = Utils::unpackNumber(&number, src);
			settings.header_table_size = number;
			src = Utils::unpackNumber(&number, src);
			settings.enable_push = number;
			src = Utils::unpackNumber(&number, src);
			settings.max_concurrent_streams = number;
			src = Utils::unpackNumber(&number, src);
			settings.initial_window_size = number;
			src = Utils::unpackNumber(&number, src);
			settings.max_frame_size = number;
			src = Utils::unpackNumber(&number, src);
			settings.max_header_list_size = number;

			std::deque<std::pair<std::string, std::string> > dynamic_table;
			src = Utils::unpackVector(dynamic_table, src);

			src = Utils::unpackContainer(headers, src);
			src = Utils::unpackContainer(data, src);
			src = Utils::unpackFilesIncoming(files, src);

			getIncomingVars(params, path);

			Http2::OutStream *stream = new Http2::OutStream(stream_id, settings, Http2::DynamicTable(settings.header_table_size, settings.max_header_list_size, std::move(dynamic_table) ) );

			prot = new HttpClient::ClientHttp2(socket_adapter, stream);

			break;
		}

		default:
		{
			success = false;
			break;
		}
	}

	*procRequest = HttpClient::Request {
		prot,
		std::move(document_root),
		std::move(host),
		std::move(path),
		std::move(method),
		std::move(params),
		std::move(headers),
		std::move(data),
		std::move(files),
		std::move(cookies),
		protocol_variant
	};

	*procResponse = HttpClient::Response {
		prot,
		protocol_variant,
		std::unordered_map<std::string, std::string>(),
		Http::StatusCode::EMPTY
	};

	return success;
}

void cleanProtocolData(HttpClient::Response *response)
{
	if (response)
	{
		delete response->prot;
	}
}

bool isSwitchingProtocols(const HttpClient::Request &request, HttpClient::Response &response)
{
	if (request.prot->getSocket()->get_tls_session() != 0)
	{
		return false;
	}

	auto const it_upgrade = request.headers.find("upgrade");

	if (request.headers.cend() == it_upgrade)
	{
		return false;
	}

	auto const it_connection = request.headers.find("connection");

	if (request.headers.cend() == it_connection)
	{
		return false;
	}

	std::vector<std::string> list = Utils::explode(it_connection->second, ',');

	bool is_upgrade = false;

	for (auto &item : list)
	{
		Utils::toLower(item);

		if ("upgrade" == item)
		{
			is_upgrade = true;
			break;
		}
	}

	if (false == is_upgrade)
	{
		return false;
	}

	const std::string &upgrade = it_upgrade->second;

	if ("h2c" != upgrade)
	{
		return false;
	}

	auto const it_settings = request.headers.find("http2-settings");

	if (request.headers.cend() == it_settings)
	{
		return false;
	}

	response.headers["connection"] = "upgrade";
	response.headers["upgrade"] = "h2c";

	const std::string headers = "HTTP/1.1 101 Switching Protocols\r\nConnection: Upgrade\r\nUpgrade: h2c\r\n\r\n";

	response.prot->getSocket()->nonblock_send(headers, std::chrono::milliseconds(5000) );

	return true;
}
