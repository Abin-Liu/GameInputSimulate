// WoWThread.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hModule = (HMODULE)hModule;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		g_hModule = NULL;
	}
    return TRUE;
}

