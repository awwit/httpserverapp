#pragma once

#include "client/Request.h"
#include "client/Response.h"
#include "transfer/AppRequest.h"
#include "transfer/AppResponse.h"

Socket::Adapter *createSocketAdapter(Transfer::app_request *request, void *addr);
void destroySocketAdapter(Socket::Adapter *adapter);

std::string getClearPath(const std::string &path);

bool initServerObjects(HttpClient::Request *procRequest, HttpClient::Response *procResponse, const Transfer::app_request *request, Socket::Adapter *socket_adapter);
void freeProtocolData(HttpClient::Response *response);

bool isSwitchingProtocols(const HttpClient::Request &request, HttpClient::Response &response);
