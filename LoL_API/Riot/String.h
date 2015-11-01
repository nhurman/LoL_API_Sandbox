#pragma once
#include "../Offsets.h"

namespace lapi {
	namespace Riot
	{
		// This class is just for testing purposes, it should not be used (just map it to std::string)
		class String
		{
			static const int INPLACE_BUFFER_SIZE = 16;
		public:
			char m_data[INPLACE_BUFFER_SIZE];
			size_t m_size;
			int m_maxSize;

			String() : m_data{ 0 }, m_size{ 0 }, m_maxSize{ INPLACE_BUFFER_SIZE - 1 } {}

			WRAP_MEMBER(assign, String&, (char const* buffer, int size), Offsets::String_Assign)

		};
	}
};
