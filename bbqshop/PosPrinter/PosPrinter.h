// PosPrinter.h : main header file for the PosPrinter DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPosPrinterApp
// See PosPrinter.cpp for the implementation of this class
//

class CPosPrinterApp : public CWinApp
{
public:
	CPosPrinterApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
