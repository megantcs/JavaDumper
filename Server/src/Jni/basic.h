//
// Created by inter on 08.11.2025.
//

#ifndef UNTITLED2_BASIC_H
#define UNTITLED2_BASIC_H

#include <fstream>
#include <iostream>
#include <jni.h>
#include <jvmti.h>
#include <Windows.h>

inline void CloseMainThreadModule(int postCode);

struct JNIContainer
{
    JNIEnv* env = nullptr;
    jvmtiEnv* jvmti = nullptr;
    JavaVM* vm = nullptr;

    void attachContainer() {
        jsize numVMs = 0;
        JNI_GetCreatedJavaVMs(&vm, 1, &numVMs);
        jint result = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);

        if (result == JNI_EDETACHED) {
            result = vm->AttachCurrentThread(reinterpret_cast<void**>(&env), nullptr);
        }

        vm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2);
    }
};

#define AllocConsoleBegin \
AllocConsole(); \
freopen("CONIN$", "r", stdin); \
freopen("CONOUT$", "w", stdout); \
freopen("CONOUT$", "w", stderr); \
std::cout.clear(); \
std::cin.clear();

#define AllocConsoleEnd \
fclose(stdin); \
fclose(stdout); \
fclose(stderr); \
FreeConsole();

#define SetSharedEntryPoint(main)\
HMODULE MAIN_THREAD_MODULE = nullptr;\
\
void CloseMainThreadModule(int postCode) {\
    FreeLibraryAndExitThread(MAIN_THREAD_MODULE, postCode); \
}\
\
DWORD WINAPI Constructor(LPVOID lpParam) {\
    HMODULE x = static_cast<HMODULE>(lpParam);\
    AllocConsoleBegin;\
    MAIN_THREAD_MODULE = x;\
    \
    main();\
    \
    AllocConsoleEnd;\
    FreeLibraryAndExitThread(x, 0);\
    return 0;\
}\
\
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {\
    switch (ul_reason_for_call) {\
        case DLL_PROCESS_ATTACH:\
            CreateThread(NULL, 0, Constructor, hModule, 0, NULL);\
            break;\
        case DLL_PROCESS_DETACH:\
        default:\
            break;\
    }\
    return TRUE;\
}


#endif //UNTITLED2_BASIC_H