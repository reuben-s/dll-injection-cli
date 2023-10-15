#pragma once

#include <Windows.h>
#include <iostream>

class InjectDLL
{
public:
	DWORD dwPid;
	LPCSTR lpDllPath;

	InjectDLL(DWORD dwPid, LPCSTR lpDllPath);
	~InjectDLL();
	BOOL Inject();
private:
	HANDLE hProcess;
	HANDLE hRemoteThread;
	LPVOID pDllPath;

};

InjectDLL::InjectDLL(DWORD dwPid, LPCSTR lpDllPath)
{
	this->dwPid		= dwPid;
	this->lpDllPath = lpDllPath;

	this->hProcess		= NULL;
	this->hRemoteThread = NULL;
	this->pDllPath	    = NULL;
}

InjectDLL::~InjectDLL()
{
	if (this->pDllPath)
	{
		VirtualFreeEx(this->hProcess, this->pDllPath, 0, MEM_RELEASE);
	}
	if (this->hProcess)
	{
		CloseHandle(hProcess);
	}
	if (this->hRemoteThread)
	{
		CloseHandle(hRemoteThread);
	}
}

BOOL InjectDLL::Inject()
{
	std::cout << "--------\nInjecting DLL..\nPID: " << this->dwPid << "\nDLL Path: \"" << this->lpDllPath << "\"\n--------\n";

	// Aquire handle to process to be injected into
	this->hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->dwPid);
	if (this->hProcess == NULL)
		throw std::exception("Obtaining process handle failed: ");

	// Allocate memory in remote process 
	this->pDllPath = VirtualAllocEx(this->hProcess, NULL, strlen(this->lpDllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	if (this->pDllPath == NULL)
		throw std::exception("Allocating memory for path failed: ");

	// Write DLL path to remote process
	BOOL bProcWriteSuccess = WriteProcessMemory(this->hProcess, this->pDllPath, this->lpDllPath, strlen(this->lpDllPath) + 1, NULL);
	if (!bProcWriteSuccess)
		throw std::exception("Writing remote process memory failed: ");

	// Get a pointer to the LoadLibraryA function.
	LPVOID lpLoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	if (lpLoadLibraryAddress == NULL)
		throw std::exception("Could not obtain pointer to LoadLibraryA function: ");

	this->hRemoteThread = CreateRemoteThread(this->hProcess, 0, 0, (LPTHREAD_START_ROUTINE)lpLoadLibraryAddress, this->pDllPath, 0, 0);
	if (hRemoteThread == NULL)
		throw std::exception("Creating remote thread failed: ");

	WaitForSingleObject(this->hRemoteThread, INFINITE);

	std::cout << "--------\nDLL injected successfully.\n--------\n";

	// Returns TRUE indicating injection was successful.
	return TRUE;
}