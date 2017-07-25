#include "PaySuccessShowDlg.h"
#include "zhfunclib.h"
#include <time.h> 
//#include "bbqpay.h"
#include <QDesktopWidget>
#include <QTimer>

PaySuccessShowDlg::PaySuccessShowDlg(QString iconPath, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->iconPath = iconPath;
	floatWidget = parent;
	//setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Popup);
	setWindowTitle(QString::fromLocal8Bit("支付成功"));

	//setStyleSheet("QDialog{border: 1px solid #112D4E;border-radius:0px}"); 
	QRect tmpRect = this->geometry();
	int iWidth = tmpRect.width();
	int iHeight = tmpRect.height();
	QDesktopWidget *desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	this->setGeometry((screen.width() - iWidth) * 0.5, (screen.height() - iHeight) * 0.5, iWidth, iHeight);

	// logo
	QString pngPath = ZHFuncLib::GetWorkPath().c_str();
	pngPath += iconPath;
	QPixmap img(pngPath);
	QSize imgsz = img.size();
	QSize parsz = this->size();
	int imgwidth = parsz.width() * 0.7;
	float widper = (float )imgwidth / (float )imgsz.width();
	int imgheight = imgsz.height() * widper;
	img = img.scaled(imgwidth, imgheight, Qt::KeepAspectRatio);
	ui.labIcon->setPixmap(img);
	int posx = (parsz.width() - imgwidth) * 0.5;
	int posy = 1;
	ui.labIcon->setGeometry(posx, posy, imgwidth, imgheight);

	pngPath = ZHFuncLib::GetWorkPath().c_str();
	pngPath += "/res/pc_btn_g.bmp";
	ui.pbtPrint->setStyleSheet("border-image: url(" + pngPath + ");color: #FFFFFF; font: 12pt \"Arial\";");
	connect(ui.pbtPrint, SIGNAL(released()), this, SLOT(accept()));

	setClientIcon("/res/clientDefaultIcon.png");

	//bbqpay *floatWin = (bbqpay *)floatWidget;
	//connect(floatWin, SIGNAL(toShowClientIcon(const QString &)), this, SLOT(showClientIcon(const QString &)));
	//connect(floatWin, SIGNAL(showUsrClientInfo(const QString &, int, int)), this, SLOT(showUsrClientInfoSlot(const QString &, int, int)));

	//if (floatWin->GetSetting().shopCashdestInfo.isAutoPrint == 1)
	//{
	//	QTimer::singleShot(2000,this, SLOT(accept()) ); 
	//}
}

PaySuccessShowDlg::~PaySuccessShowDlg()
{
}

void PaySuccessShowDlg::SetPaySuccessInfo(const Json::Value & inVal)
{
	Json::Value msg_info = inVal["msg_info"];
	const char *status_des = msg_info["status_des"].asCString();
	const char *trade_no = msg_info["trade_no"].asCString();
	const char *pay_type = msg_info["pay_type"].asCString();
	const char *favo_fee = msg_info["favo_fee"].asCString();
	const char *total_fee = msg_info["total_fee"].asCString();
	const char *pay_fee = msg_info["pay_fee"].asCString();
	const char *orig_fee = msg_info["orig_fee"].asCString();
	const char *pay_status = msg_info["pay_status"].asCString();
	const char *payType = msg_info["pay_type"].asCString();

	tradeNO = trade_no;
	time_t t = time(NULL);
	struct tm *local = localtime(&t);
	char tmpbuf[200];
	sprintf(tmpbuf, "%d-%d-%d %d:%d:%d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	tradeTime = tmpbuf;
	int iPaytype = atoi(payType);
	//bbqpay *floatWin = (bbqpay *)floatWidget;

	//tradeMoney = QString::fromLocal8Bit("￥");
	//tradeMoney += pay_fee;

	//ui.labTradeMoney->setText(pay_fee);
	//ui.labTradeNo->setText(tradeNO);
	//ui.labTradeTime->setText(tradeTime);
	//ui.labTradeValue->setText(tradeMoney);
}

void PaySuccessShowDlg::setClientIcon(const QString &clientIcon)
{
	QString pngPath = ZHFuncLib::GetWorkPath().c_str();
	pngPath += clientIcon;
	ZHFuncLib::NativeLog("", pngPath.toStdString().c_str(), "a");
	QPixmap img(pngPath);
	QSize imgsz = img.size();
	QSize labsz = ui.labCIcon->size();
	int imgwidth = labsz.width();
	float widper = (float )imgwidth / (float )imgsz.width();
	int imgheight = imgsz.height() * widper;
	img = img.scaled(imgwidth, imgheight, Qt::KeepAspectRatio);
	ui.labCIcon->setPixmap(img);
	int posx = 0;
	QSize cwidSz = ui.clientWidget->size();
	int posy = (cwidSz.height() - imgheight) * 0.5;
	ui.labCIcon->setGeometry(posx, posy, imgwidth, imgheight);
}

void PaySuccessShowDlg::showClientIcon(const QString &clientIconType)
{
	//QString iconName = "/";
	//iconName += CLIENT_ICON_NAME;
	//if (!clientIconType.isEmpty())
	//{
	//	iconName += ".";
	//	iconName += clientIconType;
	//}
	//setClientIcon(iconName);
}

void PaySuccessShowDlg::showUsrClientInfoSlot(const QString &nickName, int payTimes, int isMember)
{
	//ui.labClientNickName->setText(nickName);
	//ui.labPayTimes->setText(QString::number(payTimes) + QString::fromLocal8Bit("次"));
	//if (isMember == 1)
	//	ui.labMemberStatus->setText(QString::fromLocal8Bit("会员"));
	//else
	//	ui.labMemberStatus->setText(QString::fromLocal8Bit("非会员"));
}

