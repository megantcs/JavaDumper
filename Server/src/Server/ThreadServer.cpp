#include "../Header/basic_defines.h"
#include "../Header/ThreadServer.h"

void ThreadServer::StartThread(REF(PORT) port)
{
	Assert(port != 0);
	Assert(_thread == nullptr);
	Assert(_server == nullptr);

	_port = port;
	_server = std::make_unique<CommandServer>();
	_thread = std::make_unique<ThreadWrapper<std::thread>>([this]() { _startThread(); });
}

void ThreadServer::StopThread()
{
	Assert(_thread != nullptr);
	
	_thread.get()->destroy();
}

void ThreadServer::Wait()
{
	Assert(_thread != nullptr);
	_thread.get()->join();
	_wait_thread = true;
}

CommandServer* ThreadServer::Server()
{
	return _server.get();
}

void ThreadServer::_startThread()
{
	Assert(_port != 0);
	Assert(_server != nullptr);
	Assert(_thread != nullptr);

	_server->Start(_port);
}

