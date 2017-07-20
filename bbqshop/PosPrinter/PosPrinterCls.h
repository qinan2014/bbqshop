#pragma once
#include <vector>

#define NAMEPOS 0.02
#define CONTENTPOS 0.35
#define SPACE1 35

#define ONLYLINE L"-------------------------------------"

#define PrintHeader(mPrinter, shopInfo, pWnd, headName) \
	{ \
	mPrinter->SetDeviceWidth(pWnd->GetPrinterDeviceWidth());\
	mPrinter->AddNewLine(0.5, ONLYLINE, SPACE1, 50, L"宋体", 0); \
	mPrinter->AddNewLine(0.5, headName, SPACE1, pWnd->GetCommentFontSZ(), L"", 0);  \
	mPrinter->AddNewLine(0.5, ONLYLINE, SPACE1, 50, L"", 0);  \
	mPrinter->AddNewLine(NAMEPOS, L"", SPACE1, pWnd->GetCommentFontSZ());  \
	\
	mPrinter->AddNewLine(NAMEPOS, L"店铺名称：", SPACE1, pWnd->GetCommentFontSZ(), L"宋体"); \
	mPrinter->AppendLineString(CONTENTPOS, pWnd->StrToWStr(shopInfo.shopName)); \
	mPrinter->AddNewLine(NAMEPOS, L"店铺代码：", SPACE1); \
	mPrinter->AppendLineString(CONTENTPOS, pWnd->StrToWStr(shopInfo.shopCode)); \
	mPrinter->AddNewLine(NAMEPOS, L"收 银 台：", SPACE1); \
	mPrinter->AppendLineString(CONTENTPOS, pWnd->StrToWStr(shopInfo.cashdeskName)); \
	mPrinter->AddNewLine(NAMEPOS, L"收 银 员：", SPACE1); \
	mPrinter->AppendLineString(CONTENTPOS, pWnd->StrToWStr(shopInfo.userName)); \
	}  \

#define PrintEnd(mPrinter) \
	{ \
	mPrinter->AddNewLine(NAMEPOS, L""); \
	mPrinter->AddNewLine(NAMEPOS, L"备注：支付存根是财务对账的唯一凭证，请妥善保管 : ",SPACE1); \
	mPrinter->AddNewLine(NAMEPOS, ONLYLINE); \
	mPrinter->AddNewLine(NAMEPOS, L"智慧微生活 ",SPACE1); \
	mPrinter->AddNewLine(NAMEPOS, L"",30); \
	mPrinter->AddNewLine(0.5, ONLYLINE, 0, 0, L"", 0); \
	} \

#undef NAMEPOS
#undef CONTENTPOS
#undef SPACE1

class __declspec(dllexport) PosPrinter
{
public:
	struct LINESTRING 
	{
		unsigned int lineNum;  // 行号
		float x;  // 占这一行的百分比  小数
		int lineSpace;  // 下边距
		std::wstring lineContent;
		int fontsz;
		std::wstring fontName;
		int trimType;  // 0居中，1左侧
	};

	PosPrinter();
	virtual ~PosPrinter(void);

	//friend CFont* printLineString(PosPrinter *pp, CDC *inPrintDC, CFont *oriFont, PosPrinter::LINESTRING &inLine);//友元函数 
//private:
	std::wstring docName;
	int deviceWidth;
	int posY;  // 打印位置Y

	std::vector<LINESTRING > allLines;
	LINESTRING *lastLine;

	BOOL getPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);
	inline void getCurrentLineInfo(int &outLineNum, int &outLineSpace);
	inline void addLineByLineNum(int lineNum, float xPos, std::wstring inContent, int lineSpace ,int fontsz, std::wstring fontname, int trimType);
	//inline bool createNewFont(CDC *inPrintDC, CFont &outFont, int fontsz, std::wstring fontname, bool isOnlyLine = false);
	//inline CFont* printLineString(CDC *inPrintDC, CFont *oriFont, LINESTRING &inLine);
	inline float pixStrPercentInDC(std::wstring inContent, int textcx, int &outTextlen); // 文字所占总像素数
	inline int getNumInLine(std::wstring inDes, float pixch, float pixen, float allPix); 
protected:
	void init();
public:
	bool PreparePrinter(std::wstring inName);
	void AddNewLine(float xPos, std::wstring inContent, int lineSpace = 0, int fontsz = 0, std::wstring fontname = L"", int trimType = 1); // 如果fontsz为0，则用默认字体或者是之前设置的字体
	void AppendLineString(float xPos, std::wstring inContent, int fontsz = 0, std::wstring fontname = L"", int trimType = 1);
	int GetDeviceWidth();
	void SetDeviceWidth(int inWidth);
	void CallPrinter();
	void GetAllPrinters(std::vector<std::wstring > &outPrinters, const std::wstring &oriPrinter, int &outOriIndex);
};

