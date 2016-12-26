// Start a League of Legends.exe in a suspended thread
// then inject our DLL into it so we can call functions from there

#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")
#include <string>
#include <iostream>
#include "DBWin.h"

bool startLoLProcess(PROCESS_INFORMATION *processInfo);
HMODULE loadDll(HANDLE hProcess, wchar_t* dllPath);
bool unloadDll(HANDLE hProcess, HMODULE hDll);

int main(int argc, char* argv[])
{
	// Start a LoL process
	PROCESS_INFORMATION processInfo;
	if (!startLoLProcess(&processInfo))
	{
		std::cerr << "Could not start LoL process" << std::endl;
		return 1;
	}

	// Get a logger for OutputDebugString
	DBWin logger(processInfo.dwProcessId);
	logger.start();

	// Get the full path of our DLL
	wchar_t currentDirectory[FILENAME_MAX];
	wchar_t dllPath[FILENAME_MAX];
	GetCurrentDirectoryW(FILENAME_MAX, currentDirectory);
#ifdef _DEBUG
	PathCombineW(dllPath, currentDirectory, L"..\\Debug\\Sandbox_Dll.dll");
#else
	PathCombineW(dllPath, currentDirectory, L"..\\Release\\Sandbox_Dll.dll");
#endif

	// Inject it
	std::cout << "-- Injecting DLL. Press Enter to terminate the process" << std::endl;
	HMODULE hDll = loadDll(processInfo.hProcess, dllPath);
	if (!hDll)
	{
		std::cerr << "Could not load DLL into target process" << std::endl;
		TerminateProcess(processInfo.hProcess, 0);
		return 1;
	}

	// Wait for user input then quit
	std::cin.get();
	if (!unloadDll(processInfo.hProcess, hDll))
	{
		std::cerr << "Could not unload DLL from the target process" << std::endl;
		TerminateProcess(processInfo.hProcess, 0);
		return 1;
	}

	TerminateProcess(processInfo.hProcess, 0);

	return 0;
}

bool startLoLProcess(PROCESS_INFORMATION *processInfo)
{
	// Create the LoL process
	wchar_t workingDir[]{ L"C:\\Riot Games\\League of Legends\\RADS\\solutions\\lol_game_client_sln\\releases\\0.0.1.155\\deploy\\" };
	wchar_t imagePath[]{ L"C:\\Riot Games\\League of Legends\\RADS\\solutions\\lol_game_client_sln\\releases\\0.0.1.155\\deploy\\League of Legends.exe" };
	wchar_t commandLine[]{ L"" };
	SECURITY_ATTRIBUTES *secAttrs = nullptr;
	DWORD creationFlags = CREATE_SUSPENDED;
	STARTUPINFOW startupInfo{ sizeof(startupInfo) };
	startupInfo.dwFlags = 0;
	BOOL started = CreateProcessW(imagePath, commandLine, secAttrs, secAttrs, FALSE, creationFlags, nullptr, workingDir, &startupInfo, processInfo);

	if (!started)
	{
		std::cerr << "Could not start the LoL process" << std::endl;
	}

	return started == 1;
}

HMODULE loadDll(HANDLE hProcess, wchar_t *dllPath)
{
	size_t pathSize = wcslen(dllPath) * sizeof(wchar_t);

	// Allocate some memory
	DWORD flags = MEM_COMMIT | MEM_RESERVE;
	void *memoryAddress = VirtualAllocEx(hProcess, nullptr, pathSize, flags, PAGE_READWRITE);
	if (!memoryAddress)
	{
		std::cerr << "Could not reserve memory space" << std::endl;
		return nullptr;
	}

	// Write the dll path to it
	if (!WriteProcessMemory(hProcess, memoryAddress, dllPath, pathSize, nullptr))
	{
		std::cerr << "Could not write DLL address" << std::endl;
		return nullptr;
	}

	// Get the handle to LoadLibraryW
	HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
	if (!hKernel32)
	{
		std::cerr << L"Could not get Kernel32 handle" << std::endl;
		return nullptr;
	}

	LPTHREAD_START_ROUTINE hLoadLibraryW = reinterpret_cast<LPTHREAD_START_ROUTINE>(
		GetProcAddress(hKernel32, "LoadLibraryW"));
	if (!hLoadLibraryW)
	{
		std::cerr << "Could not get LoadLibraryW address" << std::endl;
		return nullptr;
	}

	// Now start a new thread to load our DLL	
	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, hLoadLibraryW, memoryAddress, 0, nullptr);
	if (!hThread)
	{
		std::cerr << L"Could not create remote thread" << std::endl;
		return nullptr;
	}

	// Wait for our thread to finish
	WaitForSingleObject(hThread, INFINITE);
	HMODULE returnValue;
	GetExitCodeThread(hThread, reinterpret_cast<DWORD*>(&returnValue));
	CloseHandle(hThread);

	return returnValue;
}

bool unloadDll(HANDLE hProcess, HMODULE hDll)
{
	// Get the handle to FreeLibrary
	HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
	if (!hKernel32)
	{
		std::cerr << L"Could not get Kernel32 handle" << std::endl;
		return false;
	}

	LPTHREAD_START_ROUTINE hFreeLibrary = reinterpret_cast<LPTHREAD_START_ROUTINE>(
		GetProcAddress(hKernel32, "FreeLibrary"));
	if (!hFreeLibrary)
	{
		std::cerr << "Could not get FreeLibrary address" << std::endl;
		return false;
	}

	// Call FreeLibrary in the remote process
	HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, hFreeLibrary, hDll, 0, nullptr);
	if (!hThread)
	{
		std::cerr << L"Could not create remote thread" << std::endl;
		return false;
	}

	// Return the exit code
	WaitForSingleObject(hThread, INFINITE);
	DWORD returnValue;
	GetExitCodeThread(hThread, &returnValue);
	CloseHandle(hThread);

	return returnValue == 1;
}
