
#include "Test.h"

#include "../utils/Utils.h"

namespace Application
{
	bool test(HttpServer::Request &request, HttpServer::Response &response)
	{
		// Output incoming headers

		std::string s = R"(<table>
	<thead>
		<tr>
			<th colspan="2">Incoming headers</th>
		</tr>
		<tr>
			<th>Header name</th>
			<th>Header value</th>
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

		// Output incoming url parameters

		s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="2">Incoming url parameters</th>
		</tr>
		<tr>
			<th>Parameter name</th>
			<th>Parameter value</th>
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

		// Output incoming form data

		s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="2">Incoming form data</th>
		</tr>
		<tr>
			<th>Form field</th>
			<th>Field value</th>
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

		// Output info about incoming files

		s += R"(	</tbody>
	<thead>
		<tr>
			<th colspan="5">Incoming files</th>
		</tr>
		<tr>
			<th>Form field</th>
			<th>Original name</th>
			<th>File type</th>
			<th>File size</th>
			<th>Uploaded name</th>
		</tr>
	</thead>
	<tbody>
)";

		for (auto const &pair : request.files)
		{
			const std::string &field_name = pair.first;
			const Transfer::FileIncoming &file = pair.second;

			s += R"(		<tr>
				<td>)" + field_name + R"(</td>
				<td>)" + file.getName() + R"(</td>
				<td>)" + file.getType() + R"(</td>
				<td>)" + std::to_string(file.getSize() ) + R"(</td>
				<td>)" + file.getTmpName() + R"(</td>
			</tr>
)";
		}

		s += R"(	</tbody>
</table>)";

		std::unordered_map<std::string, std::string> &headers = response.headers;

		// Set outgoing headers

		response.setStatusCode(Http::StatusCode::OK);
		headers["content-type"] = "text/html; charset=utf-8";
		headers["accept-ranges"] = "bytes";
		headers["content-length"] = std::to_string(s.size() );
		headers["connection"] = "keep-alive";
		headers["date"] = Utils::getDatetimeAsString();

		// Additional headers
		// In additional headers may be placed values of cookies
		std::vector<std::pair<std::string, std::string> > additional;
		// additional.emplace_back("set-cookie", "key=value; expires=; path=/; domain=*");

		// Send headers and data

		const std::chrono::milliseconds timeout(5000);

		if (response.sendHeaders(additional, timeout, s.empty() ) ) {
			if (s.empty() == false) {
				response.sendData(s.data(), s.size(), timeout, true);
			}
		}

		return EXIT_SUCCESS;
	}
}
