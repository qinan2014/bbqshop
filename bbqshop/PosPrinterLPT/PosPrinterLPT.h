// PosPrinterLPT.h : main header file for the PosPrinterLPT DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPosPrinterLPTApp
// See PosPrinterLPT.cpp for the implementation of this class
//

class CPosPrinterLPTApp : public CWinApp
{
public:
	CPosPrinterLPTApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
