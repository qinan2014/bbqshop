﻿#include "bbqshop.h"
#include "AllWindowTitle.h"
#include <QTimer>
#include "zhfunclib.h"
#include <QMenu>
#include "AllExeName.h"
#include <QProcess>
#include "ZhuiHuiMsg.h"
#include "ProcessProtocal.h"
#include "TipWin.h"
#include "ZHSettingRW.h"
#include "HookKeyChar.h"
#include "PayDialog.h"
#include <QDesktopWidget>
#include "ZBase64.h"
#include "HandoverDlg.h"
#include "PosPrinterCls.h"
#include "PosPrinterLptCls.h"
#include "PaySuccessShowDlg.h"
#include <time.h>
#include <QSettings>

bbqshop::bbqshop(QApplication *pApp, QWidget *parent)
	: QWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	setWindowTitle(FLOATWINTITLE);
	setGeometry(0, 0, 5, 5);
	isShowingPayResult = false;
	getOCRPriceTimes = 0;
	isShowingHandoverDlg = false;
	isShiftKeyDown = false;
	// 定时器
	QTimer::singleShot(100,this, SLOT(hide()) );  // 隐藏自己
	// 创建托盘
	createTray();
	// url
	urlServer = new BbqUrlServer(this, &mZHSetting);
	// 启动钩子
	startHook();

	// 信号
	qRegisterMetaType<Json::Value>("Json::Value");
	connect(this, SIGNAL(showTipStringSig(const QString &, const QString &)), this, SLOT(showTipStringSlot(const QString &, const QString &)));
	connect(this, SIGNAL(returnFocusToCashier()), this, SLOT(setFocusOnCashier()));
	connect(this, SIGNAL(manInputESC()), this, SLOT(onESCEvent()));
	connect(this, SIGNAL(checkPayResultSig()), this, SLOT(checkPayResultSlot()));
	connect(this, SIGNAL(tradeResultSig(const Json::Value &)), this, SLOT(tradeNoResult(const Json::Value &)));
	timers[startTimer(60000 * 3, Qt::VeryCoarseTimer)] = TIMER_MEMORYRECORD;
	// 登录
	showLoginDialog();
	// 开启启动
	setAutoRun();
}

bbqshop::~bbqshop()
{
	stopHook();
}

void bbqshop::SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag)
{
	urlServer->SendToURLRecord(logLevel, logModule, logMessage, urlTag);
}

void bbqshop::GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag)
{
	urlServer->GetDataFromServer(inSecondAddr, inApi, inData, urlTag);
}

void bbqshop::GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag)
{
	urlServer->GetDataFromServer1(inUrl, inSecondAddr, inApi, ioRootVal, urlTag);
}

std::string bbqshop::GetPayTool(int inType)
{
	return urlServer->GetPayTool(inType);
}

bool bbqshop::IsImportentOperateNow()
{
	return urlServer->IsImportentOperateNow();
}

void bbqshop::CurlError(std::string url, int res, int urlTag)
{
	emit netError(url.c_str(), res, urlTag);
	ShowTipString(QString::fromLocal8Bit("网络异常，请检查网络！"));
}

bool bbqshop::DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi)
{
	Json::Reader reader;
	Json::Value value;
	bool suc = reader.parse(mRecvJsonStr, value);
	if (suc)
	{
		//ZHFuncLib::NativeLog("", mRecvJsonStr.c_str(), "a");
		switch (urlTag)
		{
		case URL_SWIP_CARD_DLG:
			SwipCardPayURLBack(value, urlApi);
			break;
		case URL_TRADEINFODETAIL:
			emit tradeResultSig(value["data"]);
			break;
		case URL_HANDOVER:
			ExitFromServer(true);
			break;
		case URL_UPLOAD_LAST_LOG_HANDOVER:
			Handover();
			break;
		case URL_UPLOAD_LAST_LOG_EXIT:
			ZHFuncLib::TerminateProcessExceptCurrentOne(MAINDLGEXE);
			ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
			ZHFuncLib::TerminateProcessExceptCurrentOne(UPGRADEEXE);
			ZHFuncLib::TerminateProcessExceptCurrentOne(UPGRADECLIENTICONEXE);
			mainApp->quit();
			break;
		case URL_PRINTHANDOVER_STATEMENT:
			PrintHandoverStatement(value);
			LogOut(URL_HANDOVER);
			break;
		default:
			break;
		}
	}
	//NetStatus(1);
	return true;
}

bool bbqshop::CreateGoodBillRequest(double inOriPrice, double inFavoPrice, int dlgTag)
{
	double curPrice = inOriPrice - inFavoPrice;
	if (curPrice <= 0.0)
	{
		ShowTipString(QString::fromLocal8Bit("价格必须大于0，请重新输入"));
		return false;
	}

	codeSetIO::ShopCashdeskInfo &shopInfo = GetSetting().shopCashdestInfo;
	if (shopInfo.isBind != 1)
	{
		ShowTipString(QString::fromLocal8Bit("软件未进行初始化配置，请先进行设置。"));
		return false;
	}
	Json::Value root;
	root["shop_code"] = shopInfo.shopCode;  // 门店代码
	root["cashdesk_id"] = shopInfo.cashdeskId;
	root["cashdesk_name"] = shopInfo.cashdeskName;
	char str[25];
	itoa(shopInfo.id, str, 10);
	root["cashier_id"] = str;  // 收银员编号
	root["cashier_nickname"] = shopInfo.userName;  // 收银员昵称

	char moneychar[15];
	memset(moneychar, 0, 15);
	sprintf(moneychar, "%.2f", inOriPrice);
	root["orig_fee"] = moneychar;
	memset(moneychar, 0, 15);
	sprintf(moneychar, "%.2f", inFavoPrice);
	root["favo_fee"] = moneychar;
	memset(moneychar, 0, 15);
	sprintf(moneychar, "%.2f", curPrice);
	root["total_fee"] = moneychar;
	root["dcdev_no"] = "";
	root["dcdev_mac"] = "";
	root["out_tradeno"] = "";
	root["m_ticketno"] = "";

	urlServer->GetDataFromServer1(URLTRADE, CARDPAYUNIFIEDORDER, "", root, dlgTag);
	urlServer->SendToURLRecord(LOG_DEBUG, (dlgTag == URL_SWIP_CARD_DLG) ? LOG_PAY1 : LOG_PAY2, root.toStyledString().c_str());

	return true;
}


