#include "PayDialog.h"
#include <Windows.h>
#include "zhfunclib.h"
#include "URLDefine.h"
#include <QDesktopWidget>
#include "bbqshop.h"
#include <QMessageBox>
#include <QSignalMapper>

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
	
	setWindowTitle(QString::fromLocal8Bit("支付"));

	QString tmppngPath = ZHFuncLib::GetWorkPath().c_str();
	tmppngPath += "/res/pc_btn_g.bmp";
	ui.pbtPay->setStyleSheet("border-image: url(" + tmppngPath + ");color: #FFFFFF;font: 12pt \"Arial\";");

	setStyleSheet("QDialog{background-color: #F0F0F0}");

	//ui.ledtPrice->setFocus();
	// 支付按钮
	connect(ui.pbtPay, SIGNAL(released()), this, SLOT(ClickPay()));
	connect(this, SIGNAL(enablePaySig(bool )), this, SLOT(EnablePay(bool )));
	hasPayed = false;

	// 商铺名称
	bbqshop *parWid = (bbqshop *)parWidget;
	codeSetIO::ShopCashdeskInfo &shopInfo = parWid->GetSetting().shopCashdestInfo;
	ui.shopNameTxt->setText((shopInfo.shopName));
	ui.cashNoTxt->setText(shopInfo.cashdeskName);
	// 数字键盘
	addNumBtn();
}

