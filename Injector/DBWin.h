#pragma once
#include <Windows.h>

// See http://www.codeproject.com/Articles/23776/Mechanism-of-OutputDebugString

struct dbwin_buffer
{
	DWORD   pid;
	char    data[4096 - sizeof(DWORD)];
};

class DBWin
{
public:
	DBWin(DWORD pid);
	~DBWin();

	void start();
	void stop();
	static DWORD WINAPI run(void *instance);
private:
	static const int TIMEOUT = 1000;
	bool keepRunning;
	bool initialized;
	DWORD processId;
	HANDLE hThread;

	HANDLE hMutex;
	HANDLE hBufferReady;
	HANDLE hDataReady;
	HANDLE hBuffer;
	
	dbwin_buffer* dbBuffer;
};

