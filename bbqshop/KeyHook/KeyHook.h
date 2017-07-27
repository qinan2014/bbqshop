
#include <Windows.h>

#pragma comment(lib ,"user32.lib")

class __declspec(dllexport) CKeyHook
{
public:
	CKeyHook();
	~CKeyHook();
	BOOL StartHook(HWND hWnd);
	BOOL StopHook();
	void EnableInterception(int keyIndex, bool isNeedInterception);
	bool IsInterception(int keyIndex);
	void SetPayKey(int inwxKey, int inalipayKey);
};