inline void bbqshop::startHook()
{
#ifdef USEKEYHOOK
	mKeyHook.StopHook();
	//if (mZHSetting.shopCashdestInfo.isUsePayGun == 1)
	mKeyHook.StartHook((HWND)this->winId());
	mKeyHook.EnableInterception(START_HOOK, true);
	mKeyHook.EnableInterception(HOOK_WXKEY, true);
#endif
}

inline void bbqshop::stopHook()
{
#ifdef USEKEYHOOK
	mKeyHook.StopHook();
#endif
}

inline void bbqshop::hookNum(bool isEnable)
{
#ifdef USEKEYHOOK
	mKeyHook.EnableInterception(HOOK_NUM, isEnable);
	hookESC(isEnable);
	hookReturn(isEnable);
#endif
}

inline void bbqshop::hookESC(bool isEnable)
{
#ifdef USEKEYHOOK
	mKeyHook.EnableInterception(HOOK_ESC, isEnable);
#endif
}

inline void bbqshop::hookReturn(bool isEnable)
{
#ifdef USEKEYHOOK
	mKeyHook.EnableInterception(HOOK_RETURN, isEnable);
#endif
}

inline void bbqshop::hookAttempToSetPayKey(bool isEnable)
{
#ifdef USEKEYHOOK
	mKeyHook.EnableInterception(HOOK_SETPAYKEY, isEnable);
#endif
}

inline bool bbqshop::isHooking(int hookKey)
{
#ifdef USEKEYHOOK
	return mKeyHook.IsInterception(hookKey);
#else
	return false;
#endif
}

inline void bbqshop::createTray()
{
	if (!QSystemTrayIcon::isSystemTrayAvailable())      //判断系统是否支持系统托盘图标  
		return;
	QSystemTrayIcon *myTrayIcon = new QSystemTrayIcon(this);  
	QString pngPath = ZHFuncLib::GetWorkPath().c_str();
	pngPath += "/res/traylogo.png";
	myTrayIcon->setIcon(QIcon(pngPath));   //设置图标图片  
	myTrayIcon->setToolTip(QString::fromLocal8Bit("智慧微生活"));    //托盘时，鼠标放上去的提示信息  
	connect(myTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason )), this, SLOT(clickTray(QSystemTrayIcon::ActivationReason )));

	QMenu *myMenu = new QMenu(this);
	QAction *settingAction = new QAction(QString::fromLocal8Bit("设置"),this);
	myMenu->addAction(settingAction);
	connect(settingAction, SIGNAL(triggered()), this, SLOT(showSettingDlg()));

	QAction *quitAction = new QAction(QString::fromLocal8Bit("退出"),this);
	myMenu->addAction(quitAction);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(programQuit()));
	
	myTrayIcon->setContextMenu(myMenu);     //设置托盘上下文菜单  
	myTrayIcon->show();  
}

void bbqshop::programQuit()
{
	ZHFuncLib::TerminateProcessExceptCurrentOne(LOGINEXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(MAINDLGEXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(UPGRADEEXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(UPGRADECLIENTICONEXE);
	mainApp->quit();
}

void bbqshop::showSettingDlg()
{
	QString program = ZHFuncLib::GetWorkPath().c_str();
	program += "/";
	program += MAINDLGEXE;
	QStringList arguments;
	QProcess *process = new QProcess(this);
	QStringList args;
	codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	args << deskInfo.account;

	process->start(program, args);
}

void bbqshop::showLoginDialog()
{
	QString program = ZHFuncLib::GetWorkPath().c_str();
	program += "/";
	program += LOGINEXE;
	QStringList arguments;
	QProcess *process = new QProcess(this);
	QStringList args;

	process->start(program, args);
}


bool bbqshop::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	Q_UNUSED(eventType);
	//MSG是个消息结构体，里面有类型、数据等。
	MSG* msg = reinterpret_cast<MSG*>(message);
	switch (msg->message)
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);
			if (cds->dwData == ZHIHUI_CODE_MSG)
			{
				QString strMessage = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData), cds->cbData);
				parseProcessJsonData(strMessage);
				*result = 1;
				return true;
			}
		}
		break;
	case ZHIHUI_MANINPUT_MSG:
		hookManInputCodeMsg(msg);
		return true;
	case ZHIHUI_CODE_MSG:
		hookScanCodeMsg(msg);
		return true;
	default:
		break;
	}
	return false;
}

