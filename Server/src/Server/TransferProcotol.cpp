#include "../Header/Network/TransferProcotol.h"

void TransferProcotol::SendJsonText(SOCKET socket, const std::string& json)
{
	// send package header
	PackageHeader ph
	{
		json.size(),
		PackageAction::ReadStepString
	};


	send(socket, reinterpret_cast<const char*>(&ph), sizeof(ph), 0);
	send(socket, json.c_str(), json.size(), 0);
}
