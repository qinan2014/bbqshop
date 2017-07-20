
#include "stdafx.h"
#include "PosPrinterCls.h"
#include <windows.h>
#include <WinSpool.h>
#include "PosPrinter.h"
#include "zhfunclib.h"

//#include "bbqpay.h"
//#include "bbqpayDlg.h"

#ifndef FALSE
#define FALSE 0
#endif

extern CPosPrinterApp theApp;
CFont mnewFont; 
CDC printDC;

PosPrinter::PosPrinter()
{
	//mpWnd = pParent;

	init();
}

PosPrinter::~PosPrinter(void)
{
	DeleteDC(printDC.Detach());
	mnewFont.DeleteObject();  
}

void PosPrinter::init()
{
	docName = L"BoBoqi打印";
	posY = 0;
	lastLine = NULL;
}

void PosPrinter::GetAllPrinters(std::vector<std::wstring > &outPrinters, const std::wstring &oriPrinter, int &outOriIndex)
{
	DWORD dwNeeded;  
	DWORD dwReturn;  
	DWORD dwFlag = PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL;  
	EnumPrinters(dwFlag, NULL, 4, NULL, 0, &dwNeeded, &dwReturn);  
	PRINTER_INFO_4* p4;  
	p4 = new PRINTER_INFO_4[dwNeeded];  
	EnumPrinters(dwFlag, NULL, 4, (PBYTE)p4, dwNeeded, &dwNeeded, &dwReturn);  
	std::wstring tmpName;
	//outOriIndex = 0;
	outPrinters.push_back(L"不配置打印机");
	for (int i = 0; i<(int)dwReturn; i++){
		if (p4[i].pPrinterName == NULL)
			continue;

		char tmpbuf[500];
		sprintf(tmpbuf, "printname500 len %d", wcslen(p4[i].pPrinterName));
		ZHFuncLib::NativeLog("", tmpbuf, "a");

		sprintf(tmpbuf, "printname500 %s", ZHFuncLib::WstringToString(p4[i].pPrinterName).c_str());
		ZHFuncLib::NativeLog("", tmpbuf, "a");

		tmpName = p4[i].pPrinterName;
		outPrinters.push_back(p4[i].pPrinterName);
		if (tmpName.compare(oriPrinter) == 0)
			outOriIndex = i + 1;
	}

	delete []p4;  
}

bool PosPrinter::PreparePrinter(std::wstring inName)
{
	CString tmpPrinterName(inName.c_str()); 
	if (tmpPrinterName.IsEmpty())
	{
		//AfxMessageBox(_T("请配置打印机"));
		return false;
	}
	DWORD dwFlag = PD_ALLPAGES | PD_NOPAGENUMS | PD_USEDEVMODECOPIES | PD_HIDEPRINTTOFILE;  //打印配置界面的按钮可用性，因为后台打印，其实这个配置没什么意义  

	CPrintDialog pPrintdlg(FALSE, dwFlag, NULL); //CPrintDialog实例化，因为MFC的打印设备无关性，可以理解为这就是一台打印机  

	HGLOBAL hDevMode = NULL;  
	HGLOBAL hDevNames = NULL;  
	if (getPrinterDevice(tmpPrinterName.GetBuffer(0), &hDevNames, &hDevMode)) //获得指定打印机的配置、名字
	{
		theApp.SelectPrinter(hDevNames, hDevMode);
	}
	else{
		AfxMessageBox(_T("请选择正确的打印机"));
		return false;
	}  

	tmpPrinterName.ReleaseBuffer();  

	pPrintdlg.m_pd.hDevMode = hDevMode;     //让pPrintdlg使用我们指定的打印机  
	pPrintdlg.m_pd.hDevNames = hDevNames;  

	// 删除前面的DC
	DeleteDC(printDC.Detach());  
	printDC.Attach(pPrintdlg.CreatePrinterDC());    //后台打印创建法，如果需要弹出打印对话框，请用DoModal  

	// 获得设备的像素宽度
	deviceWidth = printDC.GetDeviceCaps(HORZRES);
	return true;
}

