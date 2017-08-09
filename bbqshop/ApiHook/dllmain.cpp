// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
//
//#pragma comment(lib, "detours.lib")
//#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <windows.h>
//#include <detours.h> 
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "HookKeyChar.h"
#include "ReCallApi.h"
#include "atlconv.h"
#include "common.h"

static FILE * fp = NULL;

char bbqPath[MAX_PATH];
char logBuff[255];

#define PROJECTREGIEDT "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define PROJCETNAME "bbqpay"

char *GetWorkPath()
{
	if (strlen(bbqPath) > 0)
		return bbqPath;
	HKEY hAppKey = 0;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, PROJECTREGIEDT, 0, KEY_READ, &hAppKey))
		return "";
	DWORD dataType;
	DWORD dataSize;
	LONG res = RegQueryValueEx(hAppKey, PROJCETNAME, 0, &dataType, 0, &dataSize);
	if (res != ERROR_SUCCESS) 
	{
		RegCloseKey(hAppKey);
		return "";
	}
	if (dataType == REG_SZ || dataType == REG_EXPAND_SZ)
		dataSize += 2;
	else if (dataType == REG_MULTI_SZ)
		dataSize += 4;
	static unsigned char *odata = new unsigned char[dataSize];
	res = RegQueryValueEx(hAppKey, PROJCETNAME, 0, 0, odata, &dataSize);
	if (res != ERROR_SUCCESS) {
		RegCloseKey(hAppKey);
		delete []odata;
		return "";
	}
	// 去除空格
	int charpos = 0;
	for (int i = 0; i < dataSize; ++i)
	{
		if (odata[i] != '\0' && odata[i] != ' ')
		{
			bbqPath[charpos] = odata[i];
			++charpos;
		}
	}
	bbqPath[charpos] = 0;
	// 找到路径
	int namePos = -1;
	for (int i = 0; i < strlen(bbqPath); ++i)
	{

		if (bbqPath[i] == '\\')
			namePos = i;
	}
	bbqPath[namePos] = 0;
	RegCloseKey(hAppKey);
	delete []odata;
	return bbqPath;
}

