#pragma once

namespace lapi {
	namespace Riot
	{
		// Original functions
		typedef void(__cdecl *topLevelFunction_t)(void);
		static topLevelFunction_t topLevelFunction;
	}
}