PayDialog::~PayDialog()
{
	instance = NULL;
	emit closeThisDlg(hasPayed);
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
	{
		::SetWindowPos((HWND)instance->winId(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		//instance->hasPayed = false;
	}
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

void PayDialog::closeSelf()
{
	CloseInstance();
}

void PayDialog::onNetError(QString url, int res, int urlTag)
{
	if (urlTag == URL_SWIP_CARD_DLG)
	{
		if (!mcurtradeNo.isEmpty())
		{
			emit micropaySucess(mcurtradeNo);
			emit accepted();		
		}
	}
}

void PayDialog::SetScanCode(QString inCode)
{
	if (!inCode.isEmpty())
		ui.ledtScancode->setText(inCode);
}

void PayDialog::SetMoney(QString inMoney)
{
	if (!inMoney.isEmpty())
	{
		if (inMoney.length() == 1 && inMoney.at(0) == '0')
			inMoney = "0.00";
		//ui.labMoney->setText(inMoney);
		moneyChanged(inMoney);
	}
}

void PayDialog::moneyChanged(const QString &newMoney)
{
	QString tmpPrice = newMoney;
	double moneyVal = tmpPrice.toDouble();
	if (moneyVal > MONEY_MAX_COUNT)
	{
		tmpPrice = QString::number(MONEY_MAX_COUNT);
		//ui.ledtPrice->setText(tmpPrice);
	}
	ui.labMoney->setText(tmpPrice);
	int fontsz = ui.labMoney->fontMetrics().width(tmpPrice);
	QSize moneysz = ui.labMoney->size();
	QRect moneyPos = ui.labMoney->frameGeometry();
	QRect moneyTagOriPos = ui.labMoneyTag->frameGeometry();
	ui.labMoneyTag->move(moneyPos.x() + (moneyPos.width() - fontsz) - 20, moneyTagOriPos.y());
}

//void PayDialog::resizeEvent(QResizeEvent *event)
//{
//	QSize parsz = event->size();
//	// 图标
//	std::string workPath = ZHFuncLib::GetWorkPath();
//	QString qworkPath = workPath.c_str();
//	qworkPath += pngPath;
//	QPixmap img(qworkPath);
//	QSize imgsz = img.size();
//	ui.labIcon->setPixmap(img);
//	int posx = (parsz.width() - imgsz.width()) * 0.5;
//	int posy = 70;
//	ui.labIcon->setGeometry(posx, posy, imgsz.width(), imgsz.height());
//}

void PayDialog::ClickPay()
{
	if (!ui.pbtPay->isEnabled())
		return;
	bbqshop *parWid = (bbqshop *)parWidget;
	QString pricestr = ui.labMoney->text();
	if (pricestr.isEmpty())
	{
		parWid->ShowTipString(QString::fromLocal8Bit("价格不能为空"));
		return;
	}
	if (ui.ledtScancode->text().isEmpty())
	{
		parWid->ShowTipString(QString::fromLocal8Bit("支付码不能为空"));
		return;
	}

	if (parWid->IsImportentOperateNow())
		return;
	hasPayed = true;
	bool requestSuc = parWid->CreateGoodBillRequest(pricestr.toDouble(), 0.00, URL_SWIP_CARD_DLG);
	if (requestSuc)
		emit enablePaySig(false);
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
	bbqshop *parWid = (bbqshop *)parWidget;
	parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, inVal.toStyledString().c_str());
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

	parWid->GetDataFromServer1(URLTRADE, CARDPAYMICROPAYAPI, "", root, URL_SWIP_CARD_DLG);
	parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, root.toStyledString().c_str());
}

void PayDialog::CardPayInfo(bool isPaySuc, const Json::Value &inVal)
{
	bbqshop *parWid = (bbqshop *)parWidget;
	parWid->SendToURLRecord(LOG_DEBUG, LOG_PAY1, inVal.toStyledString().c_str());
	if (!isPaySuc)
	{
		EnablePay(TRUE);
		return;
	}
	emit micropaySucess(mcurtradeNo);

	accept();
}

void PayDialog::CardPayQueryResult(const Json::Value & pjsonVal)
{
	std::string retCode = pjsonVal["return_code"].asString();
	std::string resCode = pjsonVal["result_code"].asString();
	const char *msg = pjsonVal["return_msgs"].asCString();
}


void PayDialog::addNumBtn()
{
	// 槽函数
	QSignalMapper *signalMapper = new QSignalMapper(this);
	connect(signalMapper, SIGNAL(mapped(int )), this, SLOT(clickNumBtn(int )));

	// 4行三列按钮
	QRect parRect = ui.widgetNum->geometry();
	const char txtName[12][10] = {"1","2","3", "4", "5", "6", "7", "8", "9", ".", "0", "<-"};
	int txtIndex = 0;
#define USERDATAINDEX 0
#define NUMBTNWIDTH 70
#define NUMBTNHEIGHT 40
	// 每个按钮占有的宽度和高度
	int perNumBtnWidth = parRect.width() / 3;
	int perNumBtnHeight = parRect.height() / 4;
	int xposori = (perNumBtnWidth - NUMBTNWIDTH) * 0.5;
	int yposoir = (perNumBtnHeight - NUMBTNHEIGHT) * 0.5;
	int xpos = xposori, ypos = yposoir;
	for (int i = 0; i < 4; ++i)
	{

		for (int j = 0; j < 3; ++j)
		{
			QPushButton *numBtn = new QPushButton(ui.widgetNum);
			numBtn->setText(QString::fromLocal8Bit(txtName[txtIndex]));
			numBtn->setGeometry(xpos, ypos, NUMBTNWIDTH, NUMBTNHEIGHT);
			QString sheet = "background-color: #3F72AF;border-style: outset; border-width: 1px; border-radius: 5px;";
			sheet += "border-color: #77787b; font: bold 18px; color: #F9F7F7";
			//sheet += "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #BB000000);"; min-width: 5em;
			numBtn->setStyleSheet(sheet);
			connect(numBtn, SIGNAL(clicked()), signalMapper, SLOT(map()));  
			signalMapper->setMapping(numBtn, txtIndex);
			++txtIndex;
			xpos += perNumBtnWidth;
		}
		xpos = xposori;
		ypos += perNumBtnHeight;
	}
#undef NUMBTNWIDTH
#undef NUMBTNHEIGHT
#undef USERDATAINDEX
}


void PayDialog::clickNumBtn(int inNum)
{
#define ADDNUM(inNumstr) \
	{ \
	if (pointPos != -1) \
	{   \
	if (moneyStr == "0.00")\
	moneyStr = ""; \
	if (moneyStr.length() - pointPos > 2) \
	return;  \
} \
	moneyStr += inNumstr; \
	numchanged = true;  \
}  \

	QString moneyStr = ui.labMoney->text();
	// 小数点后两位拦截住
	int pointPos = moneyStr.indexOf('.');
	bool numchanged = false;
	switch (inNum)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		ADDNUM(QString::number(inNum + 1));
		break;
	case 10:
		ADDNUM('0');
		break;
	case 9:
		if (pointPos == -1)
		{
			numchanged = true;
			moneyStr += '.';
		}
		break;
	case 11:
		moneyStr = moneyStr.remove(moneyStr.length() - 1, 1);
		//ui.edtMoney->setText(moneyStr);
		moneyChanged(moneyStr);
		break;
	}

	if (numchanged)
	{
		// 如果超出50000直接删掉最后一位即可
		double moneyVal = moneyStr.toDouble();
		if (moneyVal > MONEY_MAX_COUNT)
		{
			moneyStr = moneyStr.remove(moneyStr.length() - 1, 1);
			//ui.edtMoney->setText(moneyStr);
		}
		//ui.edtMoney->setText(moneyStr);
		moneyChanged(moneyStr);
	}
}

void PayDialog::ClickKeyNum(unsigned long inKey)
{
	switch (inKey)
	{
	case 48:
		clickNumBtn(10);
		break;
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
		clickNumBtn(inKey - 49);
		break;
	case '.':
	case VK_OEM_PERIOD:
		clickNumBtn(9);
		break;
	case VK_BACK:
		clickNumBtn(11);
		break;
	default:
		break;
	}
}