
#include "Main.h"

#include "Test.h"

DLLEXPORT bool application_init()
{
	return true;
}

DLLEXPORT int application_call(HttpServer::server_request *request, HttpServer::server_response *response)
{
	std::unordered_multimap<std::string, std::string> params;
	std::unordered_map<std::string, std::string> headers;
	std::unordered_multimap<std::string, std::string> data;
	std::unordered_multimap<std::string, HttpServer::FileIncoming> files;
	std::unordered_multimap<std::string, std::string> cookies;

	Utils::rawPairsToStl(params, request->params, request->params_count);
	Utils::rawPairsToStl(headers, request->headers, request->headers_count);
	Utils::rawPairsToStl(data, request->data, request->data_count);
	Utils::rawFilesInfoToFilesIncoming(files, request->files, request->files_count);

	auto it_cookie = headers.find("Cookie");

	if (headers.cend() != it_cookie)
	{
		Utils::parseCookies(it_cookie->second, cookies);
	}

	// Create socket adapter
	uint8_t addr[sizeof(HttpServer::SocketAdapterTls)];

	HttpServer::SocketAdapter *socket_adapter;

	if (request->tls_session)
	{
		socket_adapter = new (addr) HttpServer::SocketAdapterTls(request->tls_session);
	}
	else
	{
		socket_adapter = new (addr) HttpServer::SocketAdapterDefault(request->socket);
	}

	HttpServer::ServerRequest proc_request {
		*socket_adapter,
		std::string(request->method),
		std::string(request->uri_reference),
		std::string(request->document_root),
		std::move(params),
		std::move(headers),
		std::move(data),
		std::move(files),
		std::move(cookies)
	};

	HttpServer::ServerResponse proc_response {
		*socket_adapter,
		std::map<std::string, std::string>()
	};

	const std::string absolute_path = proc_request.document_root + proc_request.uri_reference;

	int result = EXIT_SUCCESS;

	if (std::string::npos == absolute_path.find("/../") && System::isFileExists(absolute_path) )
	{
		auto it_connection = proc_request.headers.find("Connection");

		if (proc_request.headers.cend() != it_connection)
		{
			proc_response.headers["Connection"] = it_connection->second;
		}

		proc_response.headers["X-Sendfile"] = absolute_path;
	}
	else
	{
		result = test(proc_request, proc_response);
	}

	socket_adapter->~SocketAdapter();

	if (proc_response.headers.size() )
	{
		Utils::raw_pair *headers;
		Utils::stlToRawPairs(&headers, proc_response.headers);

		response->headers_count = proc_response.headers.size();
		response->headers = headers;
	}

	return result;
}

DLLEXPORT void application_clear(Utils::raw_pair headers[], const size_t headers_count)
{
	if (headers && headers_count)
	{
		destroyRawPairs(headers, headers_count);
	}
}

DLLEXPORT void application_final()
{
	
}