#include "stdafx.h"
#include <AtlBase.h>
#include "ReCallApi.h"
#include <strsafe.h>

// hook apis infomation
RECALL_API_INFO g_arHookAPIs[] = 
{
	FLAG_HOOK_API(ORDER_MESSAGEBOXA),		_T("MessageBoxA"),		_T("user32.dll"), 
		MyMessageBoxA,		MessageBoxA,		NULL,	0,

	FLAG_HOOK_API(ORDER_MESSAGEBOXW),		_T("MessageBoxW"),		_T("user32.dll"), 
		MyMessageBoxW,		MessageBoxW,		NULL,	0,

	FLAG_HOOK_API(ORDER_DEVICEIOCONTROL),	_T("DeviceIoControl"),	_T("Kernel32.dll"), 
		MyDeviceIoControl,	DeviceIoControl,	NULL,	0,

	FLAG_HOOK_API(ORDER_CREATEFILEA),		_T("CreateFileA"),		_T("Kernel32.dll"), 
		MyCreateFileA,		CreateFileA,		NULL,	0,

	FLAG_HOOK_API(ORDER_CREATEFILEW),		_T("CreateFileW"),		_T("Kernel32.dll"), 
		MyCreateFileW,		CreateFileW,		NULL,	0,

	FLAG_HOOK_API(ORDER_READFILE),			_T("ReadFile"),			_T("Kernel32.dll"), 
		MyReadFile,			ReadFile,			NULL,	0,

	FLAG_HOOK_API(ORDER_READFILEEX),		_T("ReadFileEx"),		_T("Kernel32.dll"), 
		MyReadFileEx,		ReadFileEx,			NULL,	0,

	FLAG_HOOK_API(ORDER_WRITEFILE),			_T("WriteFile"),		_T("Kernel32.dll"), 
		MyWriteFile,		WriteFile,			NULL,	0,

	FLAG_HOOK_API(ORDER_WRITEFILEEX),		_T("WriteFileEx"),		_T("Kernel32.dll"), 
		MyWriteFileEx,		WriteFileEx,		NULL,	0
};

int WINAPI MyMessageBoxA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType)
{
	int nOrderHookApi = ORDER_MESSAGEBOXA;

	int nRet = 0;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//USES_CONVERSION;
		//PrintMsgA("%-18s %08d : 0x%08x \"%s\" \"%s\" 0x%08x\n",
		//	T2CA(g_arHookAPIs[nOrderHookApi].lpFunctionName),
		//	i++, hWnd, VALID_CHAR(lpText), VALID_CHAR(lpCaption), uType);

		nRet = ((pfnMessageBoxA)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hWnd, "HelloWorld", "Caption", MB_OKCANCEL);
	}
	return nRet;
}

int WINAPI MyMessageBoxW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType)
{
	int nOrderHookApi = ORDER_MESSAGEBOXW;
	
	int nRet = 0;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//USES_CONVERSION;
		//PrintMsgW(L"%-18s %08d : 0x%08x \"%s\" \"%s\" 0x%08x\n",
		//	T2CW(g_arHookAPIs[nOrderHookApi].lpFunctionName),
		//	i++, hWnd, VALID_WCHAR(lpText), VALID_WCHAR(lpCaption), uType);
		
		nRet = ((pfnMessageBoxW)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hWnd, L"Wide HelloWorld", L"Wide Caption", MB_OKCANCEL);
	}
	return nRet;
}

BOOL WINAPI MyDeviceIoControl(	HANDLE hDevice,
								DWORD dwIoControlCode,
								LPVOID lpInBuffer,
								DWORD nInBufferSize,
								LPVOID lpOutBuffer,
								DWORD nOutBufferSize,
								LPDWORD lpBytesReturned,
								LPOVERLAPPED lpOverlapped)
{
	int nOrderHookApi = ORDER_DEVICEIOCONTROL;
	
	BOOL bRet = FALSE;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//PrintMsg(_T("%-18s %08d : 0x%08x %d\n"), g_arHookAPIs[nOrderHookApi].lpFunctionName,
		//	i++, hDevice, dwIoControlCode);
	
		bRet = ((pfnDeviceIoControl)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize,
			lpBytesReturned, lpOverlapped);
	}
	return bRet;
}

HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName,
							DWORD dwDesiredAccess,
							DWORD dwShareMode,
							LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							DWORD dwCreationDisposition,
							DWORD dwFlagsAndAttributes,
							HANDLE hTemplateFile)
{
	int nOrderHookApi = ORDER_CREATEFILEA;
	
	HANDLE hFile = NULL;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//USES_CONVERSION;
		//PrintMsgA("%-18s %08d : \"%s\" \n", T2CA(g_arHookAPIs[nOrderHookApi].lpFunctionName), 
		//	i++, lpFileName);
		//
		hFile = ((pfnCreateFileA)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	return hFile;
}

HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName,
							DWORD dwDesiredAccess,
							DWORD dwShareMode,
							LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							DWORD dwCreationDisposition,
							DWORD dwFlagsAndAttributes,
							HANDLE hTemplateFile)
{
	int nOrderHookApi = ORDER_CREATEFILEW;
	
	HANDLE hFile = NULL;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//USES_CONVERSION;
		//PrintMsgW(L"%-18s %08d : \"%s\" \n", T2CW(g_arHookAPIs[nOrderHookApi].lpFunctionName), 
		//	i++, lpFileName);
		
		hFile = ((pfnCreateFileW)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}
	return hFile;
}

BOOL WINAPI MyReadFile(	HANDLE hFile,
						LPVOID lpBuffer,
						DWORD nNumberOfBytesToRead,
						LPDWORD lpNumberOfBytesRead,
						LPOVERLAPPED lpOverlapped)
{
	int nOrderHookApi = ORDER_READFILE;
	
	BOOL bRet = FALSE;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//PrintMsg(_T("%-18s %08d : 0x%08x %d\n"), g_arHookAPIs[nOrderHookApi].lpFunctionName,
		//	i++, hFile, nNumberOfBytesToRead);
		bRet = ((pfnReadFile)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
	}
	return bRet;
}

BOOL WINAPI MyReadFileEx(HANDLE hFile,
						 LPVOID lpBuffer,
						 DWORD nNumberOfBytesToRead,
						 LPOVERLAPPED lpOverlapped,
						 LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int nOrderHookApi = ORDER_READFILEEX;
	
	BOOL bRet = FALSE;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//PrintMsg(_T("%-18s %08d : 0x%08x %d\n"), g_arHookAPIs[nOrderHookApi].lpFunctionName,
		//	i++, hFile, nNumberOfBytesToRead);
		
		bRet = ((pfnReadFileEx)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
	}
	return bRet;
}

BOOL WINAPI MyWriteFile(HANDLE hFile,
						LPCVOID lpBuffer,
						DWORD nNumberOfBytesToWrite,
						LPDWORD lpNumberOfBytesWritten,
						LPOVERLAPPED lpOverlapped)
{
	int nOrderHookApi = ORDER_WRITEFILE;
	
	BOOL bRet = FALSE;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//PrintMsg(_T("%-18s %08d : 0x%08x %d\n"), g_arHookAPIs[nOrderHookApi].lpFunctionName,
		//	i++, hFile, nNumberOfBytesToWrite);
		
		bRet = ((pfnWriteFile)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	}
	return bRet;
}

BOOL WINAPI MyWriteFileEx(HANDLE hFile,
						  LPCVOID lpBuffer,
						  DWORD nNumberOfBytesToWrite,
						  LPOVERLAPPED lpOverlapped,
                          LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int nOrderHookApi = ORDER_WRITEFILEEX;
	
	BOOL bRet = FALSE;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		//PrintMsg(_T("%-18s %08d : 0x%08x %d\n"), g_arHookAPIs[nOrderHookApi].lpFunctionName,
		//	i++, hFile, nNumberOfBytesToWrite);
		
		bRet = ((pfnWriteFileEx)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
	}
	return bRet;
}