#include "stdafx.h"
#include "PosPrinterLptCls.h"

PosPrinterLptCls::PosPrinterLptCls(void)
{
	curType = UNKNOWN;
	Hcom = NULL;
}


PosPrinterLptCls::~PosPrinterLptCls(void)
{
	if (Hcom != NULL && Hcom != INVALID_HANDLE_VALUE)
		CloseHandle(Hcom);
}

int PosPrinterLptCls::Prepare(int lptNum)
{
	char tmpbuf[50];
	sprintf(tmpbuf, "\\\\.\\LPT%d", lptNum);
	Hcom = CreateFile(tmpbuf, 
		GENERIC_READ|GENERIC_WRITE, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL 
		); 
	if(Hcom == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		return error;
	}
	return 0;
}

bool PosPrinterLptCls::CanOpenLPT(int lptNum)
{
	char tmpbuf[50];
	sprintf(tmpbuf, "\\\\.\\LPT%d", lptNum);
	HANDLE tmpdev = CreateFile(tmpbuf, 
		GENERIC_READ|GENERIC_WRITE, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL 
		); 
	if(tmpdev == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		return false;
	}
	CloseHandle(tmpdev);
	return true;
}

int PosPrinterLptCls::Prepare(const char *lptName)
{
	char tmpbuf[50];
	sprintf(tmpbuf, "\\\\.\\%s", lptName);
	Hcom = CreateFile(tmpbuf, 
		GENERIC_READ|GENERIC_WRITE, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL 
		); 
	if(Hcom == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		return error;
	}
	CHAR poscmds[3];
	DWORD dwTemp = 27;
	poscmds[0] = 82;
	poscmds[1] = 15;
	WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);

	return 0;
}

void PosPrinterLptCls::PrintString(std::string inContent, TXTTYPE inType)
{
	if(Hcom == INVALID_HANDLE_VALUE)
		return;
	CHAR poscmds[3];
	DWORD dwTemp = 0;
	switch (inType)
	{
	case NORMAL:
		if (curType != NORMAL)
		{
			// bold off
			poscmds[0] = 0x1B;
			poscmds[1] = 'G';
			poscmds[2] = 0x00;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
			// left 
			poscmds[0] = 0x1B;
			poscmds[1] = 'a';
			poscmds[2] = 0x00;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
		}
		break;
	case CENTERALIGN:
		if (curType != CENTERALIGN)
		{
			// bold on
			poscmds[0] = 0x1B;
			poscmds[1] = 'G';
			poscmds[2] = 0x01;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
			// center 
			poscmds[0] = 0x1B;
			poscmds[1] = 'a';
			poscmds[2] = 0x01;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
		}
		break;
		break;
	case TITLE:
		if (curType != TITLE)
		{
			// bold on
			poscmds[0] = 0x1B;
			poscmds[1] = 'G';
			poscmds[2] = 0x01;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
			// center 
			poscmds[0] = 0x1B;
			poscmds[1] = 'a';
			poscmds[2] = 0x01;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
		}
		break;
	case RIGHTALIGN:
		if (curType != RIGHTALIGN)
		{
			// bold off
			poscmds[0] = 0x1B;
			poscmds[1] = 'G';
			poscmds[2] = 0x00;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
			// right 
			poscmds[0] = 0x1B;
			poscmds[1] = 'a';
			poscmds[2] = 0x02;
			WriteFile(Hcom, poscmds, 3, &dwTemp, NULL);
		}
		break;
	}

	int nLength = inContent.length();
	WriteFile(Hcom,inContent.c_str(),nLength,&dwTemp,NULL);

	curType = inType;
}