bool createNewFont(CDC *inPrintDC, CFont &outFont, int fontsz, std::wstring fontname, bool isOnlyLine)
{
	if (fontsz == 0)  // 不需要创建新的字体
		return false;

	// 需要创建新的字体
	CFont *curFont = inPrintDC->GetCurrentFont();
	LOGFONT lf;   
	curFont->GetLogFont(&lf);
	//if (lf.lfHeight == fontsz || lf.lfFaceName == fontname) // 跟原有字体是一样的
	//	return false;
	if (lf.lfHeight == fontsz || fontname.compare(lf.lfFaceName) == 0) // 跟原有字体是一样的
		return false;

	outFont.DeleteObject();
	lf.lfHeight = fontsz;
	if (!isOnlyLine)
		lf.lfWidth = fontsz * 0.5 - 5;
	else
		lf.lfWeight = fontsz * 2000;
	outFont.CreateFontIndirect(&lf);

	return true;
}

CFont* printLineString(PosPrinter *pp, CDC *inPrintDC, CFont *oriFont, PosPrinter::LINESTRING &inLine)
{
	CSize mtextSize;
#define LEFTSIDEWIDTH 0.02
	if (pp->lastLine != NULL && pp->lastLine->lineNum != inLine.lineNum)
		pp->posY += mtextSize.cy + pp->lastLine->lineSpace;

	//bool isonlyLine = (inLine.lineContent == ONLYLINE);
	bool isonlyLine = (inLine.lineContent.compare(ONLYLINE) == 0);

	bool suc = createNewFont(inPrintDC, mnewFont, inLine.fontsz, inLine.fontName, isonlyLine);
	CFont* oldFont = NULL;
	if (suc)
	{
		oldFont = inPrintDC->SelectObject(&mnewFont);
		mtextSize = inPrintDC->GetTextExtent(_T("00"), 2);
		if (mtextSize == CSize())
			oriFont = oldFont;
	}
	else if (mtextSize == CSize())
		mtextSize = inPrintDC->GetTextExtent(_T("00"), 2);

	// 居中时计算新位置
	// 文字所占像素总比例
	int txtLen = 0;
	float perszx = pp->pixStrPercentInDC(inLine.lineContent, mtextSize.cx, txtLen);
	if (inLine.trimType == 0)
		inLine.x -= perszx * 0.5;
	// 每个文字占设备比例 
	float pixtxtPerCH = perszx / txtLen * 1.55; // 中文所占像素
	float pixtxtPerEn = perszx / txtLen * 0.9;  // 英文所占像素

	// 文字过长  总行数
	float beginPrintX = inLine.x;  // 开始打印位置
	float pixPercentx = 1.0f - beginPrintX - LEFTSIDEWIDTH;  // 这一行所占像素总比例
	int lineNum = (int)(inLine.x + perszx) + 1;

	if (isonlyLine)
		lineNum = 1;

	for (int i = 0; i < lineNum; ++i)
	{
		float thislinenum = pp->getNumInLine(inLine.lineContent, pixtxtPerCH, pixtxtPerEn, pixPercentx) - 1;
		int iLineNum = (int )thislinenum - (int )thislinenum % 2 - 2;
		if (thislinenum + 1 == inLine.lineContent.length())
			iLineNum = thislinenum + 1;
		CString tmpLineContent = inLine.lineContent.c_str();
		CString leftStr = tmpLineContent.Left(iLineNum);

		inPrintDC->TextOut(beginPrintX * pp->deviceWidth, pp->posY, leftStr, leftStr.GetLength());

		inLine.lineContent = tmpLineContent.Right(inLine.lineContent.length() - leftStr.GetLength());
		if (inLine.lineContent.length() > 0 && !isonlyLine)
		{
			beginPrintX = LEFTSIDEWIDTH;
			pixPercentx = 1 - LEFTSIDEWIDTH * 2;
			pp->lastLine = &inLine;
			pp->posY += mtextSize.cy + pp->lastLine->lineSpace;
		}
	}

	//inPrintDC->TextOut(inLine.x * deviceWidth, posY, inLine.lineContent, strLen);

	pp->lastLine = &inLine;

	return oldFont;

#undef LEFTSIDEWIDTH
}


