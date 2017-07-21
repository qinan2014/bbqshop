#include <afxwin.h>
#include "maindlg.h"
#include "AllWindowTitle.h"
#include "TipExtendBtn.h"
#include "ZHSettingRW.h"
#include "ProcessProtocal.h"
#include "zhfunclib.h"
#include "ZhuiHuiMsg.h"
#include "PosPrinterCls.h"
#include "PosPrinterLptCls.h"

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
	if ((dwStyle & WS_OVERLAPPEDWINDOW) && (dwStyle & WS_VISIBLE))
	{
		// add window to list
		QString className, windowTitle;
		WCHAR tmpName[256];
		CWnd* pWnd = CWnd::FromHandle(hwnd);
		// 窗口类名
		::GetClassName(hwnd, tmpName, 256);
		className = QString::fromWCharArray(tmpName);
		// 窗口标题
		::GetWindowText(pWnd->GetSafeHwnd(), tmpName, 256);
		windowTitle = QString::fromWCharArray(tmpName);

		if (!windowTitle.isEmpty())
		{
			MainDlg *parWidget = (MainDlg *)lParam;
			parWidget->mWinClassNames.push_back(className);
			parWidget->mWinWindowNames.push_back(windowTitle);
		}
	}

	return TRUE;
}


MainDlg::MainDlg(QApplication *pApp, char *account, QWidget *parent)
	: QDialog(parent), parWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	ui.setupUi(this);
	setWindowTitle(MAINDLGTITLE);

	setStyleSheet("QDialog{background-color:#F9F7F7;border: 1px solid #112D4E;border-width:1px;border-radius:2px}"); 
	setTopBtn();

	urlServer = new BbqUrlServer(this);
	// 保存Account
	codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	memcpy(deskInfo.account, account, strlen(account));
	deskInfo.account[strlen(account)] = 0;
	ZHSettingRW settingRW(mZHSetting);
	settingRW.ReadZHSetting();
	// 初始化数据
	initFrame();
}

MainDlg::~MainDlg()
{

}

void MainDlg::SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag)
{
	urlServer->SendToURLRecord(logLevel, logModule, logMessage, urlTag);
}

void MainDlg::GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag)
{
	urlServer->GetDataFromServer(inSecondAddr, inApi, inData, urlTag);
}

void MainDlg::GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag)
{
	urlServer->GetDataFromServer1(inUrl, inSecondAddr, inApi, ioRootVal, urlTag);
}

void MainDlg::GetMAC(char *mac)
{
	urlServer->GetMAC(mac);
}

void MainDlg::TimeFormatRecover(std::string &outStr, std::string inOriTimeStr)
{
	urlServer->TimeFormatRecover(outStr, inOriTimeStr);
}

std::string MainDlg::GetPayTool(int inType)
{
	return urlServer->GetPayTool(inType);
}

bool MainDlg::IsImportentOperateNow()
{
	return urlServer->IsImportentOperateNow();
}

inline void MainDlg::setTopBtn()
{
	QRect parRect = this->geometry();
	int xpos = parRect.width() - 30;
	int ypos = 5;

	TipExtendBtn *closeBtn = new TipExtendBtn(this, "/res/close.png", "/res/closered.png");
	closeBtn->setGeometry(QRect(xpos, ypos, 25, 25));
	closeBtn->setToolTip(QString::fromLocal8Bit("关闭"));

	// 关闭
	connect(closeBtn, SIGNAL(pressed()), this, SLOT(closeMainDlg()));
}

void MainDlg::closeMainDlg()
{
	hide();
	mainApp->quit();
}

void MainDlg::hideEvent(QHideEvent * event)
{
	Json::Value mValData;
	mValData[PRO_HEAD] = TO_FLOATWIN_CLOSEMAINDLG;
	mValData[PRO_DLG_STATUS] = 0;
	HWND hwnd = ::FindWindowW(NULL, FLOATWINTITLEW);
	ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
}

bool MainDlg::DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi)
{
	return true;
}