BOOL UnhookSpecifyApi(PRECALL_API_INFO pRecallApiInfo)
{
	BOOL bRet = FALSE;	
	do 
	{
		if (!pRecallApiInfo)
		{
			break;
		}
		if (!pRecallApiInfo->pOrgfnMem || pRecallApiInfo->nOrgfnMemSize < 10)
		{
			bRet = TRUE;
			break;
		}
		HMODULE hModule = LoadLibrary(pRecallApiInfo->lpDllName);
		if (!hModule)
		{
			//PrintError(_T("LoadLibrary"), GetLastError(), __MYFILE__, __LINE__);
			break;
		}
		USES_CONVERSION;
		FARPROC pfnStartAddr = (FARPROC)GetProcAddress(hModule, T2CA(pRecallApiInfo->lpFunctionName));
		pRecallApiInfo->lpApiAddr = pfnStartAddr;
		if (!pfnStartAddr)
		{
			//PrintError(_T("GetProcAddress"), GetLastError(), __MYFILE__, __LINE__);
			break ;
		}

		DWORD dwProtect = 0;
		if (!VirtualProtect(pfnStartAddr, pRecallApiInfo->nOrgfnMemSize - 5, 
			PAGE_EXECUTE_READWRITE, &dwProtect))
		{
			//PrintError(_T("VirtualProtect"), GetLastError(), __MYFILE__, __LINE__);
			break ;
		}

		// restore bytes of the apis
		memcpy(pfnStartAddr, pRecallApiInfo->pOrgfnMem, pRecallApiInfo->nOrgfnMemSize - 5);
		VirtualProtect(pfnStartAddr, pRecallApiInfo->nOrgfnMemSize - 5, dwProtect, &dwProtect);

		delete [] pRecallApiInfo->pOrgfnMem;
		pRecallApiInfo->pOrgfnMem = NULL;
		pRecallApiInfo->nOrgfnMemSize = 0;

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

// hook the specify api
// pRecallApiInfo : infomation of the api
BOOL HookSpecifyApi(PRECALL_API_INFO pRecallApiInfo)
{
	BOOL bRet = FALSE;	
	do 
	{
		if (!pRecallApiInfo)
		{
			break;
		}
		if (pRecallApiInfo->pOrgfnMem)
		{
			bRet = TRUE;
			break;
		}
		HMODULE hModule = LoadLibrary(pRecallApiInfo->lpDllName);
		if (!hModule)
		{
			//PrintError(_T("LoadLibrary"), GetLastError(), __MYFILE__, __LINE__);
			break;
		}
		USES_CONVERSION;
		FARPROC pfnStartAddr = (FARPROC)GetProcAddress(hModule, T2CA(pRecallApiInfo->lpFunctionName));
		pRecallApiInfo->lpApiAddr = pfnStartAddr;
		if (!pfnStartAddr)
		{
			//PrintError(_T("GetProcAddress"), GetLastError(), __MYFILE__, __LINE__);
			break ;
		}

		// we must save the first few bytes of the api(at least five, and these few bytes must complete
		// the assembly codes), then make the 5 bytes in front of api to jump to our function, and our
		// function must execute the few bytes saved before, and then jump to the api to execute
		// the rest code in the api
		int nSize = 0; 
		int nDisassemblerLen = 0;
		while(nSize < 5) 
		{ 
			// GetOpCodeSize can get the assembly code size
			nDisassemblerLen = GetOpCodeSize((BYTE*)(pfnStartAddr) + nSize);
			nSize = nDisassemblerLen + nSize; 
		} 

		DWORD dwProtect = 0;
		if (!VirtualProtect(pfnStartAddr, nSize, PAGE_EXECUTE_READWRITE, &dwProtect))
		{
			//PrintError(_T("VirtualProtect"), GetLastError(), __MYFILE__, __LINE__);
			break ;
		}

		// be sure that we must change pOrgfnMem's protect, because the code in pOrgfnMem 
		// also need to execute 
		pRecallApiInfo->pOrgfnMem = new BYTE[5 + nSize];
		DWORD dwMemProtect = 0;
		if (!VirtualProtect(pRecallApiInfo->pOrgfnMem, 5 + nSize, PAGE_EXECUTE_READWRITE, &dwMemProtect))
		{
			delete [] pRecallApiInfo->pOrgfnMem;
			pRecallApiInfo->pOrgfnMem = NULL;
			//PrintError(_T("VirtualProtect"), GetLastError(), __MYFILE__, __LINE__);
			break ;
		}
		pRecallApiInfo->nOrgfnMemSize = 5 + nSize;

		memcpy(pRecallApiInfo->pOrgfnMem, pfnStartAddr, nSize);
		*(BYTE*)(pRecallApiInfo->pOrgfnMem + nSize) = 0xE9;
		*(DWORD*)(pRecallApiInfo->pOrgfnMem + nSize + 1) 
			= (DWORD)pfnStartAddr + nSize - (DWORD)(pRecallApiInfo->pOrgfnMem + 5 + nSize);
		*(BYTE*)(pfnStartAddr) = 0xE9;
		*(DWORD*)((BYTE*)pfnStartAddr + 1) = (DWORD)pRecallApiInfo->lpRecallfn - ((DWORD)pfnStartAddr + 5);
		memset((BYTE*)pfnStartAddr + 5, 0x90, nSize - 5);
		// be sure that we must set the rest to 0x90(assembly code for nop, do nothing, 
		// and occupy one byte), because we should't change the assembly code

		VirtualProtect(pfnStartAddr, nSize, dwProtect, &dwProtect);

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}


void HookAPI()
{
	for (int i = 0; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
	{
		// hook the api
		HookSpecifyApi(&g_arHookAPIs[i]);
	}
}

void UnHookApi()
{
	for (int i = 0; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
	{
		// hook the api
		UnhookSpecifyApi(&g_arHookAPIs[i]);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		{
			bbqPath[0] = 0;
			fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllmain.txt", "w");
			if(fp != NULL)
			{
				fwrite("DLL_PROCESS_ATTACH \r\n", strlen("DLL_PROCESS_ATTACH \r\n"), 1, fp);
				fclose(fp);
				fp = NULL;
			}

			HookAPI();
		}
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		UnHookApi();
		break;
	}
	return TRUE;
}