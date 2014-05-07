
#include "Test.h"

bool test(HttpServer::ServerRequest &request, HttpServer::ServerResponse &response)
{
	const std::unordered_map<std::string, std::string> &incoming_headers = request.headers;
	const std::unordered_multimap<std::string, std::string> &incoming_data = request.data;

	HttpServer::Socket &socket = response.socket;
	std::map<std::string, std::string> &headers_outgoing = response.headers;

	headers_outgoing[""] = "HTTP/1.1 200 OK";
	headers_outgoing["Content-Type"] = "text/html; charset=utf-8";
	headers_outgoing["Connection"] = "Keep-Alive";
//	headers_outgoing["Content-Type"] = "text/plain; charset=utf-8";
//	headers_outgoing["Access-Control-Allow-Origin"] = "*";

	size_t write_len;

	std::string s;

//	write_len = socket.send(s);

//	s.clear();

	for (auto h = incoming_headers.cbegin(); h != incoming_headers.cend(); ++h)
	{
		s += h->first + ":" + h->second + "\n";
	}

	s += "\n";

	for (auto v = incoming_data.cbegin(); v != incoming_data.cend(); ++v)
	{
		s += v->first + ": " + v->second + "\n";
	}

	/*	s = "\
<table width=\"100%\">\
<tr>\
	<td>1<td>\
	<td>2<td>\
</tr>\
<tr>\
	<td>3<td>\
	<td>4<td>\
</tr>\
</table>";*/

	headers_outgoing["Content-Length"] = std::to_string(s.length() );

	std::string headers;

	for (auto h = headers_outgoing.cbegin(); h != headers_outgoing.cend(); ++h)
	{
		if (h->first.length() )
		{
			headers += h->first + ": ";
		}

		headers += h->second + "\r\n";
	}

	headers += "\r\n";

	headers_outgoing.clear();

	s = headers + s;

	write_len = socket.send(s);

	return EXIT_SUCCESS;
}
