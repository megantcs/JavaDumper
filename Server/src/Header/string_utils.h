#pragma once

#include <vector>
#include "basic_defines.h"

using std::string;
using std::vector;
using std::pair;

namespace string_utils 
{
	vector<string>			SplitsArguments(REF(string) line);
	string					ArgumentsToLine(REF(vector<string>) args);

	pair<string, string>	SplitsPair(
		REF(string) line, 
		char target);

	bool					HasArgument(REF(vector<string>) args, REF(string) search);
	std::vector<string>		ParseArgumentsValues(REF(std::vector<string>) args, REF(string) searched, const char del);
}