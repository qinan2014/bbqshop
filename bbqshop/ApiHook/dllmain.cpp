// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "Ws2_32.lib")

#include <cstdio>
#include <windows.h>
#include <detours.h> 
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "ZhuiHuiMsg.h"
#include "AllWindowTitle.h"
//#include "json/json.h"
#include "ProcessProtocal.h"
using namespace std;

bool SendProcessMessage(std::string willSendData)
{
	HWND hwnd = ::FindWindowW(NULL, FLOATWINTITLEW);
	char *cstr = new char[willSendData.length() + 1];
	strcpy(cstr, willSendData.c_str());
	COPYDATASTRUCT copydata;
	copydata.dwData = ZHIHUI_CODE_MSG;  // 用户定义数据
	copydata.lpData = cstr;  //数据大小
	copydata.cbData = willSendData.length();  // 指向数据的指针
	::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(hwnd), reinterpret_cast<LPARAM>(&copydata));
	delete [] cstr;
	return true;
}


//int (WINAPI *SysMessageBox)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) = MessageBox;
//int WINAPI HookMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
//{	
//	return SysMessageBox(hWnd, L"HookMsg调用中...", lpCaption, uType);
//}

BOOL (WINAPI *SysWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;
BOOL WINAPI HookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	//Json::Value mValData;
	//mValData[PRO_HEAD] = TO_MAINDLG_IMPORTANTDATA;

	SendProcessMessage("hellotest");
	MessageBox(NULL, L"HookWriteFile!", L"Warnning!", MB_OK);

	return SysWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		//DetourAttach(&(PVOID&)SysMessageBox, HookMessageBox);
		DetourAttach(&(PVOID&)SysWriteFile, HookWriteFile);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();	//Detach
		DetourUpdateThread(GetCurrentThread());
		//DetourDetach(&(PVOID&)SysMessageBox, SysMessageBox);
		DetourDetach(&(PVOID&)SysWriteFile, HookWriteFile);
		DetourTransactionCommit();
		break;
	}
	return TRUE;
}