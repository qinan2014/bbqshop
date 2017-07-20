#include "PayDialog.h"
#include <Windows.h>
#include "zhfunclib.h"
#include "URLDefine.h"
#include <QDesktopWidget>
#include "bbqshop.h"
#include <QMessageBox>

PayDialog* PayDialog::instance = NULL;

PayDialog::PayDialog(QString imgPath, QWidget *parent)
	: QDialog(parent), parWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	pngPath = imgPath;
	isSelfClose = true;

	// 位置
	QRect parRect = this->geometry();
	int iWidth = parRect.width();
	int iHeight = parRect.height();
	QDesktopWidget *desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	int screenWidth = screen.width();
	int screenHeight = screen.height();
	int posx = (screenWidth - iWidth) * 0.5;
	int posy = (screenHeight - iHeight) * 0.5;
	this->setGeometry(posx, posy, iWidth, iHeight);
	isqr = false;
	
	// 金额
	QRegExp rx("^([0-9]{0,5}\.[0-9]{1,2})$");
	QValidator *validator = new QRegExpValidator(rx,0);
	ui.ledtPrice->setValidator(validator);
	connect(ui.ledtPrice, SIGNAL(textChanged(const QString &)), this, SLOT(moneyChanged(const QString &)));

	setWindowTitle(QString::fromLocal8Bit("支付"));

	QString tmppngPath = ZHFuncLib::GetWorkPath().c_str();
	tmppngPath += "/res/pc_btn_g.bmp";
	ui.pbtPay->setStyleSheet("border-image: url(" + tmppngPath + ");color: #FFFFFF;font: 12pt \"Arial\";");

	setStyleSheet("QDialog{background-color: #F0F0F0}");

	// 支付按钮
	connect(ui.pbtPay, SIGNAL(released()), this, SLOT(ClickPay()));
	connect(this, SIGNAL(enablePaySig(bool )), this, SLOT(EnablePay(bool )));
	//bbqpay *parWid = (bbqpay *)parWidget;
	//connect(this, SIGNAL(closeThisDlg()), parWid, SLOT(toShowFloat()));
}

PayDialog::~PayDialog()
{
	//ZHFuncLib::NativeLog("", "delete payDialog", "a");
	instance = NULL;
	emit closeThisDlg();
}

