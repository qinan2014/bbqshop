#include "bbqshop.h"
#include "AllWindowTitle.h"
#include <QTimer>
#include "zhfunclib.h"
#include <QMenu>
#include "AllExeName.h"
#include <QProcess>

bbqshop::bbqshop(QApplication *pApp, QWidget *parent)
	: QWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	setWindowTitle(FLOATWINTITLE);
	setGeometry(0, 0, 5, 5);
	// 定时器
	QTimer::singleShot(100,this, SLOT(hide()) );  // 隐藏自己
	// 创建托盘
	createTray();
	// 登录
	showLoginDialog();
}

bbqshop::~bbqshop()
{

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
	//codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	//args << deskInfo.account;
	//args << QString::number(curTab);

	process->start(program, args);
}