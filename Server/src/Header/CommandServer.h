#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <memory>
#include <string>
#include <vector>
#include "Server.h"

using std::vector;
using std::string;

#define GUserSocket REF(SOCKET) user

struct ServerApi 
{
    static void SendText(
        GUserSocket, REF(string) text);
    
    template<typename T>
    static void SendStruct(
        GUserSocket, REF(T) data) {
        send(user, reinterpret_cast<const char*>(&data), sizeof(data), 0);
    }
};

struct CommandExecute : ServerApi
{
    CommandExecute(REF(string) _name, REF(string) _command);

    virtual void Execute(
        SOCKET user, 
        REF(string) full_line,
        REF(vector<string>) arguments) = 0;

    PROPERTY(string, name, NULLSTR)
    PROPERTY(string, command, NULLSTR)
};


namespace IOB {
    enum OutputBuffer {
        SERVER,
        FILE,
        CONSOLE
    };
}
struct CommandServer : Server 
{
    virtual bool Start(REF(PORT) port) override;
    virtual bool Stop() override;
    
    virtual void AddCommand(std::unique_ptr<CommandExecute> command);
    template <class T>
    void AddCommand() {
        AddCommand(std::make_unique<T>()); 
    }

    void OutputString(SOCKET user, REF(std::string) str);
private:
    vector<std::unique_ptr<CommandExecute>> commands;
    virtual void HandleClient(REF(SOCKET) client_socket, REF(sockaddr_in) addr) override;
protected:
    IOB::OutputBuffer io = IOB::OutputBuffer::SERVER;
    std::vector<string> last_args;
};

#endif
