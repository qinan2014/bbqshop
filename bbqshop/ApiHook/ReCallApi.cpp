#include "stdafx.h"
#include <cstdio>
#include <windows.h>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include "ReCallApi.h"
#include <strsafe.h>
#include <string>


FILE *fp = NULL;

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

void SendMessageToMain(LPCWSTR lpContent, int selfType)
{
	HWND hwnd = ::FindWindowW(NULL, L"DetourInjectDlg");
	COPYDATASTRUCT copydata;
	copydata.dwData = selfType;  // 用户定义数据
	copydata.lpData = (PVOID)lpContent;  //指向数据的指针
	copydata.cbData = wcslen(lpContent);  // 数据大小
	::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(GetActiveWindow()), reinterpret_cast<LPARAM>(&copydata));
}

void SendMessageToMain(LPCSTR lpContent, int selfType)
{
	HWND hwnd = ::FindWindowW(NULL, L"DetourInjectDlg");
	COPYDATASTRUCT copydata;
	copydata.dwData = selfType;  // 用户定义数据
	copydata.lpData = (PVOID)lpContent;  //指向数据的指针
	copydata.cbData = strlen(lpContent);  // 数据大小
	::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(GetActiveWindow()), reinterpret_cast<LPARAM>(&copydata));
}

int WINAPI MyMessageBoxA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType)
{
	int nOrderHookApi = ORDER_MESSAGEBOXA;

	int nRet = 0;
	static int i = 1;
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{

		nRet = ((pfnMessageBoxA)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hWnd, lpText, lpCaption, MB_OKCANCEL);
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
		
		nRet = ((pfnMessageBoxW)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hWnd, lpText, lpCaption, MB_OKCANCEL);
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
		//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllcreateFileA.txt", "w");
		//if(fp != NULL)
		//{
		//	fwrite("MyCreateFileA \r\n", strlen("MyCreateFileA \r\n"), 1, fp);
		//	fclose(fp);
		//	fp = NULL;
		//}

		hFile = ((pfnCreateFileA)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

		SendMessageToMain(lpFileName, 8888);
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
	if (g_arHookAPIs[nOrderHookApi].pOrgfnMem)
	{
		hFile = ((pfnCreateFileW)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, 
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

		SendMessageToMain(lpFileName, 8888);
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
		//fopen_s(&fp, "d:\\qinan\\companyprogram\\gitproj\\bbqshop\\bbqshop\\debug\\hookdllreadfile.txt", "w");
		//if(fp != NULL)
		//{
		//	fwrite("myreadfile \r\n", strlen("myreadfile \r\n"), 1, fp);
		//	fclose(fp);
		//	fp = NULL;
		//}

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
		//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllreadFileEx.txt", "w");
		//if(fp != NULL)
		//{
		//	fwrite("MyReadFileEx \r\n", strlen("MyReadFileEx \r\n"), 1, fp);
		//	fclose(fp);
		//	fp = NULL;
		//}
		
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
		//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllwritefile.txt", "w");
		//if(fp != NULL)
		//{
		//	fwrite("MyWriteFile \r\n", strlen("MyWriteFile \r\n"), 1, fp);
		//	fclose(fp);
		//	fp = NULL;
		//}
		
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
		fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdllwritefileEx.txt", "w");
		if(fp != NULL)
		{
			fwrite("MyWriteFileEx \r\n", strlen("MyWriteFileEx \r\n"), 1, fp);
			fclose(fp);
			fp = NULL;
		}

		bRet = ((pfnWriteFileEx)(LPVOID)g_arHookAPIs[nOrderHookApi].pOrgfnMem)(
			hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
	}
	return bRet;
}