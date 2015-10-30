#include "Debug.h"

void debugPrint(char* message, ...)
{
	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	OutputDebugStringA(buffer);
	va_end(va);
}

void debugPrint(wchar_t* message, ...)
{
	va_list va;
	va_start(va, message);
	wchar_t buffer[1024];
	vswprintf(buffer, sizeof(buffer), message, va);
	OutputDebugStringW(buffer);
	va_end(va);
}