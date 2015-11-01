#include "Core.h"
#include "Riot/String.h"

lapi::Core::Core() : m_initialized{ false }, m_memory{ L"League of Legends.exe" }
{
	char const* lolVersion = reinterpret_cast<char const*>(Memory::BaseAddress + Offsets::Version);
	char const* offsetsVersion = Offsets::_version;
	debugPrint("Version %s", lolVersion);
		
	// To update the signatures, uncomment this block then copy the output to Offsets.h
	// then change the version in Offsets.cpp
	/*
	debugPrint("+++++++ Signature scan results");
	DumpSignatures(m_memory);
	debugPrint("+++++++");
	return;
	//*/

	// Check that the version matches the one of the signatures
	if (strcmp(offsetsVersion, lolVersion) != 0)
	{
		debugPrint("!!! VERSION MISMATCH !!!");
		debugPrint("I have signatures for %s but detected version is %s", offsetsVersion, lolVersion);
		debugPrint("Please update the signatures (see %s:%ld)", __FILE__, __LINE__);
		return;
	}

	hookFunctions();
	m_initialized = true;

	// Unpause the main LoL thread so it hits our hook
	m_memory.resume();
}

void __cdecl new_log_debug(char* message, ...)
{
	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	debugPrint("[DEBUG] %s", buffer);
	va_end(va);
}

void new_topFunction()
{
	debugPrint("-- Done");
	SuspendThread(GetCurrentThread());
	__debugbreak();
}

void lapi::Core::hookFunctions()
{
	/* This function is empty (it only contains a ret) but we need 5 bytes for the jmp
	There are some unused bytes after the function though, we overwrite them with NOP
	so the detour library knows it's safe to overwrite them */
	{
		auto addr = Memory::BaseAddress + Offsets::Log_Debug;
		DWORD access;
		VirtualProtect(addr + 1, 4, PAGE_EXECUTE_READWRITE, &access);
		addr[1] = 0x90;
		addr[2] = 0x90;
		addr[3] = 0x90;
		addr[4] = 0x90;
		VirtualProtect(addr + 1, 4, access, nullptr);
	}

	m_memory.beginTransaction();

	// Hijack one of the top-level functions in the game so we can get
	// control of the execution flow once the initialization has been done
	m_memory.detourAddress(Memory::BaseAddress + Offsets::main, Memory::GetAddress(new_topFunction));

	// Nulled debug function
	m_memory.detourAddress(Memory::BaseAddress + Offsets::Log_Debug, Memory::GetAddress(new_log_debug));

	m_memory.commit();
}
