#include <Windows.h>
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <tlhelp32.h>
#include <tchar.h>

#define nullptr 0
#define bool int
#define false 0
#define true 1



typedef struct {
    DWORD processId;
    TCHAR processName[MAX_PATH];
    TCHAR windowTitle[256];
} ProcessWindowInfo;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    ProcessWindowInfo* pInfo = (ProcessWindowInfo*)lParam;
    DWORD windowProcessId;

    GetWindowThreadProcessId(hwnd, &windowProcessId);

    if (windowProcessId == pInfo->processId) {
        TCHAR title[256];
        GetWindowText(hwnd, title, sizeof(title) / sizeof(TCHAR));

        if (_tcslen(title) > 0 && _tcslen(pInfo->windowTitle) == 0) {
            _tcscpy_s(pInfo->windowTitle, sizeof(pInfo->windowTitle) / sizeof(TCHAR), title);
        }
    }
    return TRUE;
}

char* get_absolute_path(const char* relative_path) {
    char buffer[MAX_PATH];

    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0) {
        return NULL;
    }

    char* last_slash = strrchr(buffer, '\\');
    if (last_slash == NULL) {
        return NULL;
    }

    strcpy_s(last_slash + 1, MAX_PATH - (last_slash - buffer + 1), relative_path);

    return _strdup(buffer);
}

BOOL inject_dll(DWORD pid, const char* dll_path) {
    char* absolute_path = get_absolute_path(dll_path);
    if (absolute_path == NULL) {
        printf("Error: Cannot resolve absolute path for: %s\n", dll_path);
        return FALSE;
    }

    printf("Resolved DLL path: %s\n", absolute_path);

    DWORD file_attr = GetFileAttributesA(absolute_path);
    if (file_attr == INVALID_FILE_ATTRIBUTES) {
        printf("Error: DLL file not found: %s (Error: %lu)\n", absolute_path, GetLastError());
        free(absolute_path);
        return FALSE;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        printf("Error: Cannot open process PID %lu (Error: %lu)\n", pid, GetLastError());
        free(absolute_path);
        return FALSE;
    }

    SIZE_T path_size = strlen(absolute_path) + 1;
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, path_size,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (pRemoteMemory == NULL) {
        printf("Error: Cannot allocate memory in target process (Error: %lu)\n", GetLastError());
        CloseHandle(hProcess);
        free(absolute_path);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, pRemoteMemory, absolute_path, path_size, NULL)) {
        printf("Error: Cannot write to target process memory (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        free(absolute_path);
        return FALSE;
    }

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    LPTHREAD_START_ROUTINE pLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryA");

    if (pLoadLibrary == NULL) {
        printf("Error: Cannot get LoadLibraryA address\n");
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        free(absolute_path);
        return FALSE;
    }

    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pLoadLibrary, pRemoteMemory, 0, NULL);
    if (hRemoteThread == NULL) {
        printf("Error: Cannot create remote thread (Error: %lu)\n", GetLastError());
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        free(absolute_path);
        return FALSE;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeThread(hRemoteThread, &exit_code);

    CloseHandle(hRemoteThread);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    free(absolute_path);

    if (exit_code == 0) {
        printf("Error: LoadLibrary failed in target process\n");
        return FALSE;
    }

    return TRUE;
}

void list_java_processes() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        _tprintf(_T("Error: Could not create process snapshot (Error: %lu)\n"), GetLastError());
        return;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe)) {
        _tprintf(_T("Error: Could not retrieve first process (Error: %lu)\n"), GetLastError());
        CloseHandle(hSnapshot);
        return;
    }

    _tprintf(_T("----------------------------------------\n"));

    int found = 0;

    do {
        if (_tcscmp(pe.szExeFile, _T("java.exe")) == 0 || _tcscmp(pe.szExeFile, _T("javaw.exe")) == 0) {
            ProcessWindowInfo info = { 0 };
            info.processId = pe.th32ProcessID;
            _tcscpy_s(info.processName, sizeof(info.processName) / sizeof(TCHAR), pe.szExeFile);

            EnumWindows(EnumWindowsProc, (LPARAM)&info);

            if (_tcslen(info.windowTitle) > 0) {
                _tprintf(_T("[%lu]\t%s\t%s\n"), info.processId, info.processName, info.windowTitle);
            }
            else {
                _tprintf(_T("[%lu]\t%s\t%s\n"), info.processId, info.processName, _T("No title"));
            }
            found++;
        }
    } while (Process32Next(hSnapshot, &pe));

    if (found == 0) {
        _tprintf(_T("No Java processes found.\n"));
    }
    else {
        _tprintf(_T("----------------------------------------\n"));
    }

    CloseHandle(hSnapshot);
}

bool is_empty(const char* t) {
	if (t == nullptr) return false;
	
	auto size = strlen(t);
	if (size == 0) return false;

	return true;
}

int main(int argc, char** argv) 
{
	setlocale(0, "Ru");

	printf("Client - loader 1.0.0\n");

	if (argc <= 1) {
		printf("[~] Укажите первым аргументом файл сервера.\n");
		return 4;
	}

	const char* path = argv[1];
	if (path == nullptr || !is_empty(path)) {
		printf("[~] Строка имеет не верный формат.\n");
		return 5;
	}

    list_java_processes();
    printf("$ ");
    char buffer[255];
    
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    int pID = atoi(buffer);
    if (pID == 0 || pID == -1) {
        printf("[~] pID имеет не верный формат.\n");
        return 6;
    }

    if (inject_dll(pID, path)) {
        printf("Success!\n");
        return 0;
    }

    printf("Error loading.");
    return -1;
}