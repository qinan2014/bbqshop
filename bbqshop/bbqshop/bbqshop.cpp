#include "bbqshop.h"
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

bbqshop::bbqshop(QApplication *pApp, QWidget *parent)
	: QWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	setWindowTitle(FLOATWINTITLE);
	setGeometry(0, 0, 5, 5);
	isShowingPayResult = false;
	// 定时器
	QTimer::singleShot(100,this, SLOT(hide()) );  // 隐藏自己
	// 创建托盘
	createTray();
	// 读取配置
	ZHSettingRW settingRW(mZHSetting);
	settingRW.ReadZHSetting();
	// url
	urlServer = new BbqUrlServer(this, &mZHSetting);
	// 启动钩子
	startHook();

	// 信号
	connect(this, SIGNAL(showTipStringSig(const QString &, const QString &)), this, SLOT(showTipStringSlot(const QString &, const QString &)));
	connect(this, SIGNAL(returnFocusToCashier()), this, SLOT(setFocusOnCashier()));
	// 登录
	showLoginDialog();
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
	ShowTipString(QString::fromLocal8Bit("网络异常，请检查网络！"));
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
	mKeyHook.EnableInterception(HOOK_ESC, isEnable);
	mKeyHook.EnableInterception(HOOK_RETURN, isEnable);
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

void bbqshop::parseProcessJsonData(QString inJson)
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
	default:
		break;
	}
}

inline void bbqshop::hookManInputCodeMsg(MSG* msg)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)msg->lParam;
	switch (p->vkCode)
	{
	case HOOK_KEY_WX:
		showPayDialog();
		break;
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
	if (isShowingPayResult)
		return;
	if (PayDialog::InitInstance(false) != NULL)
		return;
	//std::vector<int > ids;
	//ZHFuncLib::GetTargetProcessIds(MAINDLGEXE, ids);
	//if (ids.size() != 0)
	//	return;
	//if (isVisible())
	//	emit returnFocusToCashier();
}

void bbqshop::setFocusOnCashier()
{
	if (isShowingPayResult)
		return;
	std::wstring softname = ZHFuncLib::StringToWstring(mZHSetting.carishInfo.windowName);
	if (softname.empty())
		return;
	HWND hwnd = ::FindWindow(NULL, softname.c_str());
	if (hwnd == NULL)
		return;
	BOOL suc = ::SetForegroundWindow(hwnd);//设置后能成功激活进程窗口，但是无法将输入焦点转移到进程窗口

	::BringWindowToTop(hwnd);
	::SetFocus(hwnd);
}

void bbqshop::processJsonSaveLoginInfo(const Json::Value &value)
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
		connect(dlg, SIGNAL(closeThisDlg()), this, SLOT(closeHookNum()));
		connect(this, SIGNAL(manInputESC()), dlg, SLOT(closeSelf()));
		connect(this, SIGNAL(manInputEnter()), dlg, SLOT(ClickPay()));
		hookNum(true);
		dlg->show();
	}
}

void bbqshop::closeHookNum()
{
	hookNum(false);
	emit returnFocusToCashier();
}

inline void bbqshop::hookManInputNum(DWORD vkCode)
{
	
}

