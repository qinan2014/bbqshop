#include <Windows.h>
#include "detourinjection.h"
#pragma comment(lib ,"user32.lib")
#include <Tlhelp32.h>
#include <time.h>
#pragma comment (lib, "ws2_32.lib")
#include "ReCallApi.h"
#include "common.h"
#include <Strsafe.h>
#include "ZhuiHuiMsg.h"

LPVOID pContent;

DetourInjection::DetourInjection(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowTitle("DetourInjectDialog");

	// 开启内存共享
	CONTENT_FILE_MAPPING content;
	memzero(&content, sizeof(content));
	HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(content), NAME_FILE_MAPPINGL);
	if(!hMap)
	{
		//PrintError(_T("CreateFileMapping"), GetLastError(), __MYFILE__, __LINE__);
	}
	pContent = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(content));
	if(!pContent)
	{
		//PrintError(_T("MapViewOfFile"), GetLastError(), __MYFILE__, __LINE__);
		//break;
	}
	memcpy(pContent, &content, sizeof(content));

	ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	BOOL bProcess = Process32First(hsnap, &pe32);
	bool flag = false;
	if (bProcess == TRUE)
	{
		while (Process32Next(hsnap, &pe32) == TRUE)
		{
			if (wcscmp(pe32.szExeFile, L"Qiantai.exe") == 0)
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
				if (hProcess == NULL)
					break;
				LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"),
					"LoadLibraryW");
				if (LoadLibraryAddr == NULL)
				{
					CloseHandle(hProcess);
					break;
				}
				LPVOID LLParam = (LPVOID)VirtualAllocEx(hProcess, NULL, (wcslen(FullPath) + 1) * sizeof(wchar_t),
					MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
				BOOL SUC = WriteProcessMemory(hProcess, LLParam, FullPath, (wcslen(FullPath) + 1)* sizeof(wchar_t), NULL);
				HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddr,
					LLParam, NULL, NULL);

				// 等待远程线程结束    
				::WaitForSingleObject(hRemoteThread, INFINITE);    
				// 清理    
				::VirtualFreeEx(hProcess, LLParam, (wcslen(FullPath) + 1), MEM_DECOMMIT);    
				::CloseHandle(hRemoteThread);    
				::CloseHandle(hProcess);

				delete[] DirPath;
				delete[] FullPath;
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
			std::string hooktype;
			bool recvData = false;
			switch (cds->dwData)
			{
			case HOOKAPI_CREATEFILEA:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileA.txt";
				writemode = "a";
				hooktype = "CreateFileA";
				recvData = true;
				break;
			case HOOKAPI_CREATEFILEW:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCreateFileW.txt";
				writemode = "a";
				hooktype = "CreateFileW";
				recvData = true;
				break;
			//case HOOKAPI_READFILE:
			//	logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookReadFile.txt";
			//	recvData = true;
			//	break;
			//case HOOKAPI_READFILEEX:
			//	logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookReadFileEX.txt";
			//	recvData = true;
			//	break;
			case HOOKAPI_WRITEFILE:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFile.txt";
				hooktype = "WriteFile";
				recvData = true;
				break;
			case HOOKAPI_WRITEFILEEX:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFileEX.txt";
				hooktype = "WriteFileEX";
				recvData = true;
				break;
			case HOOKAPI_CLOSEHANDLE:
				logtxt = "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookCloseHandle.txt";
				hooktype = "CloseHandle";
				recvData = true;
				break;
			default:
				break;
			}

			if (recvData && cds->cbData < 100)
			{
				FILE * fp = NULL;
				if((fp = fopen("D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookdata.txt", "a")) != NULL)
				{
					QString strMessage = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData), cds->cbData);

					fwrite(cds->lpData, cds->cbData, 1, fp);
					char tmpbuf[100];
					static int tms = 0;
					sprintf(tmpbuf, "\r\nhook time %d, operate type: %s \r\n\r\n", tms++, hooktype.c_str());
					fwrite(tmpbuf, strlen(tmpbuf), 1, fp);
					fclose(fp);
				}

				*result = 1;
				return true;
			}
		}
	}

	return QWidget::nativeEvent(eventType, message, result);
}

bool isPriceTag(PVOID lpContent, int pContentSize)
{
	//char *priceCMD = "QA";
	//if (pContentSize > COMGETPRICECHARLEN)
	//	return false;
	//char *pChar = (char *)lpContent;
	//int priceTagSZ = strlen(priceCMD);
	//// 找到价格标志的初始位置
	//int priceTagBeginPos = -1;
	//for (int i = 0; i < pContentSize; ++i)
	//{
	//	if (pChar[i] == priceCMD[0])
	//	{
	//		priceTagBeginPos = i;
	//		break;
	//	}
	//}
	//for (int i = 0; i < priceTagSZ; ++i)
	//{
	//	if (pChar[priceTagBeginPos++] != priceCMD[i])
	//		return false;
	//}
	return true;
}

void DetourInjection::readHookFile()
{
	CONTENT_FILE_MAPPING *maping = (CONTENT_FILE_MAPPING *)pContent;

	//char *price = "1QA0.01";
	//bool ispc = isPriceTag(price, 10);

	//FILE *fp = NULL;
	//char tmpbuf[255];
	//fopen_s(&fp, "D:\\QinAn\\CompanyProgram\\GitProj\\bbqshop\\bbqshop\\Debug\\hookWriteFile.txt", "w");
	//if(fp != NULL)
	//{
	//	//sprintf(logBuff, "write file toWrite: %d, byte written: %d\r\n", nNumberOfBytesToWrite, *lpNumberOfBytesWritten);
	//	//fwrite(logBuff, strlen(logBuff), 1, fp);
	//	fread(tmpbuf, 100, 1, fp);
	//	fclose(fp);
	//	fp = NULL;
	//}
}
