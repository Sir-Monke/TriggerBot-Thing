#include <windows.h>
#include "pch.h"

DWORD ori_addr = 0x0040ADA2;
DWORD ori_call_addr = 0x4607C0;
DWORD edi_var = 0;
INPUT input = { 0 };

__declspec(naked) void hookThing() {
	__asm {
		call ori_call_addr
		pushad
		mov edi_var, eax
	}
	if (edi_var != 0) {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &input, sizeof(INPUT));
	}
	else {
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &input, sizeof(INPUT));
	}
	__asm {
		popad 
		jmp ori_addr
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	DWORD old_protect;
	unsigned char* hookLoc = (unsigned char*)0x0040AD9D;

	if (fdwReason == DLL_PROCESS_ATTACH) {
		VirtualProtect((void*)hookLoc, 5, PAGE_EXECUTE_READWRITE, &old_protect);
		*hookLoc = 0xE9;
		*(DWORD*)(hookLoc + 1) = (DWORD)&hookThing - ((DWORD)hookLoc + 5);
	}
	return true;
}