#pragma once

namespace lapi {
	namespace Riot {
		namespace Log {
			enum Severity {
				Okay = 0,
				Warn = 1,
				Error = 2,
				Always = 3
			};

			typedef void(__cdecl *debugLog_t)(char const* message, ...);
			static debugLog_t debugLog;

			typedef int(__cdecl *log_t)(int unk1, int unk2, int unk3, const char *message, ...);
			static log_t log;
		}
	}
}