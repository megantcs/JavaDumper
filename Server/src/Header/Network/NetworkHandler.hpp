#pragma once
#include "../basic_defines.h"

struct NetworkHandler : AllStatic 
{
	static bool IsCurlCall(REF(std::string) message);
	static std::string ExtractCurlMessage(REF(std::string) curlMessage);
};