void PosPrinter::CallPrinter()
{
	if (printDC.m_hDC == NULL)
		return;
	DOCINFO di;     //下面的内容网上很多，就不解释了  
	di.cbSize = sizeof(DOCINFO);  
	di.lpszDocName = docName.c_str();  
	di.lpszDatatype = NULL;  
	di.lpszOutput = NULL;  
	di.fwType = 0;  

	printDC.StartDocW(&di);  
	printDC.StartPage();  
	printDC.SetMapMode(MM_TEXT);  

	CRect recPrint(0, 0, printDC.GetDeviceCaps(LOGPIXELSX), printDC.GetDeviceCaps(LOGPIXELSY));  
	printDC.DPtoLP(&recPrint);  
	printDC.SetWindowOrg(0, 0);  

	printDC.SetTextAlign(TA_TOP | TA_LEFT);  

	CFont* oldFont = NULL;  
	int linesz = allLines.size();
	for (int i = 0; i < linesz; ++i)
	{
		printLineString(this, &printDC, oldFont, allLines[i]);
	}

	if (oldFont != NULL)
		printDC.SelectObject(oldFont);  
	mnewFont.DeleteObject();  
	printDC.EndPage();  
	printDC.EndDoc();  
	DeleteDC(printDC.Detach());  
}

BOOL PosPrinter::getPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode)  
{  
	if (phDevMode == NULL || phDevNames == NULL)  
		return FALSE;  

	// Open printer  
	HANDLE hPrinter;  
	if (OpenPrinter(pszPrinterName, &hPrinter, NULL) == FALSE)  
		return FALSE;  

	// obtain PRINTER_INFO_2 structure and close printer  
	DWORD dwBytesReturned, dwBytesNeeded;  
	GetPrinter(hPrinter, 2, NULL, 0, &dwBytesNeeded);  
	PRINTER_INFO_2* p2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwBytesNeeded);  
	if (GetPrinter(hPrinter, 2, (LPBYTE)p2, dwBytesNeeded, &dwBytesReturned) == 0) {  
		GlobalFree(p2);  
		ClosePrinter(hPrinter);  
		return FALSE;  
	}  
	ClosePrinter(hPrinter);  

	// Allocate a global handle for DEVMODE  
	HGLOBAL  hDevMode = GlobalAlloc(GHND, sizeof(*p2->pDevMode) +  
		p2->pDevMode->dmDriverExtra);  
	ASSERT(hDevMode);  
	DEVMODE* pDevMode = (DEVMODE*)GlobalLock(hDevMode);  
	ASSERT(pDevMode);  

	// copy DEVMODE data from PRINTER_INFO_2::pDevMode  
	memcpy(pDevMode, p2->pDevMode, sizeof(*p2->pDevMode) +  
		p2->pDevMode->dmDriverExtra);  
	GlobalUnlock(hDevMode);  

	// Compute size of DEVNAMES structure from PRINTER_INFO_2's data  
	DWORD drvNameLen = lstrlen(p2->pDriverName)+1;  // driver name  
	DWORD ptrNameLen = lstrlen(p2->pPrinterName)+1; // printer name  
	DWORD porNameLen = lstrlen(p2->pPortName)+1;    // port name  

	// Allocate a global handle big enough to hold DEVNAMES.  
	HGLOBAL hDevNames = GlobalAlloc(GHND,  
		sizeof(DEVNAMES) +  
		(drvNameLen + ptrNameLen + porNameLen)*sizeof(TCHAR));  
	ASSERT(hDevNames);  
	DEVNAMES* pDevNames = (DEVNAMES*)GlobalLock(hDevNames);  
	ASSERT(pDevNames);  

	// Copy the DEVNAMES information from PRINTER_INFO_2  
	// tcOffset = TCHAR Offset into structure  
	int tcOffset = sizeof(DEVNAMES)/sizeof(TCHAR);  
	ASSERT(sizeof(DEVNAMES) == tcOffset*sizeof(TCHAR));  

	pDevNames->wDriverOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pDriverName,  
		drvNameLen*sizeof(TCHAR));  
	tcOffset += drvNameLen;  

	pDevNames->wDeviceOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPrinterName,  
		ptrNameLen*sizeof(TCHAR));  
	tcOffset += ptrNameLen;  

	pDevNames->wOutputOffset = tcOffset;  
	memcpy((LPTSTR)pDevNames + tcOffset, p2->pPortName,  
		porNameLen*sizeof(TCHAR));  
	pDevNames->wDefault = 0;  

	GlobalUnlock(hDevNames);  
	GlobalFree(p2);   // free PRINTER_INFO_2  

	// set the new hDevMode and hDevNames  
	*phDevMode = hDevMode;  
	*phDevNames = hDevNames;  
	return TRUE;  
}  

