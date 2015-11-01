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
		static char const* _version;
		enum
		{
			main = 0x00D32757,
			Version = 0x01057A54,
			String_Assign = 0x004B44D0,
			Log_Debug = 0x0015CC20,
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
			addr += offset;

			if (loc == LocationType::Relative)
			{
				int relative = *(int*)addr + 4;
				addr += relative;
			}

			for (auto ind : indirections)
			{
				auto pointedAddr = *reinterpret_cast<BYTE**>(addr);
				addr = pointedAddr + ind;
			}

			size_t o = addr - Memory::BaseAddress;
			debugPrint("%s = 0x%p,", name, o);
		}
		
	}

	inline void DumpSignatures(Memory const& m)
	{
		getOffset(m, "main", Signature("55 8B EC 83 3D ? ? ? ? ? 74 19"));
		getOffset(m, "Version", Signature("A3 ? ? ? ? 75 2B"), 0x1C, std::vector<int>({0}));
		getOffset(m, "Log_Debug", Signature("68 ? ? ? ? 6A 00 6A 01 6A 03 E8 ? ? ? ? 8D 5F 0C"), 0x1A, std::vector<int>(), LocationType::Relative);
		getOffset(m, "String_Assign", Signature("8B ?? 55 8B EC 53 8B 5D 08 56 8B F1 85 DB"));
	}

}