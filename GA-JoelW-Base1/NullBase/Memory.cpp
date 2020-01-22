
#include "Includes.h"
//Vars
DWORD baseAddress = NULL;
DWORD engineAddress = NULL;
DWORD clientState = NULL;

//Vars för process snapshot
HANDLE hProcSnap = NULL;
PROCESSENTRY32 procEntry32;

//Vars för module snapshot
HANDLE hModuleSnap = NULL;
MODULEENTRY32 modEntry32;

//Process ID 
DWORD pID = NULL;

//Handle process
HANDLE hProc = NULL;

bool attatchProc(char* procName)
{
	//Definiera storlek av alla strukturer så vi kan fylla dem.
	procEntry32.dwSize = sizeof(PROCESSENTRY32);

	//copy alla snapshot av processes som är igång
	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to take snapshot of process list." << std::endl;
		return false;
	}

	//när det är en nästa process i snapshot
	while (Process32Next(hProcSnap, &procEntry32))
	{
		std::cout << procEntry32.szExeFile << std::endl;
		//om processen matchar den vi letar efter
		if (!strcmp(procName, procEntry32.szExeFile))
		{
			//Output proc name och ID sen få all access av processen och starta en Handle så vi kan läsa och skriva i minnet.
			std::cout << "Found process " << procEntry32.szExeFile << " with process ID " << procEntry32.th32ProcessID << std::endl;
			hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry32.th32ProcessID);
			pID = procEntry32.th32ProcessID;

			if (hProc == NULL)
			{
				std::cout << "Failed getting handle to process." << std::endl;
			}
			//Close handle to process snapshot och return true
			CloseHandle(hProcSnap);
			return true;
		}
	}

	std::cout << "Couldn't find " << procName << " in the process snapshot." << std::endl;
	CloseHandle(hProcSnap);
	return false;
}

DWORD getModule(LPSTR moduleName)
{
	//ta snapshot av modules i process
	hModuleSnap = INVALID_HANDLE_VALUE;
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to take a snapshot of modules." << std::endl;
		CloseHandle(hModuleSnap);
		return 0;
	}

	//Definiera structure size
	modEntry32.dwSize = sizeof(MODULEENTRY32);

	//hämta info om den första module in snapshot
	if (Module32First(hModuleSnap, &modEntry32))
	{
		//Om modulen matchar den vi letar efter
		if (!strcmp(moduleName, modEntry32.szModule))
		{
			//output module name och base address
			std::cout << "Found module " << modEntry32.szModule << " with base address " << std::hex << (DWORD)modEntry32.modBaseAddr << std::endl;

			//stäng handle av process snapshot sen returna base adressen
			CloseHandle(hModuleSnap);
			return (DWORD)modEntry32.modBaseAddr;
		}
	}

	//när det är en nästa module i snapshot
	while (Module32Next(hModuleSnap, &modEntry32))
	{
		//Om vi hittar den modulen vi letar efter i snapshot
		if (!strcmp(moduleName, modEntry32.szModule))
		{
			//output module name, base address
			std::cout << "Found module " << modEntry32.szModule << " with base address 0x" << std::hex << std::uppercase << (DWORD)modEntry32.modBaseAddr << std::endl;

			//stäng handle av process snapshot sen returna base adressen
			CloseHandle(hModuleSnap);
			return (DWORD)modEntry32.modBaseAddr;
		}
	}
	std::cout << "Couldn't find " << moduleName << " module in module snapshot of the process." << std::endl;
	CloseHandle(hModuleSnap);
	return false;
}

std::uint32_t find(const char* proc)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);

	if (Process32First(snapshot, &pe)) {
		while (Process32Next(snapshot, &pe))
		{
			if (!strcmp(proc, pe.szExeFile)) {
				CloseHandle(snapshot);
				//mylog << "Found proc!\n";
				return pe.th32ProcessID;
			}
		}
	}
	CloseHandle(snapshot);
	return 0;
}

//Start
void init()
{
	if (attatchProc(XOR("csgo.exe")))
	{
		baseAddress = getModule(XOR("client_panorama.dll"));
		engineAddress = getModule(XOR("engine.dll"));

		LocalPlayer::setLocalPlayer();

		clientState = rpm<DWORD>(engineAddress + offs::dwClientState);
	}
}

