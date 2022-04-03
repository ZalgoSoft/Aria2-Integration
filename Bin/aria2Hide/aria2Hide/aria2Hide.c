#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <WinUser.h>
void wmain(int argc, TCHAR* argv[])
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	// get parent pid
	int ppid = -1;
	int pid = -1;
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	pid = GetCurrentProcessId();
	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == pid) {
				ppid = pe.th32ParentProcessID;
			}
		} while (Process32Next(h, &pe));
	}
	CloseHandle(h);

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	TCHAR cmd[256] = { L'\0' };
	TCHAR* p = _Notnull_  wcsrchr(argv[0], L'\\');
	if (p) p[0] = L'\0';
	_wchdir(argv[0]);
	if (argc == 1)
		wsprintfW(cmd, L"aria2c.exe -D --conf-path=aria2.conf");
	else
		if (argc == 2)
			if (lstrcmpW(argv[1], L"aria2://stop-with-process/") == 0)
				wsprintfW(cmd, L"aria2c.exe -D --conf-path=aria2.conf --stop-with-process=%d", ppid);
			else
				wsprintfW(cmd, L"aria2c.exe -D --conf-path=aria2.conf");

	//wprintf(L"\n %s \n", (LPWSTR)&cmd);
	// Start the child process. 
	if (!CreateProcess(NULL,   // No module name (use command line)
		cmd,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NO_WINDOW,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		//printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	//WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}