#ifndef _RECALL_API_H_
#define _RECALL_API_H_

#include <tchar.h>
#include <windows.h>

typedef struct 
{
	DWORD64				dw64Flag;		// flag of hook api, must occupy one bit
	LPCTSTR				lpFunctionName;	// name of api
	LPCTSTR				lpDllName;		// name of dll that has the api
	LPVOID				lpRecallfn;		// recall function address
	LPVOID				lpApiAddr;		// api address
	PBYTE				pOrgfnMem;		// memory to save first few bytes of api and execute jmp code
	int					nOrgfnMemSize;	// size of pOrgfnMem
} RECALL_API_INFO, *PRECALL_API_INFO;

#define FLAG_HOOK_API(order)	(0x1 << (order))

typedef enum
{
	ORDER_MESSAGEBOXA = 0,				// HOOK MessageBoxA in user32.dll
	ORDER_MESSAGEBOXW,					// HOOK MessageBoxW in user32.dll
	ORDER_DEVICEIOCONTROL,				// HOOK DeviceIoControl in Kernel32.dll
	ORDER_CREATEFILEA,					// HOOK CreateFileA in Kernel32.dll
	ORDER_CREATEFILEW,					// HOOK CreateFileW in Kernel32.dll
	ORDER_READFILE,						// HOOK ReadFile in Kernel32.dll
	ORDER_READFILEEX,					// HOOK ReadFileEx in Kernel32.dll
	ORDER_WRITEFILE,					// HOOK WriteFile in Kernel32.dll
	ORDER_WRITEFILEEX,					// HOOK WriteFileEx in Kernel32.dll
	ORDER_CLOSEHANDLE,					// HOOK CloseHandle in Kernel32.dll
	ORDER_MAX
} ENUM_ORDER_HOOK_API;

// all infomation that we can hook
extern RECALL_API_INFO g_arHookAPIs[ORDER_MAX];

// HOOK MessageBoxA in user32.dll
typedef int (WINAPI *pfnMessageBoxA)(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType);
int WINAPI MyMessageBoxA(IN HWND hWnd, IN LPCSTR lpText, IN LPCSTR lpCaption, IN UINT uType);

// HOOK MessageBoxW in user32.dll
typedef int (WINAPI *pfnMessageBoxW)(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType);
int WINAPI MyMessageBoxW(IN HWND hWnd, IN LPCWSTR lpText, IN LPCWSTR lpCaption, IN UINT uType);

// HOOK DeviceIoControl in Kernel32.dll
typedef BOOL (WINAPI *pfnDeviceIoControl)(HANDLE hDevice,
										  DWORD dwIoControlCode,
										  LPVOID lpInBuffer,
										  DWORD nInBufferSize,
										  LPVOID lpOutBuffer,
										  DWORD nOutBufferSize,
										  LPDWORD lpBytesReturned,
										  LPOVERLAPPED lpOverlapped);
BOOL WINAPI MyDeviceIoControl(HANDLE hDevice,
							  DWORD dwIoControlCode,
							  LPVOID lpInBuffer,
							  DWORD nInBufferSize,
							  LPVOID lpOutBuffer,
							  DWORD nOutBufferSize,
							  LPDWORD lpBytesReturned,
							  LPOVERLAPPED lpOverlapped);

// HOOK CreateFileA in Kernel32.dll
typedef HANDLE(WINAPI *pfnCreateFileA)(LPCSTR lpFileName,
									   DWORD dwDesiredAccess,
									   DWORD dwShareMode,
									   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									   DWORD dwCreationDisposition,
									   DWORD dwFlagsAndAttributes,
									   HANDLE hTemplateFile);
HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName,
							DWORD dwDesiredAccess,
							DWORD dwShareMode,
							LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							DWORD dwCreationDisposition,
							DWORD dwFlagsAndAttributes,
							HANDLE hTemplateFile);

// HOOK CreateFileW in Kernel32.dll
typedef HANDLE(WINAPI *pfnCreateFileW)(LPCWSTR lpFileName,
									   DWORD dwDesiredAccess,
									   DWORD dwShareMode,
									   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									   DWORD dwCreationDisposition,
									   DWORD dwFlagsAndAttributes,
									   HANDLE hTemplateFile);
HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName,
							DWORD dwDesiredAccess,
							DWORD dwShareMode,
							LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							DWORD dwCreationDisposition,
							DWORD dwFlagsAndAttributes,
							HANDLE hTemplateFile);

// HOOK ReadFile in Kernel32.dll
typedef BOOL (WINAPI *pfnReadFile)(	HANDLE hFile,
									LPVOID lpBuffer,
									DWORD nNumberOfBytesToRead,
									LPDWORD lpNumberOfBytesRead,
									LPOVERLAPPED lpOverlapped);

BOOL WINAPI MyReadFile(	HANDLE hFile,
						LPVOID lpBuffer,
						DWORD nNumberOfBytesToRead,
						LPDWORD lpNumberOfBytesRead,
						LPOVERLAPPED lpOverlapped);

// HOOK ReadFileEx in Kernel32.dll
typedef BOOL (WINAPI *pfnReadFileEx)(HANDLE hFile,
									 LPVOID lpBuffer,
									 DWORD nNumberOfBytesToRead,
									 LPOVERLAPPED lpOverlapped,
									 LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

BOOL WINAPI MyReadFileEx(HANDLE hFile,
						 LPVOID lpBuffer,
						 DWORD nNumberOfBytesToRead,
						 LPOVERLAPPED lpOverlapped,
						 LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

// HOOK WriteFile in Kernel32.dll
typedef BOOL (WINAPI *pfnWriteFile)(HANDLE hFile,
									LPCVOID lpBuffer,
									DWORD nNumberOfBytesToWrite,
									LPDWORD lpNumberOfBytesWritten,
									LPOVERLAPPED lpOverlapped);
BOOL WINAPI MyWriteFile(HANDLE hFile,
						LPCVOID lpBuffer,
						DWORD nNumberOfBytesToWrite,
						LPDWORD lpNumberOfBytesWritten,
						LPOVERLAPPED lpOverlapped);

// HOOK WriteFileEx in Kernel32.dll
typedef BOOL (WINAPI *pfnWriteFileEx)(HANDLE hFile,
									  LPCVOID lpBuffer,
									  DWORD nNumberOfBytesToWrite,
									  LPOVERLAPPED lpOverlapped,
									  LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
BOOL WINAPI MyWriteFileEx(HANDLE hFile,
						  LPCVOID lpBuffer,
						  DWORD nNumberOfBytesToWrite,
						  LPOVERLAPPED lpOverlapped,
                          LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

// HOOK CloseHandle in Kernel32.dll
typedef BOOL (WINAPI *pfnCloseHandle)(HANDLE hFile);
BOOL WINAPI MyCloseHandle(HANDLE hFile);

#endif // _RECALL_API_H_