PayDialog *PayDialog::InitInstance(bool mustCreate, QWidget *parent, QString imagePath)
{
	if (instance == NULL && mustCreate)
	{
		instance = new PayDialog(imagePath, parent);
		//instance->exec();
		instance->setModal(true);
	}
	if (instance != NULL)
		::SetWindowPos((HWND)instance->winId(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	return instance;
}

void PayDialog::CloseInstance(bool selfclose)
{
	if (instance != NULL)
	{
		instance->isSelfClose = selfclose;
		instance->close();
		delete instance;
		instance = NULL;
	}
}

void PayDialog::SetScanCode(QString inCode)
{
	ui.ledtScancode->setText(inCode);
}

void PayDialog::SetMoney(QString inMoney)
{
	if (!inMoney.isEmpty())
	{
		if (inMoney.length() == 1 && inMoney.at(0) == '0')
			inMoney = "0.00";
		ui.ledtPrice->setText(inMoney);
	}
}

void PayDialog::moneyChanged(const QString &newMoney)
{
	QString tmpPrice = newMoney;
	double moneyVal = tmpPrice.toDouble();
	if (moneyVal > MONEY_MAX_COUNT)
	{
		tmpPrice = QString::number(MONEY_MAX_COUNT);
		ui.ledtPrice->setText(tmpPrice);
	}
	ui.labMoney->setText(tmpPrice);
	int fontsz = ui.labMoney->fontMetrics().width(tmpPrice);
	QSize moneysz = ui.labMoney->size();
	QRect moneyPos = ui.labMoney->frameGeometry();
	QRect moneyTagOriPos = ui.labMoneyTag->frameGeometry();
	ui.labMoneyTag->move(moneyPos.x() + (moneyPos.width() - fontsz) - 20, moneyTagOriPos.y());
}

void PayDialog::ToQRFrame()
{
	isqr = true;
	QRect dlgRect = this->geometry();
	int ipWidth = dlgRect.width() * 1.2;
	int iHeight = dlgRect.height() * 0.8;
	QDesktopWidget *desktop = QApplication::desktop();
	QRect screen = desktop->screenGeometry();
	int screenWidth = screen.width();
	int screenHeight = screen.height();
	int posx = (screenWidth - ipWidth) * 0.5;
	int posy = (screenHeight - iHeight) * 0.5;
	this->setGeometry(posx, posy, ipWidth, iHeight);

	ui.payBtnWidget->hide();
	ui.labPayTip->hide();
	// 金额位置移动
	QRect moneyOriRect = ui.moneyWid->frameGeometry();
	int iWidth = moneyOriRect.width();
	iHeight = moneyOriRect.height();
	posx = (ipWidth - moneyOriRect.width()) * 0.5;
	posy = moneyOriRect.y() + 55;
	ui.moneyWid->setGeometry(posx, posy, iWidth, iHeight);

	QLabel *tipLab = new QLabel(this);
	tipLab->setText(QString::fromLocal8Bit("出示给客户使用微信/支付宝扫码支付"));
	tipLab->setAlignment(Qt::AlignHCenter);
	iWidth += 50;
	posx = (ipWidth - iWidth) * 0.5;
	tipLab->setGeometry(posx, posy + 50, iWidth, iHeight);
	tipLab->setStyleSheet("font: bold 16px \"Arial Black\";");

	setWindowTitle(QString::fromLocal8Bit("支付二维码"));
}

void PayDialog::resizeEvent(QResizeEvent *event)
{
	QSize parsz = event->size();
	// 图标
	std::string workPath = ZHFuncLib::GetWorkPath();
	QString qworkPath = workPath.c_str();
	qworkPath += pngPath;
	QPixmap img(qworkPath);
	QSize imgsz = img.size();
	if (!isqr)
	{
		ui.labIcon->setPixmap(img);
		int posx = (parsz.width() - imgsz.width()) * 0.5;
		int posy = 70;
		ui.labIcon->setGeometry(posx, posy, imgsz.width(), imgsz.height());
	}
	else
	{
		int imgwidth = parsz.width() * 0.7;
		float widper = (float )imgwidth / (float )imgsz.width();
		int imgheight = imgsz.height() * widper;
		img = img.scaled(imgwidth, imgheight, Qt::KeepAspectRatio);
		ui.labIcon->setPixmap(img);
		int posx = (parsz.width() - imgwidth) * 0.5;
		int posy = 10;
		ui.labIcon->setGeometry(posx, posy, imgwidth, imgheight);
	}
}

void PayDialog::closeEvent(QCloseEvent * e)
{
	if (isSelfClose)
	{
		//bbqpay *parWid = (bbqpay *)parWidget;
		//parWid->toShowFloat();
	}
}

void PayDialog::ClickPay()
{
	//bbqpay *parWid = (bbqpay *)parWidget;
	//if (parWid->execingTipDlg)
	//	return;
	//QString pricestr = ui.ledtPrice->text();
	//codeSetIO::HOTKEYS &hotKeys = parWid->GetSetting().hotKeys;
	//if (pricestr.isEmpty())
	//{
	//	if (hotKeys.hTradeInfo.qtkey != -1)
	//		parWid->ShowTipString(QString::fromLocal8Bit("价格不能为空"));
	//	else
	//		parWid->ShowTipDialogOK(QMessageBox::Warning, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("价格不能为空"), this);
	//	return;
	//}
	//if (ui.ledtScancode->text().isEmpty())
	//{
	//	if (hotKeys.hTradeInfo.qtkey != -1)
	//		parWid->ShowTipString(QString::fromLocal8Bit("支付码不能为空"));
	//	else
	//		parWid->ShowTipDialogOK(QMessageBox::Warning, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("支付码不能为空"), this);
	//	return;
	//}

	//if (parWid->IsImportentOperateNow())
	//	return;

	//bool requestSuc = parWid->CreateGoodBillRequest(pricestr.toDouble(), 0.00, URL_SWIP_CARD_DLG);
	//if (requestSuc)
	//	emit enablePaySig(false);
}

void PayDialog::EnablePay(bool enablePay)
{
	QString statyDes;
	if (enablePay)
		statyDes = QString::fromLocal8Bit("确定(Enter)");
	else
		statyDes = QString::fromLocal8Bit("正在支付");
	isPaying = !enablePay;
	ui.pbtPay->setText(statyDes);
	ui.pbtPay->setEnabled(enablePay);
}

void PayDialog::CreatePayBillSucess(bool isSuc, const Json::Value &inVal)
{
	//bbqpay *parWid = (bbqpay *)parWidget;
	//parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, inVal.toStyledString().c_str());
	if (!isSuc)
	{
		emit enablePaySig(true);
		return;
	}
	const char *tradeNo = inVal["trade_no"].asCString();
	mcurtradeNo = tradeNo;
	Json::Value root;
	root["trade_no"] = tradeNo;  
	std::string tmp = ui.ledtScancode->text().toStdString();

	root["dynamic_id"] = tmp;

	//parWid->GetDataFromServer1(URLTRADE, CARDPAYMICROPAYAPI, "", root, URL_SWIP_CARD_DLG);
	//parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, root.toStyledString().c_str());
}

void PayDialog::CardPayInfo(bool isPaySuc, const Json::Value &inVal)
{
	//bbqpay *parWid = (bbqpay *)parWidget;
	//parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, inVal.toStyledString().c_str());
	if (!isPaySuc)
	{
		EnablePay(TRUE);
		return;
	}

	std::string tmplog = "card pay success";
	//parWid->LogError(tmplog.c_str(), "a");
	accept();
}

void PayDialog::CardPayQueryResult(const Json::Value & pjsonVal)
{
	std::string retCode = pjsonVal["return_code"].asString();
	std::string resCode = pjsonVal["result_code"].asString();
	const char *msg = pjsonVal["return_msgs"].asCString();
}
