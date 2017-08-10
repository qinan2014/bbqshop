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
				//EnableDebugPrivilege();
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
			std::string writemode = "w";
			bool recvData = false;
			switch (cds->dwData)
			{
			case HOOKAPI_CREATEFILEA:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileA.txt";
				writemode = "a";
				recvData = true;
				break;
			case HOOKAPI_CREATEFILEW:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileW.txt";
				writemode = "a";
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
				if((fp = fopen(logtxt.c_str(), writemode.c_str())) != NULL)
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