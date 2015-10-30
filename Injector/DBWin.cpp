#include "DBWin.h"
#include <iostream>

DBWin::DBWin(DWORD pid = -1)
{
	initialized = false;
	processId = pid;

	hMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"DBWinMutex");
	if (!hMutex)
	{
		std::cerr << "Failed opening DBWinMutex" << std::endl;
		return;
	}

	hBufferReady = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"DBWIN_BUFFER_READY");
	if (!hBufferReady)
	{
		hBufferReady = CreateEventW(nullptr, FALSE, TRUE, L"DBWIN_BUFFER_READY");
		if (!hBufferReady)
		{
			std::cerr << "Failed to open or create DBWIN_BUFFER_READY" << std::endl;
			return;
		}
	}

	hDataReady = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"DBWIN_DATA_READY");
	if (!hDataReady)
	{
		hDataReady = CreateEventW(nullptr, FALSE, FALSE, L"DBWIN_DATA_READY");
		if (!hDataReady)
		{
			std::cerr << "Failed to open or create DBWIN_DATA_READY" << std::endl;
			return;
		}
	}

	hBuffer = OpenFileMappingW(FILE_MAP_READ, FALSE, L"DBWIN_BUFFER");
	if (!hBuffer)
	{
		hBuffer = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
			sizeof(dbwin_buffer), L"DBWIN_BUFFER");
		if (!hBuffer)
		{
			std::cerr << "Failed to open or create DBWIN_BUFFER" << std::endl;
			return;
		}
	}

	dbBuffer = static_cast<dbwin_buffer*>(MapViewOfFile(hBuffer, SECTION_MAP_READ, 0, 0, 0));
	if (!dbBuffer)
	{
		std::cerr << "Failed to map memory to dbBuffer" << std::endl;
		return;
	}

	initialized = true;
}


DBWin::~DBWin()
{
	if (hThread)
	{
		stop();
	}

	if (hMutex)
	{
		CloseHandle(hMutex);
	}

	if (hBuffer)
	{
		CloseHandle(hBuffer);
	}

	if (hBufferReady)
	{
		CloseHandle(hBufferReady);
	}

	if (hDataReady)
	{
		CloseHandle(hDataReady);
	}

	dbBuffer = nullptr;
}

DWORD WINAPI DBWin::run(void *instance)
{
	DBWin *self = static_cast<DBWin*>(instance);
	while (self->keepRunning)
	{
		DWORD r = WaitForSingleObject(self->hDataReady, 1000);
		if (r == WAIT_OBJECT_0)
		{
			if (self->processId == -1)
			{
				std::cout << "[" << self->dbBuffer->pid << "] ";
			}

			if (self->processId == -1 || self->dbBuffer->pid == self->processId)
			{
				std::cout << self->dbBuffer->data << std::endl;
			}

			SetEvent(self->hBufferReady);
		}
	}

	return 0;
}

void DBWin::start()
{
	if (!initialized)
	{
		std::cerr << "Not initialized" << std::endl;
		return;
	}

	if (hThread)
	{
		std::cerr << "Thread already running" << std::endl;
		return;
	}

	keepRunning = true;
	hThread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(run), this, 0, nullptr);
}

void DBWin::stop()
{
	if (!hThread)
	{
		std::cerr << "Thread is not running" << std::endl;
		return;
	}

	keepRunning = false;
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	hThread = nullptr;
}