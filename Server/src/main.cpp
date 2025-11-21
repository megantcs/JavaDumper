#define _CRT_SECURE_NO_WARNINGS

#include "Header/CommandServer.h"
#include "Header/ThreadServer.h"
#include "Header/string_utils.h"

#include <csignal>
#include <Package/JDSStructHeader.hpp>

#include "Jni/JNIDumperStruct.h"
#include "Jni/SerializerClass.hpp"

#include "Jni/basic.h"

/* FIELDS */
ThreadServer server;
JNIContainer jni;
PHeader pH;

/* COMMANDS */
struct CommandDci : CommandExecute 
{
	CommandDci() : CommandExecute("DCI", "/dci") {}

	void Execute(SOCKET user, REF(string) full_line, REF(vector<string>) arguments) override
	{
		// пока хард кодом, потом добавлю main thread
		// и это будет выполняться в нем
		if (jni.env == nullptr)
			jni.attachContainer();

		JNIDumperStruct dumper(jni);
		bool header = string_utils::HasArgument(arguments, "--header");

		auto classes = dumper.GetAllClassInfo(
			string_utils::HasArgument(arguments, "--methods"),
			string_utils::HasArgument(arguments, "--fields"),
			string_utils::HasArgument(arguments, "--jdk_ignore"),
			string_utils::HasArgument(arguments, "--lambda_ignore"),
			string_utils::ParseArgumentsValues(arguments, "/gg", ':'));


		Logger::Debug("Founded class: " + ToStr(classes.size()));
		server.Server()->OutputString(user, GetHeaderClassses(classes));
	}
};

struct CommandVersion : CommandExecute 
{
	CommandVersion() : CommandExecute("VERSION", "/version") {}

	void Execute(SOCKET user, REF(string) full_line, REF(vector<string>) arguments) override
	{
		if (string_utils::HasArgument(arguments, "--V")) 
		{
			server.Server()->OutputString(user, pH.version);
			return;
		}
		if (string_utils::HasArgument(arguments, "--N"))
		{
			server.Server()->OutputString(user, pH.name);
			return;
		}
		if (string_utils::HasArgument(arguments, "--C"))
		{
			server.Server()->OutputString(user,  ToStr(pH.cid));
			return;
		}

		/* send pH */
		server.Server()->OutputString(user, Str3(pH.name, ", ", pH.version));
	}
};

/* DLL ENTRY POINT */
void main()
{
	// init pH
	pH = {"jd server", "1.0.0", 2716};
	
	// auto free library +server.stop
	signal(SIGINT, [](int signal)
	{
		server.StopThread();

		AllocConsoleEnd;
		CloseMainThreadModule(0);
		exit(0);
	});
	
	server.StartThread(2716);
	server.AddCommand<CommandDci>();
	server.AddCommand<CommandVersion>();
	server.Wait();

	// exit
	raise(SIGINT);
}

SetSharedEntryPoint(main);