inline void bbqshop::parseProcessJsonData(QString inJson)
{
	Json::Reader reader;
	Json::Value value;
	bool suc = reader.parse(inJson.toStdString(), value);
	if (!suc)
		return;
	int header = value[PRO_HEAD].asInt();
	switch (header)
	{
	case TO_SHOWTIP:
		ShowTipString(value[PRO_TIPSTR].asCString());
		break;
	case TO_FLOATWIN_LOGININFO:
		processJsonSaveLoginInfo(value);
		break;
	case TO_FLOATWIN_CLOSEMAINDLG:
		processJsonOnMainDlgClose(value);
		break;
	case TO_FLOATWIN_STARTOCR:
		processJsonStartOCR();
		break;
	case TO_FLOAT_SHOWMAINDLG:
		processJsonShowMainDlg();
		break;
	case TO_FLOATWIN_REWRITESETTING:
		processJsonRereadSetting();
		break;
	case RETURN_PRICE:
		processJsonShowPrice(value);
		break;
	case TO_FLOATWIN_EDITPAYKEY:
		processJsonPayKeySet(value);
		break;
	case OCR_PREPARED:
		processJsonOCRPrepared(value);
		break;
	default:
		break;
	}
}

inline void bbqshop::hookManInputCodeMsg(MSG* msg)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)msg->lParam;
	if (isHooking(HOOK_SETPAYKEY))
	{
		hookSettingPayKey(p);
		return;
	}
	if (p->vkCode == mZHSetting.hotKeys.hWXKey.qtkey || p->vkCode == mZHSetting.hotKeys.hAlipayKey.qtkey)
	{
		showPayDialog();
		return;
	}
	switch (p->vkCode)
	{
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case '.':
		hookManInputNum(p->vkCode);
		break;
	case VK_ESCAPE:
		emit manInputESC();
		break;
	case VK_RETURN:
		emit manInputEnter();
		break;
	case VK_LSHIFT:
	case VK_RSHIFT:
		hookManInputShift(p);
		break;
	default:
		break;
	}
}

inline void bbqshop::hookScanCodeMsg(MSG* msg)
{
#define HOOKNUM 20
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)msg->lParam;
	static int hookInterceptionIndex = 0;
	static KBDLLHOOKSTRUCT hookInterception[HOOKNUM]; // 所拦截到的数字
	if (hookInterceptionIndex >= HOOKNUM)
		return;
	hookInterception[hookInterceptionIndex++] = *p;
	if(p->vkCode == VK_RETURN )
	{
		PayDialog *dlg = PayDialog::InitInstance(false);
		if (dlg != NULL)
		{
			QString scanCode;
			int charLen = hookInterceptionIndex - 1;
			for (int i = 0; i < charLen; ++i)
			{
				scanCode += (TCHAR)hookInterception[i].vkCode;
			}
			dlg->SetScanCode(scanCode);
		}
		hookInterceptionIndex = 0;
	}
}


void bbqshop::ShowTipString(QString inTip, QString inTitle)
{
	emit showTipStringSig(inTip, inTitle);
}

void bbqshop::showTipStringSlot(const QString &inTip, const QString &inTitle)
{
	TipWin *dlgTip = new TipWin(inTip, inTitle, this);
	connect(dlgTip, SIGNAL(closeTipWin()), this, SLOT(onCloseTipWin()));
	dlgTip->show();
}

void bbqshop::onCloseTipWin()
{
	emit returnFocusToCashier();
}

void bbqshop::setFocusOnCashier()
{
	if (isOperatorOtherDlg())
		return;
	std::wstring softname = ZHFuncLib::StringToWstring(mZHSetting.carishInfo.windowName);
	if (softname.empty())
		return;
	HWND hwnd = ::FindWindow(NULL, softname.c_str());
	if (hwnd == NULL)
		return;
	//hookNum(false);
	BOOL suc = ::SetForegroundWindow(hwnd);//设置后能成功激活进程窗口，但是无法将输入焦点转移到进程窗口

	::BringWindowToTop(hwnd);
	::SetFocus(hwnd);

	mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP,5,5,0,0); 
}

inline void bbqshop::processJsonSaveLoginInfo(const Json::Value &value)
{
	const char *shopCode = value["SHOP_CODE"].asCString();
	int role = value["ROLE"].asInt();
	const char *userName = value["USER_NAME"].asCString();
	int id = value["ID"].asInt();
	const char *shopName = value["SHOP_NAME"].asCString();
	const char *shopID = value["SHOP_ID"].asCString();
	int shopType = value["SHOP_TYPE"].asInt();
	int workStatus = value["WORK_STATUS"].asInt();
	const char *account = value["ACCOUNT"].asCString();
	const char *loginTime = value["LOGIN_TIME"].asCString();
	const char *extTime = NULL;
	if (value.isMember("EXIT_TIME"))
		extTime = value["EXIT_TIME"].asCString();

	codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	memcpy(deskInfo.shopCode, shopCode, strlen(shopCode));
	deskInfo.shopCode[strlen(shopCode)] = 0;
	deskInfo.role = role;
	memcpy(deskInfo.userName, userName, strlen(userName));
	deskInfo.userName[strlen(userName)] = 0;
	deskInfo.id = id;
	memcpy(deskInfo.shopName, shopName, strlen(shopName));
	deskInfo.shopName[strlen(shopName)] = 0;
	deskInfo.shopid = atoi(shopID);
	deskInfo.shoptype = shopType;
	deskInfo.workStatus = workStatus;
	memcpy(deskInfo.account, account, strlen(account));
	deskInfo.account[strlen(account)] = 0;
	memcpy(deskInfo.loginTime, loginTime, strlen(loginTime));
	deskInfo.loginTime[strlen(loginTime)] = 0;
	if (extTime != NULL)
	{
		memcpy(deskInfo.exitTime, extTime, strlen(extTime));
		deskInfo.exitTime[strlen(extTime)] = 0;
	}

	// 读取配置
	ZHSettingRW settingRW(mZHSetting);
	settingRW.ReadZHSetting();
	setHookPayKeyValueFromZHSetting();
}

inline void bbqshop::processJsonOnMainDlgClose(const Json::Value &value)
{
	if (value[PRO_DLG_STATUS].asInt() == 0)
		emit returnFocusToCashier();
}

