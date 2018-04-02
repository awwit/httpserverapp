#pragma once

#include "server/Request.h"
#include "server/Response.h"
#include "transfer/AppRequest.h"
#include "transfer/AppResponse.h"

Socket::Adapter *createSocketAdapter(
	Transfer::app_request *request,
	void *addr
);

void destroySocketAdapter(Socket::Adapter *adapter);

std::string getClearPath(const std::string &path);

bool initServerObjects(
	HttpServer::Request *procRequest,
	HttpServer::Response *procResponse,
	const Transfer::app_request *request,
	Socket::Adapter *socket_adapter
);

void freeProtocolData(HttpServer::Response *response);

bool isSwitchingProtocols(
	const HttpServer::Request &request,
	HttpServer::Response &response
);
