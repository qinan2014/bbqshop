// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
#include <cstdio>
#include <windows.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "HookKeyChar.h"
#include "ReCallApi.h"
#include "atlconv.h"
#include "common.h"
#include "ZhuiHuiMsg.h"
PCONTENT_FILE_MAPPING pFileMapContent = NULL;
HANDLE hFileMapping = NULL;

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
	for (int i = ORDER_CREATEFILEA; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
	{
		// hook the api
		HookSpecifyApi(&g_arHookAPIs[i]);
	}

	// 创建共享内存
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, NAME_FILE_MAPPINGT);
	if(!hFileMapping)
	{
		//PrintError(_T("CreateFileMapping"), GetLastError(), __MYFILE__, __LINE__);
		//break ;
	}
	pFileMapContent = (PCONTENT_FILE_MAPPING)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CONTENT_FILE_MAPPING));
}

void UnHookApi()
{
	for (int i = ORDER_CREATEFILEA; i < sizeof(g_arHookAPIs) / sizeof(g_arHookAPIs[0]); i++)
	{
		// hook the api
		UnhookSpecifyApi(&g_arHookAPIs[i]);
	}
	if (pFileMapContent != NULL)
	{
		UnmapViewOfFile(pFileMapContent); //解除映射
		CloseHandle(hFileMapping); //一个指定的文件映射对象
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
		HookAPI();
		break;
	case DLL_PROCESS_DETACH:
		UnHookApi();
		break;
	default:
		break;
	}
	return TRUE;
}