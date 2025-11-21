#include "../Header/basic_defines.h"
#include <sstream>

#include IncludeDown(Header/string_utils.h)

string string_utils::ArgumentsToLine(REF(vector<string>) args) 
{
	if (args.empty()) return {};

	string buf;
	for (const auto& t : args)
		buf += t + "";

	return buf;
}

bool string_utils::HasArgument(REF(vector<string>) args, REF(string) search) {
	for (const auto& arg : args)
		if (arg == search) 
			return true;
	

	return false;
}

std::vector<string> string_utils::ParseArgumentsValues(REF(std::vector<string>) args, REF(string) searched, const char del)
{
	std::vector<string> buf;

	for (const auto& line : args) {
		auto pair = SplitsPair(line, del);
		if (pair.second.empty()) continue; // no valid

		buf.push_back(pair.second);
	}
	return buf;
}

vector<string> string_utils::SplitsArguments(REF(string) line)
{
	if (line.empty()) return {};

	vector<string> buf;
	std::istringstream iss(line);
	string token;

	while (iss >> token) {
		buf.push_back(token);
	}

	return buf;
}

pair<string, string> string_utils::SplitsPair(REF(string) line, char target)
{
	size_t pos = line.find(target);

	/* if not found pos return empty */
	if (pos == string::npos) return {};

	string first = line.substr(0, pos);
	string second = line.substr(pos + 1);

	return { first, second };
}
