#include "Core.h"
#include "Riot/Log.h"
#include "Riot/Misc.h"
#include "Riot/String.h"

#include <iostream>

lapi::Core::Core() : m_initialized{ false }, m_memory{ L"League of Legends.exe" }
{
	uint32_t lolCommit = *reinterpret_cast<uint32_t*>(Memory::BaseAddress + Offsets::Commit);
	uint32_t offsetsCommit = Offsets::_commit;
	debugPrint("LoL binary from commit %u", lolCommit);
		
	// To update the signatures, uncomment this block then copy the output to Offsets.h
	// then change the version in Offsets.cpp
	/*
	debugPrint("+++++++ Signature scan results");
	DumpSignatures(m_memory);
	debugPrint("+++++++");
	return;
	//*/

	// Check that the version matches the one of the signatures
	if (lolCommit != offsetsCommit)
	{
		debugPrint("!!! VERSION MISMATCH !!!");
		debugPrint("I have signatures for [%u] but detected version is [%u]", offsetsCommit, lolCommit);
		debugPrint("Please update the signatures (see %s:%ld)", __FILE__, __LINE__);
		return;
	}

	hookFunctions();
	m_initialized = true;

	// Unpause the main LoL thread so it hits our hook
	m_memory.resume();
}

__declspec(naked) uint32_t* getEip()
{
	__asm {
		mov eax, [esp]
		ret
	}
}

void __cdecl new_debugLog(char const* message, ...)
{
	//printf("Paused at %p\n", getEip());
	//std::cin.get();

	for (auto i = 0; i < 3; ++i)
		if (IsBadReadPtr(&message[i], 1) || message[i] > 126 || message[i] < 9
			|| (9 < message[i] && message[i] < 32))
			return;

	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	auto len = strlen(buffer);
	if ('\n' == buffer[len - 1])
	{
		buffer[len - 1] = '\0';
	}
	debugPrint("[DEBUG] %s", buffer);
	va_end(va);
}

int __cdecl new_log(int unk1, int unk2, int unk3, const char *message, ...)
{
	if (unk1 == 0 && unk2 == 5 && unk3 == 0) return 0;
	va_list va;
	va_start(va, message);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), message, va);
	auto len = strlen(buffer);
	if ('\n' == buffer[len - 1])
	{
		buffer[len - 1] = '\0';
	}
	debugPrint("[Log] (%i %i %i) %s", unk1, unk2, unk3, buffer);
	va_end(va);
	return 0;
}

void new_topFunction()
{
	debugPrint(":)");
	//lapi::Riot::topLevelFunction();
	//SuspendThread(GetCurrentThread());
	//__debugbreak();
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
	Riot::topLevelFunction = reinterpret_cast<Riot::topLevelFunction_t>(Memory::BaseAddress + Offsets::main);
	m_memory.detourAddress(&Riot::topLevelFunction, new_topFunction);

	// Regular logging function
	Riot::Log::log = reinterpret_cast<Riot::Log::log_t>(Memory::BaseAddress + Offsets::Log);
	m_memory.detourAddress(&Riot::Log::log, new_log);

	// Nulled debug function
	Riot::Log::debugLog = reinterpret_cast<Riot::Log::debugLog_t>(Memory::BaseAddress + Offsets::Log_Debug);
	m_memory.detourAddress(&Riot::Log::debugLog, new_debugLog);

	m_memory.commit();
}
