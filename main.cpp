// XIV.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include "Memory.h"
#include "Process.h"
#include "FFXIV.h"

void initialize_ffxiv_data() {
	/*
		Updates ffxiv singleton with process information.
	*/
	FFXIV *ffxiv = FFXIV::instance();

	// Get data
	HWND hwnd = get_hwnd(ffxiv->window_name);
	DWORD pid = get_pid(hwnd);
	HANDLE handle = get_process_handle(pid);
	MODULEENTRY32 module = get_module(pid, ffxiv->process_name);

	// Update instance
	ffxiv->set_hwnd(hwnd);
	ffxiv->set_pid(pid);
	ffxiv->set_handle(handle);
	ffxiv->set_module(module);
}


//template<typename T>
DWORD protectMemory(HANDLE proc, unsigned long long adr, unsigned long long adr_size, DWORD prot) {
	DWORD oldProt;
	VirtualProtectEx(proc, (LPVOID) adr, adr_size, prot, &oldProt);
	return oldProt;
}

bool CompareData(BYTE* pbData, BYTE* pattern, const char* XsAndQuestionMarks)
{
	for (; *XsAndQuestionMarks; ++XsAndQuestionMarks, ++pbData, ++pattern)
	{
		if (*XsAndQuestionMarks == 'x' && *pbData != *pattern)
		{
			return FALSE;
		}
	}
	return (*XsAndQuestionMarks) == NULL;
}

BYTE* FindPattern(HANDLE hproc, BYTE* start, BYTE* pattern, char* XsAndQuestionMarks)
{
	size_t bytesRead;
	BYTE* data = NULL;
	MEMORY_BASIC_INFORMATION meminfo = { 0 };

	while (VirtualQueryEx(hproc, start, &meminfo, sizeof(meminfo)))
	{
		size_t inc_size = meminfo.RegionSize;
		data = new BYTE[inc_size];
		if ((meminfo.State && MEM_COMMIT) && (meminfo.Protect && PAGE_READWRITE)) {
			if (!ReadProcessMemory(hproc, start, data, inc_size, &bytesRead))
			{
				delete data;
				return NULL;
			}

			for (unsigned long long i = 0; i < inc_size; i++)
			{
				if (CompareData((BYTE*)(data + i), pattern, XsAndQuestionMarks))
				{
					delete data;
					return start + i;
				}
			}
		}
		
		start += inc_size;
		
	}
	
	delete data;
	return NULL;
}

int main()
{	
	FFXIV *ffxiv = FFXIV::instance();
	initialize_ffxiv_data();
	
	//send_key_press(xiv_process.hwnd);
	//send_key_press(xiv_process.hwnd);
	//send_key_press(xiv_process.hwnd);
	//send_key_press(xiv_process.hwnd);

	//HANDLE xiv_handle = ffxiv->get_handle();
	//unsigned long long xiv_base = ffxiv->get_base_address();
	//std::vector<long long> health_offsets = { 0x1828C34 };

	//while (true) {
	//	uint8_t health = read_address_pointers<uint8_t>(xiv_handle, xiv_base, health_offsets);
	//	//health = read_address<uint8_t>(xiv_handle, xiv_base + 0x1828C34);
	//	std::cout << unsigned(health);
	//	Sleep(600);
	//	system("cls");
	//}

	byte signature[] = "\xD0\x8C\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x62\xE7";
	char* mask = "xx?????xxxxxxxxxxx";

	MODULEENTRY32 mod_info = ffxiv->get_module();
	MEMORY_BASIC_INFORMATION meminfo = { 0 };

	//while (1)
	//{
	//	if (VirtualQueryEx(ffxiv->get_handle(), addr, &meminfo, sizeof(meminfo)) == 0)
	//	{
	//		break;
	//	}
	//	if (meminfo.State & MEM_COMMIT)
	//	{

	//		std::cout << meminfo.BaseAddress << " " << meminfo.RegionSize << std::endl;

	//	}
	//	addr = (BYTE*)meminfo.BaseAddress + meminfo.RegionSize;
	//	
	//}
	BYTE* res = FindPattern(ffxiv->get_handle(), (BYTE*)0x000001A18E88D000, signature, mask);
	return 0;
}
