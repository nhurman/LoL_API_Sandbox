#pragma once
#include "Memory.h"
#include "Riot/String.h"

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
	};
}