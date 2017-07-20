#include "zhfunclib.h"
#include <QCoreApplication>

#pragma comment(lib ,"user32.lib")
#include <Tlhelp32.h>
#include <time.h>
#pragma comment (lib, "ws2_32.lib")
#include <QString>
#include <QFile>
#include "AllExeName.h"
#include <QSettings>

ZHFuncLib::ZHFuncLib()
{

}

ZHFuncLib::~ZHFuncLib()
{

}

std::string ZHFuncLib::GetWorkPath()
{
	static QString _workPath;
	if (_workPath.isEmpty())
	{
		_workPath = QCoreApplication::applicationDirPath();
		QFile tmpFile(_workPath + "/" + BBQSHOPEXE);
		if (!tmpFile.exists())
		{
			QSettings *reg = new QSettings(REGEDITRUN,QSettings::NativeFormat);
			QString startpath = reg->value(APPLICATIONNAME).toString();
			startpath = startpath.replace("\\", "/");
			int exepos = startpath.lastIndexOf('/');
			if (exepos > 0)
			{
				_workPath = startpath.left(exepos);
			}
		}
	}
	return _workPath.toStdString();
}

void ZHFuncLib::GetTargetProcessIds(std::string inTarget, std::vector<int > &outIds)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		return;
	BOOL bProcess = Process32First(hProcessSnap, &pe32);
	int targetNameLen = inTarget.length();
	while(bProcess)
	{
		int searchLen = wcslen(pe32.szExeFile);
		if (targetNameLen == searchLen)
		{
			bool isEqualName = true;
			for (int i = 0; i < targetNameLen; ++i)
			{
				if (inTarget[i] != pe32.szExeFile[i])
				{
					isEqualName = false;
					break;
				}
			}
			if (isEqualName)
				outIds.push_back(pe32.th32ProcessID);
		}
		
		// 继续查找
		bProcess = Process32Next(hProcessSnap,&pe32);
	}
	CloseHandle(hProcessSnap);
}

bool ZHFuncLib::TerminateProcessExceptCurrentOne(std::string inTarget)
{
	std::vector<int > ids;
	GetTargetProcessIds(inTarget, ids);
	DWORD processId = GetCurrentProcessId();//当前进程id

	// 除了当前进程外 其余都结束
	int sz = ids.size();
	bool hasAnotherProcess = false;
	for (int i = 0; i < sz; ++i)
	{
		if (ids[i] == processId)
			continue;
		hasAnotherProcess = true;
		HANDLE Hwnd = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE,0,ids[i]);
		TerminateProcess(Hwnd, 0);
	}
	return hasAnotherProcess;
}

void ZHFuncLib::NativeLog(const char *extname, const char *content, char *mode)
{
	time_t t = time(NULL);
	struct tm *local = localtime(&t);
	char logbuftxt[200];
	sprintf(logbuftxt, "%s/logdata%s-%d-%d-%d-%d.txt", ZHFuncLib::GetWorkPath().c_str(), extname, local->tm_year, local->tm_mon + 1, local->tm_mday, local->tm_hour);

	FILE * fp = NULL;
	if((fp = fopen(logbuftxt, mode)) != NULL)
	{
		char buf[30];
		sprintf(buf, "time %d:%d:%d------- \r\n", local->tm_hour, local->tm_min, local->tm_sec);
		fwrite(buf, strlen(buf), 1, fp);
		fwrite(content, strlen(content), 1, fp);
		fwrite("\r\n\r\n", strlen("\r\n\r\n"), 1, fp);
		fclose(fp);
	}
}

bool ZHFuncLib::SendProcessMessage(HWND selfWnd, HWND targetWnd, ULONG_PTR dataType, std::string willSendData)
{
	if (::IsWindow(targetWnd))
	{
		char *cstr = new char[willSendData.length() + 1];
		strcpy(cstr, willSendData.c_str());
		COPYDATASTRUCT copydata;
		copydata.dwData = dataType;  // 用户定义数据
		copydata.lpData = cstr;  //数据大小
		copydata.cbData = willSendData.length();  // 指向数据的指针
		::SendMessage(targetWnd, WM_COPYDATA, reinterpret_cast<WPARAM>(selfWnd), reinterpret_cast<LPARAM>(&copydata));
		delete [] cstr;
		return true;
	}
	return false;
}

std::wstring ZHFuncLib::StringToWstring(const std::string str)
{// string转wstring
	unsigned len = str.size() * 2;// 预留字节数
	setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs(p,str.c_str(),len);// 转换
	std::wstring str1(p);
	delete[] p;// 释放申请的内存
	return str1;
}

std::string ZHFuncLib::WstringToString(const std::wstring str)
{// wstring转string
	unsigned len = str.size() * 4;
	setlocale(LC_CTYPE, "");
	char *p = new char[len];
	wcstombs(p,str.c_str(),len);
	std::string str1(p);
	delete[] p;
	return str1;
}
