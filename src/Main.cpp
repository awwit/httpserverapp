
#include "Main.h"
#include "Init.h"

#include "application/Test.h"

#include "utils/Utils.h"

EXPORT bool application_init(const char *root)
{
	return true;
}

EXPORT int application_call(Transfer::app_request *request, Transfer::app_response *response)
{
	// Allocate memory on the stack
	uint8_t addr[sizeof(Socket::AdapterTls)];

	// Create the socket adapter
	Socket::Adapter *socket_adapter = createSocketAdapter(request, addr);

	HttpClient::Request proc_request;
	HttpClient::Response proc_response;

	if (false == initServerObjects(&proc_request, &proc_response, request, socket_adapter) == false) {
		return EXIT_FAILURE;
	}

	const std::string absolute_path = proc_request.document_root + getClearPath(proc_request.path);

	int result = EXIT_SUCCESS;

	if (isSwitchingProtocols(proc_request, proc_response) )
	{

	}
	else if (std::string::npos == absolute_path.find("/../") && System::isFileExists(absolute_path) )
	{
		auto it_connection = proc_request.headers.find("connection");

		if (proc_request.headers.cend() != it_connection)
		{
			proc_response.headers["connection"] = it_connection->second;
		}

		proc_response.headers["x-sendfile"] = absolute_path;
	}
	else
	{
		// Call application
		result = Application::test(proc_request, proc_response);
	}

	destroySocketAdapter(socket_adapter);

	if (proc_response.headers.size() ) {
		response->data_size = Utils::getPackContainerSize(proc_response.headers);
		response->response_data = new uint8_t[response->data_size];
		Utils::packContainer(reinterpret_cast<uint8_t *>(response->response_data), proc_response.headers);
	}

	freeProtocolData(&proc_response);

	return result;
}

EXPORT void application_clear(void *response_data, size_t response_size)
{
	if (response_data && response_size) {
		delete[] reinterpret_cast<uint8_t *>(response_data);
	}
}

EXPORT void application_final(const char *root)
{
	
}