inline void bbqshop::processJsonShowMainDlg()
{
	Json::Value mValData;
	mValData[PRO_HEAD] = TO_MAINDLG_IMPORTANTDATA;
	codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	mValData[PRO_SHOP_CODE] = deskInfo.shopCode;
	mValData[PRO_ROLE] = deskInfo.role;
	mValData[PRO_USERNAME] = deskInfo.userName;
	mValData[PRO_ID] = deskInfo.id;
	mValData[PRO_SHOPTYPE] = deskInfo.shoptype;
	mValData[PRO_SHOP_NAME] =(deskInfo.shopName);
	mValData[PRO_SHOPID] = deskInfo.shopid;
	mValData[PRO_WORKSTATUS] = deskInfo.workStatus;
	mValData[PRO_ACCOUNT] = deskInfo.account;
	mValData[PRO_LOGINTIME] = deskInfo.loginTime;
	mValData[PRO_EXITTIME] = deskInfo.exitTime;
	HWND hwnd = ::FindWindowW(NULL, MAINDLGTITLEW);
	ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
}

inline void bbqshop::processJsonRereadSetting()
{
	ZHSettingRW settingRW(mZHSetting);
	settingRW.ReadZHSetting();
	setHookPayKeyValueFromZHSetting();
	//char tmpbuf[100];
	//sprintf(tmpbuf, "isgetprice in time, %d", mZHSetting.shopCashdestInfo.isGetPriceActualTime);
	//LogError(tmpbuf, "a");
	if (mZHSetting.shopCashdestInfo.isGetPriceActualTime == 1)
	{
		std::vector<int > ids;
		ZHFuncLib::GetTargetProcessIds(OCREXE, ids);
		if (ids.size() > 0)
			sendCashInfo();
	}
	else
		ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);

	if (mZHSetting.shopCashdestInfo.isUsePayGun == 1)
	{
		startHook();
	}
	else
	{
		stopHook();
	}

	ShowTipString(QString::fromLocal8Bit("保存完成！"));
}

inline void bbqshop::processJsonShowPrice(const Json::Value &inJson)
{
	getOCRPriceTimes = 0;
	QString pricestr = inJson[PRO_OCR_PRICE].asCString();
	int priceerror = inJson[PRO_OCR_ERROR].asInt();
	if (priceerror == PRO_OCR_ERROR_NAME_EMPTY)
	{
		//priceLab->setText("0.00");
		//killTargetTimer(TIMER_GETPRICE);
		QTimer::singleShot(200,this, SLOT(sendCashInfo()) );
		return;
	}
	if (priceerror != PRO_OCR_ERROR_SOFT_NOERROR)
	{
		stopGetOCRPriceTimer();
		return;
	}
	if (isPriceNum(pricestr));
	{
		PayDialog *dlg = PayDialog::InitInstance(false);
		if (dlg != NULL)
		{
			dlg->SetMoney(pricestr);
			if (pricestr.toDouble() > 0.00)
				stopGetOCRPriceTimer();
		}
	}
}

inline void bbqshop::processJsonPayKeySet(const Json::Value &inJson)
{
	int editstatus = inJson[PRO_PAYKEY_EDIT].asInt();
	switch (editstatus)
	{
	case Modify_WXKey:
	case Modify_AlipayKey:
		hookAttempToSetPayKey(true);
		break;
	case Finished_WXKey:
	case Finished_AlipayKey:
		hookAttempToSetPayKey(false);
		break;
	default:
		break;
	}
}

inline void bbqshop::processJsonOCRPrepared(const Json::Value &value)
{
	int status = value[PRO_OCR_PREPARE_STATUS].asInt();
	if (status == 1 && PayDialog::InitInstance(false) != NULL)
	{
		if (!isHasTargetTimer(TIMER_GETPRICE))
			timers[startTimer(300, Qt::VeryCoarseTimer)] = TIMER_GETPRICE;		
	}
}

bool bbqshop::isPriceNum(QString &ioPriceStr)
{
	if (ioPriceStr.isEmpty()) 
		return false;	
	if (ioPriceStr.at(0) == '-')
		return false;
	ioPriceStr.replace('o', '0');
	ioPriceStr.replace(']', '1');
	QString newStr;
	// 确保第一个字符是数字
	int tmpLen = ioPriceStr.length();
	for (int i = 0; i < tmpLen; ++i)
	{
		QChar tmpchar = ioPriceStr.at(i);
		if ((tmpchar >= '0' && tmpchar <= '9') || tmpchar == '.')
		{
			newStr += tmpchar;
		}
	}
	ioPriceStr = newStr;

	if (ioPriceStr.isEmpty()) 
		return false;	
	int bytelen = ioPriceStr.length();
	QChar tmpchar = ioPriceStr.at(0);
	// 如果第一个字符不是数字则直接返回
	if (tmpchar < '0' || tmpchar > '9')
		return false;

	// 第一个数字是0，第二个数字不是小数点返回false
	if (bytelen > 1 && tmpchar == '0' && ioPriceStr.at(1) != '.')
		return false;

	short dotNum = 0;
	int dotPos = -1;
	for (int i = 1; i < bytelen; ++i)
	{
		QChar bychar = ioPriceStr.at(i);
		// 小数点的判断
		if (bychar == '.')
		{
			dotPos = i;
			++dotNum;
		}
		if (dotNum > 1)
			return false;

		if ((bychar < '0' || bychar > '9') && bychar != '.' && bychar != 10){
			return false;}

		//char numch[50];
		//sprintf(numch, "char is %c assic: %d\r\n", bychar, bychar);
		//LogError(numch, "a");
	}

	// 小数点后超过2位的判断
	if (dotPos != -1 &&(bytelen - 1 - dotPos > 2))
		return false;

	return true;
}


