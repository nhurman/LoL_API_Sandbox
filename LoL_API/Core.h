#pragma once
#include "Memory.h"

namespace lapi
{
	class Core
	{
	public:
		Core();

	private:
		Memory m_memory;

		void hijackTopFunction();
		void getFunctionPointers();

	public:
		//! Creates a new RiotString object
		/*!
		 * If the string size is < 15, it is stored directly into the object,
		 * otherwise the first 4 bytes are a pointer to a remote memory location.
		 * \param dest Destination address
		 * \param src  Source string
		 * \param size Length of the string
		 */
		typedef char*(__thiscall *NewString_f)(char* dest, char* src, size_t size);
		static NewString_f NewString;
	};
}