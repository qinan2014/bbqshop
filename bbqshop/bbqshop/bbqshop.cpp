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
	//if (strlen(mZHSetting.shopCashdestInfo.account) == 0)
	//	ZHFuncLib::NativeLog("", "mZHSetting.shopCashdestInfo.account == NULL", "a");
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
	case ZHIHUI_CODE_MSG:
		hookCodeMsg(msg);
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

	case TO_SHOWPAYDLG:
		//setPayInfo(value);
		break;
	case TO_SHOWTIP_NETERROR:
		{
			QString tipstr = QString::fromLocal8Bit("网络异常，请检查网络！");
			QString tiptitle = QString::fromLocal8Bit("警告");
			//showTipStringSlot(tipstr, tiptitle);
		}
		break;
	case TO_FLOATWIN_REWRITESETTING:
		{
			//ZHSettingRW settingRW(mZHSetting);
			//settingRW.ReadZHSetting();
			////char tmpbuf[100];
			////sprintf(tmpbuf, "isgetprice in time, %d", mZHSetting.shopCashdestInfo.isGetPriceActualTime);
			////LogError(tmpbuf, "a");
			//if (mZHSetting.shopCashdestInfo.isGetPriceActualTime == 1)
			//{
			//	std::vector<int > ids;
			//	ZHFuncLib::GetTargetProcessIds(OCREXE, ids);
			//	if (ids.size() > 0)
			//		sendcashinfo();
			//	else
			//		startocr(); // 启动ocr
			//}
			//else
			//	ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
#ifdef USEKEYHOOK
			if (mZHSetting.shopCashdestInfo.isUsePayGun == 1)
			{
				mKeyHook.StopHook();
				mKeyHook.StartHook((HWND)this->winId());
			}
			else
			{
				mKeyHook.StopHook();
			}
#endif
			ShowTipString(QString::fromLocal8Bit("保存完成！"));
		}
		break;
	case TO_FLOATWIN_STARTOCR:
		//killTargetTimer(TIMER_GETPRICE);
		//onlyStartOCR();
		break;
	case RETURN_PRICE:
		//showPrice(value);
		break;
	case TO_FLOAT_INSTALL:
		//installNewVersion(value);
		break;
	case TO_FLOATWIN_HANDOVERPRINT:
		//handoverPrint();
		break;
	case TO_FLOATWIN_HANDOVER:
		//handover();
		break;
	case TO_FLOATWIN_CURTAB:
		//changeCurrentTab(value);
		break;
	case TO_FLOATWIN_CHECKUPGRADE:
		//CheckUpgrade(URL_UPGRDE_DLG_CHECKBTN);
		break;
	case TO_SHOW_CLIENT_ICON:
		//isDownloadingClientIcon = false;
		//emit toShowClientIcon(value[PRO_CLIENT_ICON_TYPE].asCString());
		break;
	case TO_FLOATWIN_RECONNECT_SOCKET:
		//{
		//	killTargetTimer(TIMER_SOCKET_INIT);
		//	int tmtag = startTimer(1000, Qt::VeryCoarseTimer);
		//	timers[tmtag] = TIMER_SOCKET_INIT;
		//	char tmpchar[100];
		//	sprintf(tmpchar, "reconnect socket startTimer tmtag: %d", tmtag);
		//	ZHFuncLib::NativeLog("", tmpchar, "a");
		//}
		break;
	}
}

inline void bbqshop::hookCodeMsg(MSG* msg)
{
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)msg->lParam;
	switch (p->vkCode)
	{
	case HOOK_KEY_WX:
		showPayDialog();
		break;
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
	//if (PayDialog::InitInstance(false) != NULL)
	//	return;
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

void bbqshop::showPayDialog()
{

};