codeSetIO::ZHIHUISETTING &bbqshop::GetSetting()
{
	return mZHSetting;
}

void bbqshop::showPayDialog()
{
	if (strlen(mZHSetting.shopCashdestInfo.account) == 0)
		return;
	PayDialog *dlg = PayDialog::InitInstance(true, this);
	if (dlg != NULL)
	{
		connect(dlg, SIGNAL(closeThisDlg(bool )), this, SLOT(onClosePayDlg(bool)));
		connect(this, SIGNAL(manInputESC()), dlg, SLOT(closeSelf()));
		connect(this, SIGNAL(manInputEnter()), dlg, SLOT(ClickPay()));
		connect(dlg, SIGNAL(micropaySucess(QString )), this, SLOT(saveCurrentTradeNo(QString )));
		connect(this, SIGNAL(netError(QString, int, int)), dlg, SLOT(onNetError(QString, int, int)));
		hookNum(true);
		startGetOCRPrice();
		dlg->show();
	}
}

void bbqshop::onClosePayDlg(bool haspayed)
{
	stopGetOCRPriceTimer();
	hookNum(false);
	emit returnFocusToCashier();

}

inline void bbqshop::hookManInputNum(DWORD vkCode)
{
	if (isShiftKeyDown)
	{
		if (vkCode == 49)
		{
			QString program = ZHFuncLib::GetWorkPath().c_str();
			program += "/";
			program += MAINDLGEXE;
			QStringList arguments;
			QProcess *process = new QProcess(this);
			QStringList args;
			codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
			args << deskInfo.account;

			process->start(program, args);
		}
	}
}

inline void bbqshop::hookManInputShift(PKBDLLHOOKSTRUCT p)
{
	if (p->flags < 128)  // 这是按下
	{
		isShiftKeyDown = true;
		//hookNum(true);
	}
	else
	{
		isShiftKeyDown = false;
		if (isOperatorOtherDlg())
			return;
		//hookNum(false);
	}
}

void bbqshop::processJsonStartOCR()
{
	stopGetOCRPriceTimer();
	QString program = ZHFuncLib::GetWorkPath().c_str();
	program += "/";
	program += OCREXE;
	QProcess *process = new QProcess(this);
	QStringList args;
	//args << QString::number(1);
	process->start(program, args);
	SendToURLRecord(LOG_DEBUG, LOG_RESTARTOCR, "restart-ocr-process", URL_RECORD_STARTOCR);
}

void bbqshop::sendCashInfo()
{
	Json::Value mValData;
	mValData[PRO_HEAD] = TO_OCR_CASHINFO;
	mValData[PRO_OCR_FROMDLG] = PRO_OCR_FROM_FLOATWIN;
	codeSetIO::CarishDesk &carishInfo = GetSetting().carishInfo;
	if (strlen(carishInfo.windowName) < 1)
	{
		ZHFuncLib::NativeLog("", "windowName empty", "a");
		return;
	}
	codeSetIO::SelectRange &curSelRange = carishInfo.selectRange;
	ZBase64 base64;
	mValData[PRO_OCR_CASHNAME] = base64.Encode(carishInfo.windowName, strlen(carishInfo.windowName));
	mValData[PRO_OCR_REALITIVETYPE] = curSelRange.relitiveType;
	mValData[PRO_OCR_SELX] = curSelRange.xCenterDistance;
	mValData[PRO_OCR_SELY] = curSelRange.yCenterDistance;
	mValData[PRO_OCR_SELW] = curSelRange.widImage;
	mValData[PRO_OCR_SELH] = curSelRange.heightImage;
	mValData[PRO_OCR_SCALETAG] = curSelRange.priceImageScaleTag;
	mValData[PRO_OCR_BINA] = curSelRange.imageBinaryzation;

	HWND hwnd = ::FindWindowW(NULL, OCRDLGTITLEW);
	ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
}

void bbqshop::startGetOCRPrice()
{
	if (mZHSetting.shopCashdestInfo.isGetPriceActualTime != 1)
	{
		killTargetTimer(TIMER_GETPRICE);
		return;
	}
	// ocr方式获得价格
	std::vector<int > ids;
	ZHFuncLib::GetTargetProcessIds(OCREXE, ids);
	if (ids.size() == 0)
	{
		killTargetTimer(TIMER_GETPRICE);
		getOCRPriceTimes = 0;
		processJsonStartOCR();
		QTimer::singleShot(200,this, SLOT(sendCashInfo()));
		return;
	}
	if (!isHasTargetTimer(TIMER_GETPRICE))
		timers[startTimer(300, Qt::VeryCoarseTimer)] = TIMER_GETPRICE;
}

inline void bbqshop::stopGetOCRPriceTimer()
{
	// ocr方式
	killTargetTimer(TIMER_GETPRICE);
}

inline bool bbqshop::isHasTargetTimer(int targetTimer)
{
	bool hasFloatTimer = false;
	std::map<int, int>::iterator iter = timers.begin();
	for (iter; iter != timers.end(); ++iter)
	{
		if (iter->second == targetTimer)
		{
			hasFloatTimer = true;
			break;
		}
	}
	return hasFloatTimer;
}

inline void bbqshop::killTargetTimer(int targetTimer)
{
	std::map<int, int>::iterator iter = timers.begin();
	for (iter; iter != timers.end(); ++iter)
	{
		if (iter->second == targetTimer)
		{
			killTimer(iter->first);
			timers.erase(iter);
			break;
		}
	}
}

