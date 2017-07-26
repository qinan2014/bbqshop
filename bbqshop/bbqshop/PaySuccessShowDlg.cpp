#include "PaySuccessShowDlg.h"
#include "zhfunclib.h"
#include <time.h> 
#include "bbqshop.h"
#include <QDesktopWidget>
#include <QTimer>

PaySuccessShowDlg::PaySuccessShowDlg(QString iconPath, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->iconPath = iconPath;
	floatWidget = parent;
	//setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Popup);
	setWindowTitle(QString::fromLocal8Bit("Ö§¸¶³É¹¦"));

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

	bbqshop *floatWin = (bbqshop *)floatWidget;
	if (floatWin->GetSetting().shopCashdestInfo.isAutoPrint == 1)
	{
		QTimer::singleShot(2000, this, SLOT(accept())); 
	}
}

PaySuccessShowDlg::~PaySuccessShowDlg()
{
}

void PaySuccessShowDlg::SetPaySuccessInfo(const char *trade_no, const char *pay_fee)
{
	tradeNO = trade_no;
	time_t t = time(NULL);
	struct tm *local = localtime(&t);
	char tmpbuf[200];
	sprintf(tmpbuf, "%d-%d-%d %d:%d:%d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
	tradeTime = tmpbuf;
	tradeMoney = QString::fromLocal8Bit("£¤");
	tradeMoney += pay_fee;

	ui.labTradeMoney->setText(pay_fee);
	ui.labTradeNo->setText(tradeNO);
	ui.labTradeTime->setText(tradeTime);
	ui.labTradeValue->setText(tradeMoney);
}
