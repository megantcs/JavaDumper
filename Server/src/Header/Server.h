#ifndef SERVER_H
#define SERVER_H

#include "basic_defines.h"
#include "Logger.h"

#include "WinSocketHeader.h"

typedef unsigned int PORT;

enum ServerState
{
    Stop = 0,
    Running = 1,
    Wait = 2
};

interface Server 
{
    /*  USER METHOD  */
	virtual CHECK_RETURN bool 
    Start(REF(PORT) port) = 0;

    virtual CHECK_RETURN bool
    Stop() = 0;
protected:
    /*  PRIVATE METHOD  */
    virtual void 
    HandleClient(
        REF(SOCKET) client_socket,
        REF(sockaddr_in) client_addr) = 0;

protected:
    /* PRIVATE FIELDS   */
    ServerState server_state = ServerState::Wait;
    SOCKET server_socket = 0;
    PORT server_port = 0;
};

#endif

