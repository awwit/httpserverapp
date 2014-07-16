
#include "Main.h"

#include "Test.h"

#include <fstream>

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

	Utils::rawPairsToStlUnorderedMultimap(params, request->params, request->params_count);
	Utils::rawPairsToStlUnorderedMap(headers, request->headers, request->headers_count);
	Utils::rawPairsToStlUnorderedMultimap(data, request->data, request->data_count);
	Utils::rawFilesInfoToFilesIncoming(files, request->files, request->files_count);

	auto it_cookie = headers.find("Cookie");

	if (headers.end() != it_cookie)
	{
		Utils::parseCookies(it_cookie->second, cookies);
	}

	HttpServer::ServerRequest proc_request {
		HttpServer::Socket(request->socket),
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
		HttpServer::Socket(request->socket),
		std::map<std::string, std::string>()
	};

	std::string absolute_path = proc_request.document_root + proc_request.uri_reference;

	int result = EXIT_SUCCESS;

	std::ifstream file(absolute_path, std::ifstream::binary);

	if (file)
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

	file.close();

	if (proc_response.headers.size() )
	{
		Utils::raw_pair *headers;
		Utils::stlMapToRawPairs(&headers, proc_response.headers);

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