#include "../Header/basic_defines.h"
#include <stdio.h>


#include IncludeDown(Header/Logger.h)

void Logger::Log(const std::string& msg) 
{
#ifdef USE_DEBUG
	printf("[SERVER] [LOG] %s\n", msg.c_str());
#endif
}

void Logger::Error(const std::string& msg)
{
#ifdef USE_DEBUG
	printf("[SERVER] [ERR] %s\n", msg.c_str());
#endif
}

void Logger::Debug(const std::string& msg)
{
#ifdef USE_DEBUG
	printf("[SERVER] [Debug] %s\n", msg.c_str());
#endif
}
