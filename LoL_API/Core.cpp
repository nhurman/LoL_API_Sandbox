#include "Core.h"
#include "Riot/String.h"

lapi::Core::Core() : m_memory(L"League of Legends.exe")
{
	getFunctionPointers();
	hijackTopFunction();

	// Unpause the main LoL thread so it hits our hook
	m_memory.resume();
}

lapi::Address lapi::Riot::String::_assign;
void new_topFunction()
{	
	// Do testing with functions from League of Legends.exe below
	{
		std::string inString = "Hello";
		lapi::Riot::String* rs = (lapi::Riot::String*)&inString;
		rs->assign("WASSUP BOYS", 12);
		debugPrint("New value: %s(%ld)", inString.c_str(), inString.length());
	}
	
	debugPrint("-- Done");
	SuspendThread(GetCurrentThread());
	__debugbreak();
}

void lapi::Core::hijackTopFunction()
{
	// Hijack one of the top-level functions in the game so we can get
	// control of the execution flow once the initialization has been done
	lapi::Signature sig("\x8B\x41\x08\x8B\x49\x04\x8B\x10", "xxxxxxxx");
	lapi::Address function = m_memory.findSignature(sig);
	if (!function)
	{
		debugPrint("Signature not found");
		return;
	}

	lapi::Address newAddress = lapi::GetAddress(new_topFunction);
	m_memory.beginTransaction();
	m_memory.detourAddress(&function, newAddress);
	m_memory.commit();
}

void lapi::Core::getFunctionPointers()
{
	lapi::Riot::String::_assign = m_memory.findSignature(Signature("8B ?? 55 8B EC 53 8B 5D 08 56 8B F1 85 DB"));	
}