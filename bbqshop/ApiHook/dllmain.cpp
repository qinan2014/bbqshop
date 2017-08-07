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

FILE * fp = NULL;
char bbqPath[MAX_PATH];

char *GetWorkPath()
{
	if (strlen(bbqPath) > 0)
		return bbqPath;
	HKEY hAppKey = 0;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hAppKey))
		return "";
	DWORD dataType;
	DWORD dataSize;
	LONG res = RegQueryValueEx(hAppKey, L"bbqpay", 0, &dataType, 0, &dataSize);
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
	res = RegQueryValueEx(hAppKey, L"bbqpay", 0, 0, odata, &dataSize);
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

	//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\processmessage.txt", "a");
	//if(fp != NULL)
	//{
	//	char tmpbuf[100];
	//	sprintf(tmpbuf, "name pos : %d\r\n", namePos);
	//	fwrite(tmpbuf, strlen(tmpbuf), 1, fp);
	//	sprintf(tmpbuf, "bbqPath length : %d\r\n", strlen(bbqPath));
	//	fwrite(tmpbuf, strlen(tmpbuf), 1, fp);
	//	fwrite(bbqPath, strlen(bbqPath), 1, fp);
	//	fwrite("\r\n", strlen("\r\n"), 1, fp);
	//	fclose(fp);
	//	fp = NULL;
	//}

	RegCloseKey(hAppKey);
	delete []odata;
	return bbqPath;
}

//bool SendProcessMessage( ULONG_PTR dataType, std::string willSendData)
//{
//	HWND hwnd = ::FindWindowW(NULL, FLOATWINTITLEW);
//	char *cstr = new char[willSendData.length() + 1];
//	strcpy(cstr, willSendData.c_str());
//	COPYDATASTRUCT copydata;
//	copydata.dwData = dataType;  // 用户定义数据
//	copydata.lpData = cstr;  //数据大小
//	copydata.cbData = willSendData.length();  // 指向数据的指针
//	::SendMessage(hwnd, WM_COPYDATA, reinterpret_cast<WPARAM>(hwnd), reinterpret_cast<LPARAM>(&copydata));
//	delete [] cstr;
//	return true;
//}


//int (WINAPI *SysMessageBox)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) = MessageBox;
//int WINAPI HookMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
//{	
//	return SysMessageBox(hWnd, L"HookMsg调用中...", lpCaption, uType);
//}

BOOL (WINAPI *SysWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped) = WriteFile;
BOOL WINAPI HookWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	//SendProcessMessage(ZHIHUI_CODE_MSG_HOOK_WRITEFILE, "hello test hook write file");
	
	//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\processmessage.txt", "a");
	//if(fp != NULL)
	//{
	//	fwrite("lpBuffer", strlen("HookWriteFile test\r\n"), 1, fp);
	//	fwrite("\r\n", strlen("\r\n"), 1, fp);
	//	fclose(fp);
	//	fp = NULL;
	//}
	GetWorkPath();

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
		bbqPath[0] = 0;
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