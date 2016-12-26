#pragma once
#include "stdafx.h"
#include <vector>
#include "Memory.h"

#define WRAP_MEMBER(_NAME, _RET, _ARGS, _OFFSET) \
__pragma(warning(disable:4731))\
__declspec(noinline) _RET _NAME _ARGS\
{\
	void* _function_addr = _OFFSET + lapi::Memory::BaseAddress;\
	__asm\
	{\
		__asm mov eax, _function_addr\
		__asm mov esp, ebp\
		__asm pop ebp\
		__asm jmp eax\
	};\
}\
__pragma(warning(default:4731))

namespace lapi
{
	struct Offsets
	{
		static uint32_t _commit;
		enum
		{
			main = 0x0012CF20,
			Commit = 0x0126345C,
			String_Assign = 0x004B44D0,
			Log_Debug = 0x001524B0,
			Log = 0x008064D0,
		};
	};

	enum LocationType
	{
		Absolute,
		Relative
	};

	inline static void getOffset(Memory const& memory, char const* name, Signature const& sig, size_t offset = 0, std::vector<int> indirections = std::vector<int>(), LocationType loc = LocationType::Absolute)
	{
		auto addr = memory.findSignature(sig);
		if (!addr)
		{
			debugPrint("!!! %s NOT FOUND", name);
		}
		else
		{
			debugPrint("  // Signature found at 0x%p,", addr);
			addr += offset;
			debugPrint("  // With offset, I'm now at 0x%p,", addr);
			debugPrint("  // Value at 0x%p: 0x%x", addr, *(byte*)addr);
			debugPrint("  // Value at 0x%p+1: 0x%x", addr, *((byte*)addr+1));
			debugPrint("  // Value at 0x%p+2: 0x%x", addr, *((byte*)addr+2));
			debugPrint("  // Value at 0x%p+3: 0x%x", addr, *((byte*)addr+3));
			debugPrint("  // Value at 0x%p+4: 0x%x", addr, *((byte*)addr+4));

			if (loc == LocationType::Relative)
			{
				int relative = *(int*)addr + 4;
				debugPrint("  // Relative requires a jump of 0x%p,", relative);
				addr += relative;
				debugPrint("  // Relative address, I'm now at 0x%p,", addr);
			}

			for (auto ind : indirections)
			{
				auto pointedAddr = *reinterpret_cast<BYTE**>(addr);
				addr = pointedAddr + ind;
				debugPrint("  // Applied indirection, I'm now at 0x%p,", addr);
			}

			size_t o = addr - Memory::BaseAddress;
			debugPrint("%s = 0x%p,", name, o);
		}
		
	}

	inline void DumpSignatures(Memory const& m)
	{
		// Search for WinMain
		getOffset(m, "main", Signature("51 6A 00 BA ? ? ? ? B9 ? ? ? ? E8 ? ? ? ? 83 C4 08 A2 ? ? ? ? C3"));
		getOffset(m, "Commit", Signature("8B 35 ? ? ? ? 80 3D ? ? ? ? ? 75 2C"), 2, std::vector<int>({ 0 }));
		getOffset(m, "Log_Debug", Signature("E8 ? ? ? ? 83 3D ? ? ? ? ? 74 05 E8 ? ? ? ? 83 3D"), 1, std::vector<int>(), LocationType::Relative);
		getOffset(m, "Log", Signature("55 8B EC 83 E4 F8 8B 55 14"));
	}

}
