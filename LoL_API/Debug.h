#pragma once
void debugPrint(char const* message, ...);
void debugPrint(wchar_t const* message, ...);

inline void debugCallStack(int n)
{
	void **_ebp;
	__asm { mov _ebp, ebp };

	void **ebp = reinterpret_cast<void**>(&_ebp);

	for (int i = 0; i < n; ++i)
	{
		ebp = static_cast<void**>(*ebp);
		debugPrint("%p", ebp[1]);
	}
}