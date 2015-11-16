
#include "Test.h"

bool test(HttpServer::ServerRequest &request, HttpServer::ServerResponse &response)
{
	// Output incoming headers

	std::string s = R"(<table>
	<thead>
		<tr>
			<th colspan="2">Incoming headers</th>
		</tr>
	</thead>
	<tbody>
)";

	for (auto const &pair : request.headers)
	{
		s += R"(		<tr>
			<td>)" + pair.first + R"(</td>
			<td>)" + pair.second + R"(</td>
		</tr>
)";
	}

	// Output incoming data

	s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="2">Incoming data</th>
		</tr>
	</thead>
	<tbody>
)";

	for (auto const &pair : request.data)
	{
		s += R"(		<tr>
			<td>)" + pair.first + R"(</td>
			<td>)" + pair.second + R"(</td>
		</tr>
)";
	}

	// Output incoming url parameters

	s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="2">Incoming url parameters</th>
		</tr>
	</thead>
	<tbody>
)";

	for (auto const &pair : request.params)
	{
		s += R"(		<tr>
				<td>)" + pair.first + R"(</td>
				<td>)" + pair.second + R"(</td>
			</tr>
)";
	}

	// Output info about incoming files

	s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="2">Incoming files</th>
		</tr>
	</thead>
	<tbody>
)";

	for (auto const &pair : request.files)
	{
		const HttpServer::FileIncoming &file = pair.second;

		s += R"(		<tr>
				<td>)" + file.getName() + R"(</td>
				<td>)" + std::to_string(file.getSize() ) + R"(</td>
			</tr>
)";
	}

	s += R"(	</tbody>
</table>)";

	HttpServer::Socket &socket = response.socket;
	std::map<std::string, std::string> &headers_outgoing = response.headers;

	// Set outgoing headers

	headers_outgoing[""] = "HTTP/1.1 200 OK";
	headers_outgoing["Content-Type"] = "text/html; charset=utf-8";
	headers_outgoing["Accept-Ranges"] = "bytes";
	headers_outgoing["Content-Length"] = std::to_string(s.length() );
	headers_outgoing["Connection"] = "Keep-Alive";
	headers_outgoing["Date"] = Utils::getDatetimeAsString();

	std::string headers;

	for (auto const &h : headers_outgoing)
	{
		if (h.first.length() )
		{
			headers += h.first + ": ";
		}

		headers += h.second + "\r\n";
	}

	headers += "\r\n";

	// Send headers and page

	std::chrono::milliseconds timeout(5000);

	socket.nonblock_send(headers, timeout);
	socket.nonblock_send(s, timeout);

	return EXIT_SUCCESS;
}
