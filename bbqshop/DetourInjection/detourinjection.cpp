#include <Windows.h>
#include "detourinjection.h"
#pragma comment(lib ,"user32.lib")
#include <Tlhelp32.h>
#include <time.h>
#pragma comment (lib, "ws2_32.lib")
#include "ReCallApi.h"
#include "common.h"
#include <Strsafe.h>
#include "HookApi.h"

void EnableDebugPrivilege()
{
	HANDLE			 hToken;
	TOKEN_PRIVILEGES tp;
	LUID			 luid;
	if(::OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

		tp.PrivilegeCount		    = 1;
		tp.Privileges[0].Luid	    = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		::AdjustTokenPrivileges(hToken, FALSE, &tp,
			sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	}
}

DetourInjection::DetourInjection(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle("DetourInjectDlg");

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	BOOL bProcess = Process32First(hsnap, &pe32);
	bool flag = false;
	if (bProcess == TRUE)
	{
		while (Process32Next(hsnap, &pe32) == TRUE)
		{
			if (wcscmp(pe32.szExeFile, L"PosTouch.exe") == 0)
			//if (wcscmp(pe32.szExeFile, L"PCommTest.exe") == 0)
			{
				wchar_t* DirPath = new wchar_t[MAX_PATH];
				wchar_t* FullPath = new wchar_t[MAX_PATH];
				flag = true;
				GetCurrentDirectory(MAX_PATH, DirPath);
				swprintf_s(FullPath, MAX_PATH, L"D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\ApiHook.dll", DirPath);
				EnableDebugPrivilege();
				HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
					PROCESS_VM_WRITE, FALSE, pe32.th32ProcessID);
				LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
					"LoadLibraryW");
				LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, (wcslen(FullPath) + 1) * sizeof(wchar_t),
					MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				WriteProcessMemory(hProcess, LLParam, FullPath, (wcslen(FullPath) + 1)* sizeof(wchar_t), NULL);
				HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
					LLParam, NULL, NULL);
				CloseHandle(hProcess); 
				delete[] DirPath;
				delete[] FullPath;

				//injectDll(pe32.th32ProcessID, 511, L"D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\ApiHook.dll");
				break;
			}
		}
	}
	//if (!flag)
	//	cout << "sorry not find the process..." << endl;
	//while (1);

	connect(ui.pushButton, SIGNAL(pressed()), this, SLOT(readHookFile()));
}

DetourInjection::~DetourInjection()
{

}

bool DetourInjection::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG *param = static_cast<MSG *>(message);

	switch (param->message)
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(param->lParam);
			std::string logtxt;
			bool recvData = false;
			switch (cds->dwData)
			{
			case HOOKAPI_CREATEFILEA:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileA.txt";
				recvData = true;
				break;
			case HOOKAPI_CREATEFILEW:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileW.txt";
				recvData = true;
				break;
			case HOOKAPI_READFILE:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookReadFile.txt";
				recvData = true;
				break;
			case HOOKAPI_READFILEEX:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookReadFileEX.txt";
				recvData = true;
				break;
			case HOOKAPI_WRITEFILE:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFile.txt";
				recvData = true;
				break;
			case HOOKAPI_WRITEFILEEX:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFileEX.txt";
				recvData = true;
				break;
			default:
				break;
			}

			if (recvData)
			{
				FILE * fp = NULL;
				if((fp = fopen(logtxt.c_str(), "a")) != NULL)
				{
					fwrite(cds->lpData, cds->cbData, 1, fp);
					fwrite("\r\n\r\n", strlen("\r\n\r\n"), 1, fp);
					fclose(fp);
				}

				*result = 1;
				return true;
			}
		}
	}

	return QWidget::nativeEvent(eventType, message, result);
}

