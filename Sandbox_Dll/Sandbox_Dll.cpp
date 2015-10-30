#include <Windows.h>
#include <iostream>
#include <string>

void debugPrint(char* message, ...);

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		debugPrint("Injected!");
		break;
	}
	case DLL_PROCESS_DETACH:
		debugPrint("Farewell cruel world :c");
		break;
	default:
		break;
	}

	return TRUE;
}

void debugPrint(char* message, ...)
{
	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	OutputDebugStringA(buffer);
	va_end(va);
}