#pragma once

#include <thread>
#include <memory>
#include "CommandServer.h"
#include "_threadSource.hpp"

// нахуй ооп
struct ThreadServer 
{
	void StartThread(
		REF(PORT) port);

	void StopThread();
	void Wait();

	template <class T>
	void AddCommand();

	CommandServer* Server();
private:
	void _startThread();

	bool _wait_thread = false;
	std::unique_ptr<ThreadWrapper<std::thread>> _thread = nullptr;
	std::unique_ptr<CommandServer> _server = nullptr;
	PORT _port = 0;
};

template <class T>
inline void ThreadServer::AddCommand() 
{
	Assert(_server != nullptr && "Server not Running");
	_server.get()->AddCommand<T>();
}