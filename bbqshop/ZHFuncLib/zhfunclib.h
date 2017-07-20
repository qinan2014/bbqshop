#ifndef ZHFUNCLIB_H
#define ZHFUNCLIB_H

#include "zhfunclib_global.h"
//#include <QString>
#include <vector>
#include <Windows.h>

#define APPLICATIONNAME "bbqpay"
#define REGEDITRUN "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"

class __declspec(dllexport) ZHFuncLib
{
public:
	ZHFuncLib();
	~ZHFuncLib();
	
	static std::string GetWorkPath();
	static void GetTargetProcessIds(std::string inTarget, std::vector<int > &outIds);
	static bool TerminateProcessExceptCurrentOne(std::string inTarget); // 返回值表示是否已经有其他进程打开
	static void NativeLog(const char *extname, const char *content, char *mode);
	static bool SendProcessMessage(HWND selfWnd, HWND targetWnd, ULONG_PTR dataType, std::string willSendData); // 返回值是否发送消息成功
	static std::wstring StringToWstring(const std::string str);
	static std::string WstringToString(const std::wstring str);
};

#endif // ZHFUNCLIB_H
