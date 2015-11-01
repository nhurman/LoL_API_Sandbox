#line 1 "String.h"
#pragma once














namespace lapi {
	namespace Riot
	{
		class String
		{
			static const int INPLACE_BUFFER_SIZE = 16;
		public:
			char m_data[INPLACE_BUFFER_SIZE];
			size_t m_size;
			int m_maxSize;

			String() : m_data{ 0 }, m_size{ 0 }, m_maxSize{ INPLACE_BUFFER_SIZE - 1 } {}

			__declspec(noinline) String& assign (char const* buffer, int size){ void* _function_addr = _assign; __asm { __asm mov esp, ebp __asm pop ebp __asm jmp _function_addr };}

		public:
			static void* _assign;
		};
	}
};
