#include "Core.h"

lapi::Core::Core() : m_memory(L"League of Legends.exe")
{
	hijackTopFunction();
	getFunctionPointers();
}

void new_topFunction()
{	
	// Do testing with functions from League of Legends.exe below
	{
		char result[200] = { 0 };
		char *s = "012345678912345";
		int l1 = strlen(s);
		char *r = lapi::Core::NewString(result, s, l1);

		if (result[3] < ' ' || result[3] > '~')
			debugPrint(" Result = [%p]", result);
		else
			debugPrint(" Result = %s", result);
		if (IsBadReadPtr(*reinterpret_cast<char**>(result), 10))
			debugPrint("*Result = [bad ptr]");
		else
			debugPrint("*Result = %s", *reinterpret_cast<char**>(result));
		debugPrint(" s      = %s", s);
		debugPrint("l1      = %ld", l1);
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

	m_memory.resume();
}

lapi::Core::NewString_f lapi::Core::NewString;
void lapi::Core::getFunctionPointers()
{
	NewString = m_memory.findSignature<NewString_f>(Signature("\x8B\xFA\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x85\xDB", "x?xxxxxxxxxxxx"));
}