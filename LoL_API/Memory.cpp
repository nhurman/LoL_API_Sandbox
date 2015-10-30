#include "Memory.h"
#include <memory>
#include <detours.h>
using namespace lapi;

Memory::Memory(std::wstring const& moduleName) : m_inTransaction{ false }
{
	if (!getModule(moduleName, m_module))
	{
		debugPrint("ERROR - Could not find main module");
		return;
	}
}

Memory::~Memory()
{
}

Address Memory::findSignature(Signature const& sig)
{
	// Adapted from Boyer-Moore-Horspool algorithm
	// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm

	// Preprocess the pattern to get the skip lengths
	std::string::size_type pos = sig.mask().rfind('?');
	size_t maxSkip = sig.size();
	if (pos != std::string::npos)
	{
		maxSkip -= pos + 1;
	}
	
	size_t T[256];
	for (int i = 0; i < 256; ++i)
	{
		T[i] = maxSkip;
	}
	for (size_t i = 0; i < sig.size() - 1; ++i)
	{
		T[sig.pattern()[i]] = min(maxSkip, sig.size() - 1 - i);
	}

	// Now scan
	int skip = 0;
	while (m_module.modBaseSize - skip >= sig.size())
	{
		int i = sig.size() - 1;
		while (sig.mask()[i] == '?' || m_module.modBaseAddr[skip + i] == static_cast<unsigned char>(sig.pattern()[i]))
		{
			if (i == 0)
			{
				return m_module.modBaseAddr + skip;
			}

			i -= 1;
		}

		skip += T[m_module.modBaseAddr[skip + sig.size() - 1]];
	}

	return nullptr;
}

void Memory::beginTransaction()
{
	if (!m_inTransaction)
	{
		m_inTransaction = true;
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
	}
}

Address Memory::detourAddress(Address const& source, Address const& dest)
{
	PVOID originalFunction = source;
	PVOID newFunction = dest;
	if (0 != DetourAttach(&originalFunction, newFunction))
	{
		return nullptr;
	}

	return static_cast<Address>(originalFunction);
}

bool Memory::detourAddress(Address* source, Address const& dest)
{
	Address originalFunction = detourAddress(*source, dest);
	if (nullptr == originalFunction)
	{
		return false;
	}

	*source = originalFunction;
	return true;
}

void Memory::commit()
{
	if (m_inTransaction)
	{
		DetourTransactionCommit();
		m_inTransaction = false;
	}
}

void Memory::resume()
{
	DWORD mainThreadId = 0;
	{
		std::shared_ptr<void> hSnapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, m_module.th32ProcessID), CloseHandle);
		THREADENTRY32 threadEntry;
		threadEntry.dwSize = sizeof(THREADENTRY32);
		Thread32First(hSnapshot.get(), &threadEntry);
		do
		{
			if (threadEntry.th32OwnerProcessID == m_module.th32ProcessID)
			{
				mainThreadId = threadEntry.th32ThreadID;
				break;
			}
		} while (Thread32Next(hSnapshot.get(), &threadEntry));
	}

	debugPrint("Pid: %ld, Tid: 0x%4X, Base address: 0x%p", m_module.th32ProcessID, mainThreadId, m_module.modBaseAddr);
	HANDLE mainThread = OpenThread(THREAD_RESUME, FALSE, mainThreadId);
	ResumeThread(mainThread);
	CloseHandle(mainThread);
}

bool Memory::getModule(std::wstring const& moduleName, MODULEENTRY32W& module)
{
	// Get the current process
	HANDLE hProcess = GetCurrentProcess();
	DWORD pid = GetProcessId(hProcess);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

	// Initialize structure
	module.dwSize = sizeof(module);

	// Iterate over the loaded modules trying to find our main exe
	bool found = false;
	Module32FirstW(hSnapshot, &module);
	do
	{
		if (moduleName == module.szModule)
		{
			found = true;
			break;
		}
	} while (Module32NextW(hSnapshot, &module));

	CloseHandle(hSnapshot);
	CloseHandle(hProcess);

	return found;
}