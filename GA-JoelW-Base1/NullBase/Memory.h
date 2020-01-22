#pragma  once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "Offsets.hpp"


extern DWORD baseAddress;
extern DWORD engineAddress;
extern DWORD clientState;


extern HANDLE hProcSnap;
extern PROCESSENTRY32 procEntry32;


extern HANDLE hModuleSnap;
extern MODULEENTRY32 modEntry32;


extern DWORD pID;

//Handle to process
extern HANDLE hProc;

//WPM wrapper -g�r s� vi kan kalla WriteProcessMemory 
template <class dataType>
void wpm(dataType valToWrite, DWORD addressToWrite)
{
	WriteProcessMemory(hProc, (PVOID)addressToWrite, &valToWrite, sizeof(dataType), 0);
}


template <class dataType>
dataType rpm(DWORD addressToRead)
{
	//sparar v�rdet n�r den har l�st
	dataType rpmBuffer;

	//RPM
	ReadProcessMemory(hProc, (PVOID)addressToRead, &rpmBuffer, sizeof(dataType), 0);

	
	return rpmBuffer;
}

//attach procssensen s� vi kan anv�nda /RPM och WPM och s� vi kan s�tta de till en offset senare
extern bool attatchProc(char* procName);

extern DWORD getModule(char* moduleName);

extern uintptr_t patternScan(char* base, size_t size, char* pattern);

extern void init();

extern std::uint32_t find(const char* proc);