#include "stdafx.h"
#include "common.h"
#include <tchar.h>
#include <strsafe.h>

// print error message to a console screen
// lpText : operation
// hResult : result of operation
// lpFile : source file that operation occur
// nLine : source file line that operation occur
void PrintError(LPCTSTR lpText, HRESULT hResult, LPCTSTR lpFile, UINT nLine)
{
	DWORD dwLastError = GetLastError();
	LPVOID lpResultMsgBuf = NULL;
	LPVOID lpLastErrMsgBuf = NULL;
	
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		0,
		(LPTSTR) &lpLastErrMsgBuf,
		0,
		NULL 
		);

	// if lasterror not equa to the hResult, print lasterror and hResult
	if (dwLastError != (DWORD)hResult)
	{
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			hResult,
			0,
			(LPTSTR) &lpResultMsgBuf,
			0,
			NULL 
			);
		
		PrintMsg(_T("%s failed, error no : %I64d %s, last error no : %I64d %s, file : %s, line : %d\n"),
			VALID_TEXT(lpText, _T("No operation")), (__int64)hResult, VALID_TCHAR(lpResultMsgBuf),
			(__int64)dwLastError, VALID_TCHAR(lpLastErrMsgBuf), VALID_TCHAR(lpFile), nLine);
	}
	else
	{
		PrintMsg(_T("%s failed, error no : %I64d %s, file : %s, line : %d\n"),
			VALID_TEXT(lpText ,_T("No operation")), (__int64)dwLastError, VALID_TCHAR(lpLastErrMsgBuf),
			VALID_TCHAR(lpFile), nLine);
	}
	
	if (!lpResultMsgBuf)
	{
		LocalFree(lpResultMsgBuf);
		lpResultMsgBuf = NULL;
	}
	if (!lpLastErrMsgBuf)
	{
		LocalFree(lpLastErrMsgBuf);
		lpLastErrMsgBuf = NULL;
	}
}

// print message with wide char, supports variable parameters
// lpFormat : message format, like the format in printf
// ... : variable parameters
void PrintMsgW(LPCWSTR lpFormat, ...)
{
	WCHAR szText[MAX_TEXT_LENGTH] = {0};
	HRESULT hr = 0;
	
	va_list args;
	va_start(args, lpFormat);
	
	hr = StringCchVPrintfW(szText, MAX_TEXT_LENGTH, lpFormat, args);
	
	if (SUCCEEDED(hr))
	{
		size_t cchLength = 0;
		StringCchLengthW(szText, MAX_TEXT_LENGTH, &cchLength);
		DWORD dwWriteReturned = 0;
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), szText, cchLength, &dwWriteReturned, NULL);
	}
	
	va_end(args);
}

// print message with char, supports variable parameters
// lpFormat : message format, like the format in printf
// ... : variable parameters
void PrintMsgA(LPCSTR lpFormat, ...)
{
	CHAR szText[MAX_TEXT_LENGTH] = {0};
	HRESULT hr = 0;
	
	va_list args;
	va_start(args, lpFormat);
	
	hr = StringCchVPrintfA(szText, MAX_TEXT_LENGTH, lpFormat, args);
	
	if (SUCCEEDED(hr))
	{
		size_t cchLength = 0;
		StringCchLengthA(szText, MAX_TEXT_LENGTH, &cchLength);
		DWORD dwWriteReturned = 0;
		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), szText, cchLength, &dwWriteReturned, NULL);
	}
	
	va_end(args);
}

// get size of the assembly code
// iptr0 : address of the assembly code
DWORD __stdcall GetOpCodeSize(unsigned char* iptr0) 
{ 
    unsigned char* iptr = iptr0;

    DWORD f = 0; 
    unsigned char mod=0; 
    unsigned char rm=0; 
    unsigned char b;

prefix: 
    b = *iptr++;    

    f |= table_1[b];

    if (f&C_FUCKINGTEST) 
        if (((*iptr)&0x38)==0x00)   // ttt 
            f=C_MODRM+C_DATAW0;       // TEST 
        else 
            f=C_MODRM;                // NOT,NEG,MUL,IMUL,DIV,IDIV

    if (f&C_TABLE_0F) 
    { 
        b = *iptr++; 
        f = table_0F[b]; 
    }

    if (f==C_ERROR) 
    { 
        //printf("error in %02X/n",b); 
        return C_ERROR; 
    }

    if (f&C_PREFIX) 
    { 
        f&=~C_PREFIX; 
        goto prefix; 
    }

    if (f&C_DATAW0) if (b&0x01) f|=C_DATA66; else f|=C_DATA1;

    if (f&C_MODRM) 
    { 
        b = *iptr++; 
        mod = b & 0xC0; 
        rm  = b & 0x07; 
        if (mod!=0xC0) 
        { 
            if (f&C_67)         // modrm16 
            { 
                if ((mod==0x00)&&(rm==0x06)) f|=C_MEM2; 
                if (mod==0x40) f|=C_MEM1; 
                if (mod==0x80) f|=C_MEM2; 
            } 
            else                // modrm32 
            { 
                if (mod==0x40) f|=C_MEM1; 
                if (mod==0x80) f|=C_MEM4; 
                if (rm==0x04) rm = (*iptr++) & 0x07;    // rm<-sib.base 
                if ((rm==0x05)&&(mod==0x00)) f|=C_MEM4; 
            } 
        } 
    } // C_MODRM

    if (f&C_MEM67)  if (f&C_67) f|=C_MEM2;  else f|=C_MEM4; 
    if (f&C_DATA66) if (f&C_66) f|=C_DATA2; else f|=C_DATA4;

    if (f&C_MEM1)  iptr++; 
    if (f&C_MEM2)  iptr+=2; 
    if (f&C_MEM4)  iptr+=4;

    if (f&C_DATA1) iptr++; 
    if (f&C_DATA2) iptr+=2; 
    if (f&C_DATA4) iptr+=4;

    return iptr - iptr0; 
}
