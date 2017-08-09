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

//BOOL (WINAPI *SysWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;
//BOOL WINAPI HookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
//{
//	//std::string workpath = GetWorkPath();
//	//workpath += "/";
//	//workpath += HOOKAPIDIR;
//	//workpath += "/";
//	//workpath += HOOKWRITEFUNC;
//	fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFile.txt", "w");
//	if(fp != NULL)
//	{
//		sprintf(logBuff, "write file toWrite: %d, byte written: %d\r\n", nNumberOfBytesToWrite, *lpNumberOfBytesWritten);
//		fwrite(logBuff, strlen(logBuff), 1, fp);
//
//		fwrite(lpBuffer, nNumberOfBytesToWrite, 1, fp);
//		fclose(fp);
//		fp = NULL;
//	}
//
//	return SysWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
//}

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
	PCONTENT_FILE_MAPPING pContent = NULL;
	// the file-mapping NAME_FILE_MAPPING has the infomation about hook or unhook apis
	HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NAME_FILE_MAPPING);
	do 
	{
		if(!hMap)
		{
			break ;
		}

		// alloc a new console of attach a exist console
		AllocConsole();
		COORD coordDest = {300, 3000}; 
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordDest);

		pContent = (PCONTENT_FILE_MAPPING)MapViewOfFile(
			hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CONTENT_FILE_MAPPING));
		if(!pContent)
		{
			break ;
		}

		DWORD64 dw64ApiFlags = pContent->dw64FlagNeedHook;
		BOOL bHook = bHook = pContent->bHook;
		for (int i = 0; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
		{
			// hook the api
			if (pContent->dw64FlagNeedHook & g_arHookAPIs[i].dw64Flag)
			{
				if (bHook ? HookSpecifyApi(&g_arHookAPIs[i]) : UnhookSpecifyApi(&g_arHookAPIs[i]))
				{
					//PrintMsg(bHook ? _T("dll Hook %s(%s) successful!\n") : _T("Unhook %s(%s) successful!\n"), 
					//	g_arHookAPIs[i].lpFunctionName, g_arHookAPIs[i].lpDllName);
					pContent->dw64FlagHookReturn |= (pContent->dw64FlagNeedHook & g_arHookAPIs[i].dw64Flag);
				}
				else
				{
					//PrintMsg(bHook ? _T("dll Unhook %s(%s) failed!\n") : _T("Unhook %s(%s) failed!\n"), 
					//	g_arHookAPIs[i].lpFunctionName, g_arHookAPIs[i].lpDllName);
				}
			}
		}

		if (!bHook)
		{
			int i;
			for (i = 0; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
			{
				if (g_arHookAPIs[i].pOrgfnMem)
				{
					break;
				}
			}

			// if no apis hooked, we should free the console, this operation will shutdown the console
			if (i == sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]))
			{
				pContent->bAllUnhooked = TRUE;
			}
		}
	} while (FALSE);

	CLOSE_HANDLE(hMap);
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
			//DisableThreadLibraryCalls(hModule);
			//DetourTransactionBegin();
			//DetourUpdateThread(GetCurrentThread());
			//LONG errNo1 = DetourAttach(&(PVOID&)SysWriteFile, HookWriteFile);
			//LONG errNo = DetourTransactionCommit();
			bbqPath[0] = 0;
			fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllmain.txt", "w");
			if(fp != NULL)
			{
				fwrite("DLL_PROCESS_ATTACH \r\n", strlen("DLL_PROCESS_ATTACH \r\n"), 1, fp);
				fclose(fp);
				fp = NULL;
			}
		}
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//DetourTransactionBegin();	//Detach
		//DetourUpdateThread(GetCurrentThread());
		//DetourDetach(&(PVOID&)SysWriteFile, HookWriteFile);
		//DetourTransactionCommit();
		break;
	}
	return TRUE;
}