inline void MainDlg::initFrame()
{
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
	ui.shopNameTxt->setText((shopInfo.shopName));
	ui.cashNoTxt->setText(shopInfo.cashdeskName);
	ui.shopNumTxt->setText(QString::number(shopInfo.shopid));

	char cashdes[60];
	sprintf(cashdes, "(%s) %s", shopInfo.cashdeskId, shopInfo.cashdeskName);
	ui.cboCashNo->addItem(cashdes);
	ui.cboCashNo->setCurrentIndex(0);
	// 收银软件信息
	codeSetIO::CarishDesk &carishInfo = mZHSetting.carishInfo;
	int mRelativeType = carishInfo.selectRange.relitiveType;
	ui.cboMoneyPos->setCurrentIndex(mRelativeType);

	QString windowName = QString::fromStdWString(ZHFuncLib::StringToWstring(carishInfo.windowName));
	QString exeName = carishInfo.exeName;
	// 是否二值化
	ui.chbBinarization->setChecked(mZHSetting.carishInfo.selectRange.imageBinaryzation != 0);
	// 截图放大倍数
	float imageScale;
	ui.cboImageScale->setCurrentIndex(getImageScaleTag(imageScale));
	// 获取所有的窗口
	mWinClassNames.clear();
	mWinWindowNames.clear();
	mWinClassNames.push_back(QString::fromLocal8Bit("不开启自动识别"));
	mWinWindowNames.push_back(QString::fromLocal8Bit("不开启自动识别"));
	EnumWindows(EnumWindowsProc, (LPARAM)this);//遍历窗口程序
	int sz = mWinWindowNames.size();
	int cursel = 0;
	for (int i = 0; i < sz; ++i)
	{
		ui.cboCashTool->addItem(mWinWindowNames[i]);
		if (mWinWindowNames[i] == windowName)
			cursel = i;
	}
	ui.cboCashTool->setCurrentIndex(cursel);
	connect(ui.cboCashTool, SIGNAL(currentIndexChanged(int)), this, SLOT(cashToolChanged(int)));
	cashToolChanged(cursel != 0);
	// 是否使用支付扫码枪
	ui.chbStartGun->setChecked(shopInfo.isUsePayGun == 1);
	// 获取pos打印机
	QString printerName = carishInfo.printerName;
	int printIndex = -1;
	PosPrinter posP;
	std::vector<std::wstring > allPrinters;
	posP.GetAllPrinters(allPrinters, printerName.toStdWString(), printIndex);
	sz = allPrinters.size();
	for (int i = 0; i < sz; ++i)
	{
		ui.cboPrinter->addItem(QString::fromStdWString(allPrinters[i]));
	}
	// 获取 LPT printer
	char lptlastchar = 0;
	if (printIndex < 0 && printerName.contains("LPT"))
	{
		lptlastchar = printerName.at(3).toLatin1();
		printIndex = sz;
	}
	//else if (printIndex == -1)
	//	printIndex = 0;
	int lptnum = 0;
	for (int i = 1; i <= 9; ++i)
	{
		if (PosPrinterLptCls::CanOpenLPT(i))
		{
			char tmpbuf[50];
			sprintf(tmpbuf, "LPT%d", i);
			ui.cboPrinter->addItem(tmpbuf);
			if (lptlastchar == (i + 48))
			{
				printIndex += lptnum;
			}
			++lptnum;
		}
	}
	if (printIndex < 0)
		printIndex = 0;
	ui.cboPrinter->setCurrentIndex(printIndex);
	connect(ui.cboPrinter, SIGNAL(currentIndexChanged(int)), this, SLOT(printerChanged(int)));
	printerChanged(printIndex);
	// 打印设置
	ui.cboSize->setCurrentIndex(carishInfo.printerType);
	ui.printFont->setValue(carishInfo.commentFontSZ);
	// 是否自动打印
	ui.chbAutioPrint->setChecked(shopInfo.isAutoPrint == 1);

	// 快捷键界面
	QStringList hotkeyLs;
	codeSetIO::HOTKEYS &pHotKeys = mZHSetting.hotKeys;
	int oriAscii[2], curIndexs[2];
	oriAscii[0] = pHotKeys.hPrintHandover.qtkey;
	oriAscii[1] = pHotKeys.hTradeInfo.qtkey;
	asciiIntoIndex(hotkeyLs, 2, oriAscii, curIndexs);
	ui.cboPrintHandover->addItems(hotkeyLs);
	ui.cboPrintHandover->setCurrentIndex(curIndexs[0]);
	ui.cboTradeInfo->addItems(hotkeyLs);
	ui.cboTradeInfo->setCurrentIndex(curIndexs[1]);
}

int MainDlg::getImageScaleTag(float &outScale)
{
	switch (mZHSetting.carishInfo.selectRange.priceImageScaleTag)
	{
	case 0:  // 表示不放大
		outScale = 0;
		break;
	case 1:
		outScale = 1.2f;
		break;
	case 2:
		outScale = 1.3f;
		break;
	case 3:
		outScale = 1.5f;
		break;
	case 4:
		outScale = 2.0f;
		break;
	}
	return mZHSetting.carishInfo.selectRange.priceImageScaleTag;
}

void MainDlg::cashToolChanged(int newIndex)
{
	bool hasCashTool = (newIndex != 0);
	ui.btnCheck->setEnabled(hasCashTool);
	ui.gbMoneyPos->setEnabled(hasCashTool);
}

void MainDlg::printerChanged(int newIndex)
{
	bool userPrinter = (newIndex != 0);
	ui.gbPrinter->setEnabled(userPrinter);
	ui.widPrinter2->setEnabled(userPrinter);
}

inline void MainDlg::asciiIntoIndex(QStringList &ioHotkeyLs, int tabNum, int *inASCII, int *outIndex)
{
	ioHotkeyLs << QString::fromLocal8Bit("无"); // -1 index 0
	for (char i = 'A'; i <= 'Z'; ++i)  // ascii 65~~90 index 1~~26
	{
		ioHotkeyLs << QString(i);
	}
	for (char i = '0'; i <= '9'; ++i) // ascii 48~~57  index 27~~36
	{
		ioHotkeyLs << QString(i);
	}
	ioHotkeyLs << "Left" << "Up" << "Right" << "Down"; // ascii 37~~40 index 37~~40

	for (int i = 0; i < tabNum; ++i)
	{
		if (inASCII[i] == -1)
			outIndex[i] = 0;
		else if (inASCII[i] >= 65 && inASCII[i] <= 90)
			outIndex[i] = inASCII[i] - 64;
		else if (inASCII[i] >= 48 && inASCII[i] <= 57)
			outIndex[i] = inASCII[i] - 21;
		else if (inASCII[i] >= 37 && inASCII[i] <= 40)
			outIndex[i] = inASCII[i];
	}
}