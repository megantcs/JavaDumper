#pragma once
#include <string>

namespace Logger 
{
	void Log(const std::string &msg);
	void Error(const std::string& msg);
	void Debug(const std::string& msg);
}