void bbqshop::timerEvent(QTimerEvent * event)
{
	int timetag = event->timerId();
	switch (timers[timetag])
	{
	case TIMER_GETPRICE:
		getOCRPrice();
		break;
	case TIMER_MEMORYRECORD:
		urlServer->RecordMemoryInfo("Get memory by fixed time");
		break;
	default:
		break;
	}
}

void bbqshop::getOCRPrice()
{
	if (getOCRPriceTimes > 4)
	{
		ZHFuncLib::NativeLog("", "after send get price cmd, there is not return", "a");
		ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
		startGetOCRPrice();
		return;
	}
	++getOCRPriceTimes;
	Json::Value mValData;
	mValData[PRO_HEAD] = TO_OCR_GETPRICE;

	HWND hwnd = ::FindWindowW(NULL, OCRDLGTITLEW);
	bool suc = ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
	if (!suc && getOCRPriceTimes > 4)
	{
		ZHFuncLib::NativeLog("", "send getprice cmd failed", "a");
		ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
		startGetOCRPrice();
	}
}

void bbqshop::onESCEvent()
{
	// 正在弹出着任何对话框 此响应时间都不做操作
	if (isOperatorOtherDlg())
		return;
	// 余下的这种情况是按着Shift键
	if (strlen(mZHSetting.shopCashdestInfo.account) == 0)
		return;
	HandoverDlg handoverDlg(this);
	isShowingHandoverDlg = true;
	hookESC(true);
	hookReturn(true);
	connect(this, SIGNAL(manInputESC()), &handoverDlg, SLOT(reject()));
	connect(this, SIGNAL(manInputEnter()), &handoverDlg, SLOT(accept()));
	int exeres = handoverDlg.exec();
	hookReturn(false);
	hookESC(false);
	isShowingHandoverDlg = false;
	if (exeres == QDialog::Accepted)
	{
		// 先打印当班对账单，再进行交班
		//LogOut(URL_HANDOVER);
		PrintHandoverStatementRequest();
	}
	else
	{
		emit returnFocusToCashier();
	}
	
}

bool bbqshop::isOperatorOtherDlg()
{
	PayDialog *dlg = PayDialog::InitInstance(false);
	if (dlg != NULL)
		return true;
	std::vector<int > ids;
	ZHFuncLib::GetTargetProcessIds(MAINDLGEXE, ids);
	if (ids.size() > 0)
		return true;
	if (isShowingHandoverDlg)
		return true;
	if (isShowingPayResult)
		return true;
	return false;
}

void bbqshop::SwipCardPayURLBack(const Json::Value &value, std::string urlApi)
{
	std::string retCode = value["return_code"].asString();
	std::string resCode = value["result_code"].asString();
	bool isReturnSuc = !(retCode == "FAIL" || resCode == "FAIL" || retCode == "fail" || resCode == "fail" );
	if (!isReturnSuc && value.isMember("return_msgs"))
	{
		ShowTipString((value["return_msgs"].asCString()));
	}

	std::string dataApi = value["api"].asString();
	if (dataApi.empty())
		dataApi = urlApi;
	PayDialog *cardDlg = PayDialog::InitInstance(false);
	if (cardDlg != NULL)
	{
		if (dataApi == CARDPAYUNIFIEDORDER){
			cardDlg->CreatePayBillSucess(isReturnSuc, value);
		}
		else if(dataApi == CARDPAYMICROPAYAPI)
			cardDlg->CardPayInfo(isReturnSuc, value);
	}
}

void bbqshop::saveCurrentTradeNo(QString tradeNo)
{
	curTradeNo = tradeNo;
	QTimer::singleShot(200,this, SLOT(requestTradeInfoByNo()) );
}

void bbqshop::checkPayResultSlot()
{
	QTimer::singleShot(200,this, SLOT(requestTradeInfoByNo()) );
}

void bbqshop::requestTradeInfoByNo()
{
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
	Json::Value mValData;
	mValData["shopid"] = shopInfo.shopid;
	mValData["tradeno"] = curTradeNo.toStdString();
	std::string itemVal = mValData.toStyledString();
	std::string::size_type rePos;
	while ((rePos = itemVal.find(" ")) != -1) {
		itemVal.replace(rePos, 1, "");
	}
	GetDataFromServer("api/app/v1", TRADEINFODETAILAPI, itemVal, URL_TRADEINFODETAIL);
}

