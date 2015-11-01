#include "Debug.h"
#include <Windows.h>
#include <cstdio>

void debugPrint(char const* message, ...)
{
	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	OutputDebugStringA(buffer);
	//printf("%s\n", buffer);
	va_end(va);
}

void debugPrint(wchar_t const* message, ...)
{
	va_list va;
	va_start(va, message);
	wchar_t buffer[1024];
	vswprintf(buffer, sizeof(buffer), message, va);
	OutputDebugStringW(buffer);
	//wprintf(L"%s\n", buffer);
	va_end(va);
}