void DetourInjection::readHookFile()
{
	FILE *fp = NULL;
	char tmpbuf[255];
	fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFile.txt", "w");
	if(fp != NULL)
	{
		//sprintf(logBuff, "write file toWrite: %d, byte written: %d\r\n", nNumberOfBytesToWrite, *lpNumberOfBytesWritten);
		//fwrite(logBuff, strlen(logBuff), 1, fp);
		fread(tmpbuf, 100, 1, fp);
		fclose(fp);
		fp = NULL;
	}
}



// allocatte a memory in remote process, and write the dll name in it
LPVOID WriteProcessMem(HANDLE hProcess, LPCTSTR lpText)
{
	LPVOID pRemoteMem = NULL;
	do 
	{
		if(!hProcess)
		{
			break;
		}

		size_t cchLength = 0;
		HRESULT hr = StringCbLength(lpText, MAX_PATH, &cchLength);
		cchLength += sizeof(TCHAR);
		if (FAILED(hr))
		{
			break;
		}

		pRemoteMem = VirtualAllocEx(hProcess, NULL, cchLength, MEM_COMMIT, PAGE_READWRITE);
		if(!pRemoteMem)
		{
			break;
		}
		if(!WriteProcessMemory(hProcess, pRemoteMem, (LPVOID)lpText, cchLength, NULL))
		{
			VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
			pRemoteMem = NULL;
			break;
		}
	} while (FALSE);
	return pRemoteMem;
}

LPTHREAD_START_ROUTINE GetLoadLibraryAddr()
{
	LPTHREAD_START_ROUTINE pfnStartAddr = NULL;

	HMODULE hModule = LoadLibrary(L"Kernel32.dll");
	do 
	{
		if (!hModule)
		{
			break;
		}
		pfnStartAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "LoadLibraryA");
		if(!pfnStartAddr)
		{
		}
	} while (FALSE);
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = NULL;
	}
	return pfnStartAddr;
}

LPVOID WriteFileMapping(HANDLE hMap, CONTENT_FILE_MAPPING content)
{
	LPVOID pContent = NULL;
	do 
	{
		if(!hMap)
		{
			break;
		}
		pContent = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(content));
		if(!pContent)
		{
			break;
		}
		memcpy(pContent, &content, sizeof(content));
	} while (FALSE);
	return pContent;
}

BOOL CreateRemoteThreadEX(HANDLE hProcess, LPTHREAD_START_ROUTINE pfnStartAddr, LPVOID pRemoteMem)
{
	BOOL bRet = FALSE;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnStartAddr, pRemoteMem, 0, NULL);
	do 
	{
		if(!hThread)
		{
			break;
		}
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		hThread = NULL;
		bRet = TRUE;
	} while (FALSE);
	return bRet;
}

void DetourInjection::injectDll(unsigned long processID, unsigned long long hookApiFlag, wchar_t *dllPath)
{
	DWORD	dwProcessID	= processID;
	HANDLE	hProcess	= NULL;
	LPVOID	pRemoteMem	= NULL;
	HANDLE	hMap		= NULL;
	BOOL	bHook		= FALSE;

	do 
	{
		DWORD64 dw64ApiFlags = hookApiFlag;

		EnableDebugPrivilege();
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
		pRemoteMem = WriteProcessMem(hProcess, dllPath);

		LPTHREAD_START_ROUTINE pfnStartAddr = GetLoadLibraryAddr();

		CONTENT_FILE_MAPPING content;
		memset(&content, 0, sizeof(content));
		content.dw64FlagNeedHook = dw64ApiFlags;
		content.bHook = bHook;

		hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(content), NAME_FILE_MAPPINGL);
	
		LPVOID pContent = WriteFileMapping(hMap, content);
		CreateRemoteThreadEX(hProcess, pfnStartAddr, pRemoteMem);

	} while (FALSE);

	if (hProcess)
	{
		if (pRemoteMem)
		{
			VirtualFreeEx(hProcess, pRemoteMem, 0, MEM_RELEASE);
			pRemoteMem = NULL;
		}
		CLOSE_HANDLE(hProcess);
	}
	CLOSE_HANDLE(hMap);
}