void bbqshop::tradeNoResult(const Json::Value & inData)
{
	const char *tradeNo = inData["TRADE_NO"].asCString();
	double tradeMoney = inData["ORIG_FEE"].asDouble();
	char orig_fee[10];
	sprintf(orig_fee, "%0.2f", tradeMoney);
	double favoFee = inData["TOTAL_FEE"].asDouble();
	char favo_fee[10];
	sprintf(favo_fee, "%0.2f", favoFee);
	double payFee = inData["PAY_FEE"].asDouble();
	char pay_fee[10];
	sprintf(pay_fee, "%0.2f", payFee);

	const char *tradeTm = inData["CTIME"].asCString();
	int tradeStatus = inData["STATUS"].asInt();
	int paytype = inData["PAY_TYPE"].asInt();
	if (tradeStatus == 0) // 表示还未到账，要继续查询
	{
		emit checkPayResultSig();
		return;
	}
	if (tradeStatus == 1) // 支付成功
	{
		isShowingPayResult = true;
		emit manInputESC(); // 目的是关闭支付对话框
		QString payTypeIcon;
		switch (paytype)
		{
		case 1:
			payTypeIcon = "/res/paytool_ali.png";
			break;
		case 2:
		case 9:
			payTypeIcon = "/res/paytool_wx.png";
			break;
		case 4:
			payTypeIcon = "/res/paytool_jd.png";
			break;
		case 5:
			payTypeIcon = "/res/paytool_msyh.png";
			break;
		}
		// get private account 
		codeSetIO::ShopCashdeskInfo &carishInfo = GetSetting().shopCashdestInfo;
		if (carishInfo.isAutoPrint == 1)
			printPayResult(paytype, tradeNo, orig_fee, favo_fee, pay_fee);
		PaySuccessShowDlg dlg(payTypeIcon, this);
		connect(this, SIGNAL(manInputEnter()), &dlg, SLOT(accept()));
		dlg.SetPaySuccessInfo(tradeNo, pay_fee);
		hookReturn(true);
		INT_PTR nResponse = dlg.exec();
		bool clickPrint = false;
		if (nResponse == QDialog::Accepted)
		{
			if (carishInfo.isAutoPrint != 1)  // if equal 1, it has been printed
				clickPrint = true;
		}
		isShowingPayResult = false; 
		hookReturn(false);
		emit returnFocusToCashier();
		if (clickPrint)
			printPayResult(paytype, tradeNo, orig_fee, favo_fee, pay_fee);
		QString showStatus = QString::fromLocal8Bit(urlServer->GetPayTool(paytype).c_str());
		showStatus += QString::fromLocal8Bit("，已收款");
		showStatus += pay_fee;
		showStatus += QString::fromLocal8Bit("元");

		SendToURLRecord(LOG_DEBUG, LOG_PUSHPAYRES, inData.toStyledString().c_str(), URL_RECORE_PAYRESULT);

		//ShowTipString(showStatus);
	}
}

