#include "../Header/basic_defines.h"
#include <fstream>

#include IncludeDown(Header/Network/NetworkHandler.hpp)
#include IncludeDown(Header/string_utils.h)
#include IncludeDown(Header/CommandServer.h)

CommandExecute::CommandExecute(REF(string) _name, REF(string) _command) : name(_name), command(_command)
{

}

bool CommandServer::Start(REF(PORT) port)
{
    Assert(port != 0 && "Port cannot be 0");
    Assert(port <= 65535 && "Port must be <= 65535");
    Assert(port > 1023 && "System ports (1-1023) require administrator privileges");

    /* WSA DATA*/
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    /* set field */
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_port = port;

    /* create socket */
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_socket, (sockaddr*)&addr, sizeof(addr));
    listen(server_socket, SOMAXCONN);

    /* marking the server as running */
    server_state = ServerState::Running;

    /* server loop */
    while(server_state == ServerState::Running) 
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(server_socket, &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        int result = select(0, &readSet, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(server_socket, &readSet)) {
            sockaddr_in client_addr;
            int client_size = sizeof(client_addr);
            SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_size);

            if (client_socket != INVALID_SOCKET) {
                HandleClient(client_socket, client_addr);
            }
        }
    }

    return true;
}

bool CommandServer::Stop()
{
    // if server not running return false
    if (server_state == ServerState::Stop 
        || server_state == ServerState::Wait) return false;

    // mark server how stop
    server_state = ServerState::Stop;

    if (server_socket != 0 && server_socket != INVALID_SOCKET) {
        closesocket(server_socket);
        server_socket = 0;
    }
    Logger::Log("Shutdown");
    return true;
} 

void CommandServer::AddCommand(std::unique_ptr<CommandExecute> command)
{
    Assert(command != nullptr && "Command cannot be null");
    commands.push_back(std::move(command));
}

// пока хардкод потом мб исправлю руки дойдут
void CommandServer::OutputString(SOCKET user, REF(std::string) str)
{
    if (io == IOB::OutputBuffer::SERVER)
    {
        ServerApi::SendText(user, str);
    }
    if (io == IOB::OutputBuffer::FILE)
    {
        auto paths = string_utils::ParseArgumentsValues(last_args, "of", '=');
        if (paths.empty()) {
            Logger::Error("the required argument 'of=file' is missing");
            return;
        }

        auto path = paths[0];
        std::ofstream f(path);
        f << str;
    }
    if (io == IOB::OutputBuffer::CONSOLE)
    {
        printf("~   %s\n", str.c_str());
    }
}

void CommandServer::HandleClient(REF(SOCKET) client_socket, REF(sockaddr_in) addr)
{
    char buffer[1024];
    int bytes = recv(client_socket, buffer, sizeof(buffer), 0);

    // get bytes
    if (bytes > 0) {
        buffer[bytes] = '\0';
        string line = buffer;

        // is curl call
        if (NetworkHandler::IsCurlCall(line)) {
            Logger::Log("[bufer is curl call]");
            line = NetworkHandler::ExtractCurlMessage(line);
        }
        const vector<string> args = string_utils::SplitsArguments(line);
        last_args = args;

        Logger::Log("message: " + line);

        /* if empty command, then exit */
        if (args.empty()) return;
        bool user_exit = string_utils::HasArgument(args, "?ux");
        if (string_utils::HasArgument(args, "?iof")) { /* требуется отдельно указать аргументом of=<path>*/
            this->io = IOB::OutputBuffer::FILE;
        }
        if (string_utils::HasArgument(args, "?ioc")) {
            this->io = IOB::OutputBuffer::CONSOLE;
        }
        if (string_utils::HasArgument(args, "?ios")) {
            this->io = IOB::OutputBuffer::SERVER;
        }

        // stop command (temporarily)
        if (args[0] == "/stop") {
            Stop();
            goto error;
        }

        for (REF(auto) command_ptr : commands) 
        {
            auto command = command_ptr.get();
            if (args.front() == command->get_command()) {
                command->Execute(client_socket, line, args);
                break; // exit
            }
        }

        if (user_exit) goto exit;
    }
    return;

error: /* send echo with error */
    send(client_socket, buffer, bytes, 0);
exit: /* close user socket */
    closesocket(client_socket);
}
