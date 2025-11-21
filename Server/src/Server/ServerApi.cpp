#include "../Header/CommandServer.h"

void ServerApi::SendText(GUserSocket, REF(string) text)
{
	send(user, text.c_str(), text.size(), 0);
}