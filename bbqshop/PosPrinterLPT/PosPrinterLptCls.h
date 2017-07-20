#pragma once

#include <string>

#define ONLYLINELPT "---------------------------------\r\n"

#define PrintHeaderLPT(mPrinter, shopInfo, headName) \
	{ \
	mPrinter.PrintString(ONLYLINELPT, PosPrinterLptCls::CENTERALIGN); \
	mPrinter.PrintString(headName, PosPrinterLptCls::TITLE);  \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::CENTERALIGN);  \
	mPrinter.PrintString(ONLYLINELPT, PosPrinterLptCls::CENTERALIGN);  \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::CENTERALIGN);  \
	\
	mPrinter.PrintString("店铺名称：", PosPrinterLptCls::NORMAL); \
	QString tmpstr = shopInfo.shopName; \
	mPrinter.PrintString(tmpstr.toLocal8Bit().toStdString(), PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("店铺代码：", PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString(shopInfo.shopCode, PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("收 银 台：", PosPrinterLptCls::NORMAL); \
	tmpstr = shopInfo.cashdeskName; \
	mPrinter.PrintString(tmpstr.toLocal8Bit().toStdString(), PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("收 银 员：", PosPrinterLptCls::NORMAL); \
	tmpstr = shopInfo.userName; \
	mPrinter.PrintString(tmpstr.toLocal8Bit().toStdString(), PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL); \
	}  \

#define PrintEndLPT(mPrinter) \
	{ \
	mPrinter.PrintString("\r\n备注：支付存根是财务对账的唯一凭证，请妥善保管 : \r\n", PosPrinterLptCls::NORMAL); \
	mPrinter.PrintString(ONLYLINELPT, PosPrinterLptCls::CENTERALIGN); \
	mPrinter.PrintString("智慧微生活\r\n",PosPrinterLptCls::CENTERALIGN); \
	mPrinter.PrintString(ONLYLINELPT, PosPrinterLptCls::CENTERALIGN); \
	} \

class __declspec(dllexport) PosPrinterLptCls
{
public:
	enum TXTTYPE
	{
		UNKNOWN,
		TITLE,
		CENTERALIGN,
		RIGHTALIGN,
		NORMAL,
	};
	PosPrinterLptCls(void);
	virtual ~PosPrinterLptCls(void);

	int Prepare(int lptNum); // If there is no error, this function will return 0, else return error num.
	int Prepare(const char *lptName);
	void PrintString(std::string inContent, TXTTYPE inType);
	static bool CanOpenLPT(int lptNum);

private:
	HANDLE Hcom;
	TXTTYPE curType;
};