inline void bbqshop::printPayResult(int pay_type, const char *trade_no, const char *orig_fee, const char *favo_fee, const char *pay_fee)
{
#ifdef ENABLE_PRINTER
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
	QString printerName = mZHSetting.carishInfo.printerName;
	std::string paytypestr = GetPayTool(pay_type);
	paytypestr += "支付";

	time_t t = time(0);
	struct tm *p = gmtime(&t);
	char ctimetmp[25];
	memset(ctimetmp, 0, 25);
	sprintf(ctimetmp, "%d-%d-%d %d:%d:%d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour + 8, p->tm_min, p->tm_sec);

	if (printerName.contains("LPT"))
	{
		char tmpchar = printerName.at(3).toLatin1();
		if (tmpchar >= '1' && tmpchar <= '9')
		{
			PosPrinterLptCls mPrinter;
			mPrinter.Prepare(printerName.toStdString().c_str());
			PrintHeaderLPT(mPrinter, shopInfo, "支付存根（收银留存）");
			mPrinter.PrintString("订单编号：",PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(trade_no, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			mPrinter.PrintString("支付方式：", PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(paytypestr, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			mPrinter.PrintString("原价金额：", PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(orig_fee, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			mPrinter.PrintString("优惠金额：", PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(favo_fee, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			mPrinter.PrintString("支付金额：", PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(pay_fee, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			mPrinter.PrintString("支付时间：", PosPrinterLptCls::NORMAL);
			mPrinter.PrintString(ctimetmp, PosPrinterLptCls::NORMAL);
			mPrinter.PrintString("\r\n", PosPrinterLptCls::NORMAL);

			PrintEndLPT(mPrinter);
		}
	}
	else {
		PosPrinter *mPrinter = new PosPrinter();
#define NAMEPOS 0.02
#define CONTENTPOS 0.35
#define NEXTLINE 0.25
#define SPACE1 35
		if (mPrinter->PreparePrinter(printerName.toStdWString()))
		{
			PrintHeader(mPrinter, shopInfo, this, L"支付存根（收银留据）");
			mPrinter->AddNewLine(NAMEPOS, L"订单编号：",SPACE1);
			mPrinter->AppendLineString(CONTENTPOS, StrToWStr(trade_no));
			mPrinter->AddNewLine(NAMEPOS, L"");
			mPrinter->AddNewLine(NAMEPOS, L"支付方式：",SPACE1);
			QString tmpstr = QString::fromLocal8Bit(paytypestr.c_str());
			mPrinter->AppendLineString(CONTENTPOS, tmpstr.toStdWString());

			mPrinter->AddNewLine(NAMEPOS, L"原价金额：",SPACE1);
			mPrinter->AppendLineString(CONTENTPOS, StrToWStr(orig_fee));
			mPrinter->AddNewLine(NAMEPOS, L"优惠金额：",SPACE1);
			mPrinter->AppendLineString(CONTENTPOS, StrToWStr(favo_fee));
			mPrinter->AddNewLine(NAMEPOS, L"支付金额：",SPACE1);
			mPrinter->AppendLineString(CONTENTPOS, StrToWStr(pay_fee));
			mPrinter->AddNewLine(NAMEPOS, L"支付时间：",SPACE1);
			mPrinter->AppendLineString(CONTENTPOS, StrToWStr(ctimetmp));

			PrintEnd(mPrinter);
#undef NAMEPOS
#undef CONTENTPOS
#undef NEXTLINE
#undef SPACE1
			mPrinter->CallPrinter();
		}
		delete mPrinter;
	}
#endif
}

int bbqshop::GetCommentFontSZ()
{
	return mZHSetting.carishInfo.commentFontSZ;
}

int bbqshop::GetPrinterDeviceWidth()
{
	codeSetIO::CarishDesk &printer = mZHSetting.carishInfo;
	int deviceWidth = -1;
	switch (printer.printerType)
	{
	case 0:
		deviceWidth = printer.deviceWidth58;
		break;
	case 1:
		deviceWidth = printer.deviceWidth80;
		break;
	default:
		break;
	}
	if (deviceWidth > 1000)
		return -1;
	if (deviceWidth < 100)
		return -1;
	return deviceWidth;
}

std::wstring bbqshop::StrToWStr(const char *inChar)
{
	QString tmpStr(inChar);
	return tmpStr.toStdWString();
}

void bbqshop::LogOut(int urlTag)
{
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
	Json::Value mValData;
	mValData["id"] = shopInfo.id;

	std::string itemVal = mValData.toStyledString();
	std::string::size_type rePos;
	while ((rePos = itemVal.find(" ")) != -1) {
		itemVal.replace(rePos, 1, "");
	}
	GetDataFromServer("api/app/v1", USERLOGOUTAPI, itemVal, urlTag);
}

void bbqshop::ExitFromServer(bool isHandover)
{
	if (isHandover)
		SendToURLRecord(LOG_DEBUG, LOG_LOGOUT, "logout", URL_UPLOAD_LAST_LOG_HANDOVER);
	else
		SendToURLRecord(LOG_DEBUG, LOG_LOGOUT, "logout", URL_UPLOAD_LAST_LOG_EXIT);
}

void bbqshop::Handover()
{
	QString program = ZHFuncLib::GetWorkPath().c_str();
	program += "/";
	program += RELOGINEXE;
	QProcess *process = new QProcess();
	QStringList args;
	process->start(program, args);
}

void bbqshop::PrintHandoverStatementRequest()
{
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
	if (shopInfo.isBind != 1)
	{
		return;
	}
	Json::Value root;
	char str[25];
	itoa(shopInfo.id, str, 10);
	root["uid"] = str; 
	root["cashdesk_id"] = shopInfo.cashdeskId;
	root["cashdesk_name"] = shopInfo.cashdeskName;
	root["dcdev_no"] = shopInfo.dcdevNo;  
	root["dcdev_mac"] = shopInfo.dcdevMac;

	GetDataFromServer1(URLCLOUND, PRINTSTATEMENTAPI, "", root, URL_PRINTHANDOVER_STATEMENT);
}

void bbqshop::PrintHandoverStatement(const Json::Value & inVal)
{
	//CFloatWindowDlg *pWnd = (CFloatWindowDlg *)theApp.m_pMainWnd;
	const char *return_msgs = inVal["return_msgs"].asCString();
	std::string retCode = inVal["return_code"].asString();
	std::string resCode = inVal["result_code"].asString();
	bool isReturnSuc = !(retCode == "FAIL" || resCode == "FAIL" || retCode == "fail" || resCode == "fail" );
	if (isReturnSuc)
	{
		codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting.shopCashdestInfo;
		QString printerName = mZHSetting.carishInfo.printerName;

		if (printerName.contains("LPT"))
		{
			char tmpchar = printerName.at(3).toLatin1();
			if (tmpchar >= '1' && tmpchar <= '3')
			{
				std::string printData = inVal["print_data"].asString();
				QString qprintdata = printData.c_str();
				qprintdata.replace(QString("\\n"), QString("\r\n"));
				PosPrinterLptCls mPrinter;
				mPrinter.Prepare(printerName.toStdString().c_str());
				mPrinter.PrintString(qprintdata.toLocal8Bit().toStdString(), PosPrinterLptCls::NORMAL);
				return;
			}
		}

#ifdef ENABLE_PRINTER
		PosPrinter *mPrinter = new PosPrinter();
#define NAMEPOS 0.02
#define CONTENTPOS 0.75
#define NEXTLINE 0.3
#define SPACE1 35

		if (mPrinter->PreparePrinter(printerName.toStdWString()))
		{
			mPrinter->SetDeviceWidth(GetPrinterDeviceWidth());

			std::string printData = inVal["print_data"].asString();
			int pos = -1;
			//const int lineMaxLen = 40;
			while ((pos = printData.find("\\n")) != std::string::npos)
			{
				QString tmpLine = printData.substr(0, pos).c_str();
				mPrinter->AddNewLine(NAMEPOS, tmpLine.toStdWString(), SPACE1, GetCommentFontSZ());
				printData = printData.substr(pos + 2);
			}
			mPrinter->CallPrinter();
		}

		delete mPrinter;
#undef NAMEPOS
#undef CONTENTPOS
#undef NEXTLINE
#undef SPACE1
#endif
	}
	else
	{
		ShowTipString(return_msgs);
	}
}

void bbqshop::setAutoRun(bool isAuto)
{
	QSettings *reg = new QSettings(REGEDITRUN,QSettings::NativeFormat);
	QString application_path = ZHFuncLib::GetWorkPath().c_str();
	application_path += "/";
	application_path += BBQSHOPEXE;
	if (isAuto)
		reg->setValue(APPLICATIONNAME, application_path.replace("/", "\\"));
	else
		reg->setValue(APPLICATIONNAME, "");
}

void bbqshop::hookSettingPayKey(PKBDLLHOOKSTRUCT p)
{
	std::vector<int > ids;
	ZHFuncLib::GetTargetProcessIds(MAINDLGEXE, ids);
	if (ids.size() == 0)
	{
		hookAttempToSetPayKey(false);
		return;
	}
	Json::Value mValData;
	mValData[PRO_HEAD] = TO_MAINDLG_SET_PAYKEY;
	mValData[PRO_PAYKEY_VALUE] = (int )p->vkCode;

	HWND hwnd = ::FindWindowW(NULL, MAINDLGTITLEW);
	ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
}

inline void bbqshop::setHookPayKeyValueFromZHSetting()
{
#ifdef USEKEYHOOK
	mKeyHook.SetPayKey(mZHSetting.hotKeys.hWXKey.qtkey, mZHSetting.hotKeys.hAlipayKey.qtkey);
#endif
}