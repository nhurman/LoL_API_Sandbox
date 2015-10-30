#include <Windows.h>
#include <iostream>
#include <string>

#include "../LoL_API/Debug.h"
#include "../LoL_API/Core.h"

lapi::Core *g_api = nullptr;

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		debugPrint("Injected!");
		g_api = new lapi::Core();

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
