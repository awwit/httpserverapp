
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

	Utils::rawPairsToStlUnorderedMultimap(params, request->params, request->params_count);
	Utils::rawPairsToStlUnorderedMap(headers, request->headers, request->headers_count);
	Utils::rawPairsToStlUnorderedMultimap(data, request->data, request->data_count);
	Utils::rawFilesInfoToFilesIncoming(files, request->files, request->files_count);

	HttpServer::ServerRequest proc_request {
		HttpServer::Socket(request->socket),
		std::string(request->method),
		std::string(request->uri_reference),
		std::string(request->document_root),
		params,
		headers,
		data,
		files
	};

	HttpServer::ServerResponse proc_response {
		HttpServer::Socket(request->socket),
		std::map<std::string, std::string>()
	};

	std::string absolute_path = proc_request.document_root + proc_request.uri_reference;

	int result = EXIT_SUCCESS;

	std::ifstream file(absolute_path);

	if (file)
	{
		proc_response.headers["Connection"] = "keep-alive";
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

DLLEXPORT void application_final()
{
	
}