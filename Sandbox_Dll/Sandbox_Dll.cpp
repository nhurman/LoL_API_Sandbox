#include <Windows.h>
#include <iostream>
#include <string>

#include "../LoL_API/Debug.h"
#include "../LoL_API/Core.h"

lapi::Core *g_api = nullptr;

void openConsole();

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		//openConsole();
		debugPrint("Injected!");
		g_api = new lapi::Core();
		if (!g_api->initialized())
		{
			return FALSE;
		}

		break;
	}
	case DLL_PROCESS_DETACH:
		debugPrint("Farewell cruel world :c");
		delete g_api;
		break;
	default:
		break;
	}

	return TRUE;
}

void openConsole()
{
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	freopen_s(&fp, "CONIN$", "r", stdin);
}