void PosPrinter::AddNewLine(float xPos, std::wstring inContent, int lineSpace, int fontsz, std::wstring fontname, int trimType)
{
	int prelineNum, prelineSpace;
	getCurrentLineInfo(prelineNum, prelineSpace);
	addLineByLineNum(prelineNum + 1, xPos, inContent, lineSpace, fontsz, fontname, trimType);
}

void PosPrinter::AppendLineString(float xPos, std::wstring inContent, int fontsz /*= 0*/, std::wstring fontname /*= L""*/, int trimType)
{
	int prelineNum, prelineSpace;
	getCurrentLineInfo(prelineNum, prelineSpace);
	addLineByLineNum(prelineNum, xPos, inContent, prelineSpace, fontsz, inContent, trimType);
}
inline void PosPrinter::addLineByLineNum(int lineNum, float xPos, std::wstring inContent, int lineSpace, int fontsz, std::wstring fontname, int trimType)
{
	ASSERT(xPos < 1);
	LINESTRING lineStr;
	lineStr.lineNum = lineNum;
	lineStr.x = xPos;
	lineStr.lineContent = inContent;
	lineStr.lineSpace = lineSpace;
	lineStr.fontsz = fontsz;
	lineStr.fontName = fontname;
	lineStr.trimType = trimType;
	allLines.push_back(lineStr);

}

inline void PosPrinter::getCurrentLineInfo(int &outLineNum, int &outLineSpace)
{
	// 计算lineNum
	int linesz = allLines.size();
	outLineNum = 0;
	if (linesz != 0)
	{
		const LINESTRING &linestr = allLines[linesz -1];
		outLineNum = linestr.lineNum;
		outLineSpace = linestr.lineSpace;
	}
}

inline int PosPrinter::getNumInLine(std::wstring inDes, float pixch, float pixen, float allPix)
{
	int len = inDes.length();
	float beginPix = 0.0f;
	int index = 0;
	for (int i = 0; i < len; ++i)
	{
		index = i;
		if (beginPix >= allPix)
			break;
		char tmpch = inDes.at(i);
		if (beginPix > allPix)
			break;
		if (tmpch > 32 && tmpch < 127) // 英文
			beginPix += pixen;
		else
			beginPix += pixch * 1.35;
	}
	if (beginPix < allPix)
		return len;
	return index;
}

// 文字所占总像素数比例
inline float PosPrinter::pixStrPercentInDC(std::wstring inContent, int textcx, int &outTextlen)
{
	USES_CONVERSION; 
	std::string tmp = W2A(CString(inContent.c_str()));
	outTextlen = tmp.length();
	int szx = tmp.length() * textcx;
	// 占设备总像素数
	float perszx = (float )szx / (float )deviceWidth * 0.5;

	return perszx;
}

int PosPrinter::GetDeviceWidth()
{
	return deviceWidth;
}

void PosPrinter::SetDeviceWidth(int inWidth)
{
	if (inWidth < 0)
		return;
	deviceWidth = inWidth;
}