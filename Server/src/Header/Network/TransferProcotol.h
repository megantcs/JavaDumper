#pragma once

#include "../WinSocketHeader.h"
#include <Package/JDSStructHeader.hpp>

#include <string>

namespace TransferProcotol 
{
	void SendJsonText(SOCKET socket, const std::string& json);
}