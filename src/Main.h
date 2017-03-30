#pragma once

#ifdef WIN32
	#define DLLEXPORT extern "C" __declspec(dllexport)
#else
	#define DLLEXPORT extern "C"
#endif
