#include "detourinjection.h"
#include <Windows.h>
#pragma comment(lib ,"user32.lib")
#include <Tlhelp32.h>
#include <time.h>
#pragma comment (lib, "ws2_32.lib")

DetourInjection::DetourInjection(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	BOOL bProcess = Process32First(hsnap, &pe32);
	bool flag = false;
	if (bProcess == TRUE)
	{
		while (Process32Next(hsnap, &pe32) == TRUE)
		{
			if (wcscmp(pe32.szExeFile, L"PCommTest.exe") == 0)
			{
				wchar_t* DirPath = new wchar_t[MAX_PATH];
				wchar_t* FullPath = new wchar_t[MAX_PATH];
				flag = true;
				GetCurrentDirectory(MAX_PATH, DirPath);
				swprintf_s(FullPath, MAX_PATH, L"D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\ApiHook.dll", DirPath);
				//swprintf_s(FullPath, MAX_PATH, L"C:\\Users\\Boboqi\\Desktop\\shopbincpy\\ApiHook.dll", DirPath);
				//std::wcout << L"find the target process ..." << endl;
				//wcout << L"the dir of dll is: " << FullPath << endl;
				HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
					PROCESS_VM_WRITE, FALSE, pe32.th32ProcessID);
				//wcout << L"open the target process..." << endl;
				LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
					"LoadLibraryW");
				//wcout << L"LoadLibary..." << endl;
				LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, (wcslen(FullPath) + 1) * sizeof(wchar_t),
					MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				WriteProcessMemory(hProcess, LLParam, FullPath, (wcslen(FullPath) + 1)* sizeof(wchar_t), NULL);
				//wcout << L"path to the target process..." << endl;
				HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
					LLParam, NULL, NULL);
				//if (hRemoteThread)
				//	wcout << L"create the remote thread for LoadLibrary success...." << endl;
				//else
				//	wcout << L"create the remote thread for LoadLibrary fail...." << endl;
				CloseHandle(hProcess); 
				delete[] DirPath;
				delete[] FullPath;
				break;
			}
		}
	}
	//if (!flag)
	//	cout << "sorry not find the process..." << endl;
	//while (1);
}

DetourInjection::~DetourInjection()
{

}
