#include "PaySuccessShowDlg.h"
#include "zhfunclib.h"
#include <time.h> 
#include "bbqshop.h"
#include <QDesktopWidget>
#include <QTimer>

PaySuccessShowDlg::PaySuccessShowDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	floatWidget = parent;
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	//setWindowTitle(QString::fromLocal8Bit("支付成功"));

	setStyleSheet("QDialog{border: 1px solid #112D4E;border-radius:0px}"); 
	QRect tmpRect = this->geometry();
	int iWidth = tmpRect.width();
	int iHeight = tmpRect.height();
	QDesktopWidget *desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	this->setGeometry((screen.width() - iWidth) * 0.5, (screen.height() - iHeight) * 0.5, iWidth, iHeight);

	payWait();
}

PaySuccessShowDlg::~PaySuccessShowDlg()
{
}

void PaySuccessShowDlg::SetPaySuccessInfo(const char *trade_no, const char *pay_fee)
{
	//tradeNO = trade_no;
	//time_t t = time(NULL);
	//struct tm *local = localtime(&t);
	//char tmpbuf[200];
	//sprintf(tmpbuf, "%d-%d-%d %d:%d:%d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	//tradeTime = tmpbuf;
	//tradeMoney = QString::fromLocal8Bit("￥");
	//tradeMoney += pay_fee;

	//ui.labTradeMoney->setText(pay_fee);
	//ui.labTradeNo->setText(tradeNO);
	//ui.labTradeTime->setText(tradeTime);
	//ui.labTradeValue->setText(tradeMoney);
}

void PaySuccessShowDlg::payWait()
{
	QRect parRect = this->geometry();
	ui.payStatus1->setText(QString::fromLocal8Bit("正在发起支付。。。"));
	ui.payStatus1->setStyleSheet("color: rgb(0, 0, 0);font: bold 17pt \"Arial\";");
	ui.payStatus2->hide();
	ui.payStatus3->hide();
	int posx = ui.payStatus1->x() + 20;
	ui.payStatus1->move(posx, parRect.height() * 0.4);
}

void PaySuccessShowDlg::paySuccess(const QString &inPrice)
{
	QRect parRect = this->geometry();
	ui.payStatus1->setText(QString::fromLocal8Bit("支付成功"));
	ui.payStatus1->setStyleSheet("color: rgb(90, 162, 0);font: bold 17pt \"Arial\";");
	ui.payStatus2->show();
	ui.payStatus2->setText(QString::fromLocal8Bit("支付金额：") + inPrice);
	ui.payStatus3->show();
	ui.payStatus1->move(0, 20);

	QTimer::singleShot(2000, this, SLOT(accept())); 
}

void PaySuccessShowDlg::payFailed()
{
	QRect parRect = this->geometry();
	ui.payStatus1->setText(QString::fromLocal8Bit("支付失败/超时"));
	ui.payStatus1->setStyleSheet("color: rgb(139, 0, 0);font: bold 17pt \"Arial\";");
	ui.payStatus3->hide();
	int posx = ui.payStatus1->x() - 10;
	ui.payStatus1->move(posx, parRect.height() * 0.25);

	ui.payStatus2->show();
	posx = ui.payStatus2->x();
	ui.payStatus2->setText(QString::fromLocal8Bit("请重新扫码支付"));
	ui.payStatus2->move(posx, parRect.height() * 0.55);

	QTimer::singleShot(2000, this, SLOT(accept())); 
}