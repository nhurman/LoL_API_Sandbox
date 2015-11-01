#pragma once
#include "Memory.h"

namespace lapi
{
	class Core
	{
		bool m_initialized;
		Memory m_memory;
		void hookFunctions();

	public:
		Core();
		bool initialized() const { return m_initialized; }
	};
}
