
#include "Test.h"

bool test(HttpServer::ServerRequest &request, HttpServer::ServerResponse &response)
{
	const std::unordered_map<std::string, std::string> &incoming_headers = request.headers;
	const std::unordered_multimap<std::string, std::string> &incoming_data = request.data;

	HttpServer::Socket &socket = response.socket;
	std::map<std::string, std::string> &headers_outgoing = response.headers;

	std::string s;

	for (auto h = incoming_headers.cbegin(); h != incoming_headers.cend(); ++h)
	{
		s += h->first + ": " + h->second + '\n';
	}

	s += '\n';

	for (auto v = incoming_data.cbegin(); v != incoming_data.cend(); ++v)
	{
		s += v->first + ": " + v->second + '\n';
	}

	s += '\n';

	for (auto &p : request.params)
	{
		s += p.first + ": " + p.second + '\n';
	}

		s = R"(
<table width="100%">
<tr>
	<td>1<td>
	<td>2<td>
</tr>
<tr>
	<td>3<td>
	<td>4<td>
</tr>
</table>)";

	headers_outgoing[""] = "HTTP/1.1 200 OK";
	headers_outgoing["Content-Type"] = "text/plain; charset=utf-8";
//	headers_outgoing["Access-Control-Allow-Origin"] = "*";
//	headers_outgoing["Content-Type"] = "text/html; charset=utf-8";
	headers_outgoing["Content-Length"] = std::to_string(s.length() );
	headers_outgoing["Connection"] = "keep-alive";
	headers_outgoing["Date"] = Utils::getDatetimeStringValue();

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

//	s = headers + s;

	std::chrono::milliseconds timeout(5000);

	socket.nonblock_send(headers, timeout);

	if ( (size_t)~0 == socket.nonblock_send(s, timeout) )
	{
		socket.close();
	}

	return EXIT_SUCCESS;
}
