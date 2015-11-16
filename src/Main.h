#pragma once

#include "ServerRequest.h"
#include "ServerResponse.h"
#include "FileIncoming.h"
#include "Utils.h"

#include <string>
#include <unordered_map>
#include <map>

#ifdef WIN32
	#define DLLEXPORT extern "C" __declspec(dllexport)
#else
	#define DLLEXPORT extern "C"
#endif