#include "BindShopDlg.h"
#include "maindlg.h"
#include "ProcessProtocal.h"
#include "ZhuiHuiMsg.h"
#include "zhfunclib.h"
#include <QMessageBox>

BindShopDlg::BindShopDlg(QWidget *parent)
	: QDialog(parent)
{
	parWidget = parent;
	ui.setupUi(this);

	connect(ui.btnCommit, SIGNAL(pressed()), this, SLOT(commitSlot()));
	connect(ui.btnBind, SIGNAL(pressed()), this, SLOT(bindSlot()));
}

BindShopDlg::~BindShopDlg()
{

}

void BindShopDlg::commitSlot()
{
	MainDlg *mDlg = (MainDlg *)parWidget;
	codeSetIO::ShopCashdeskInfo &shopInfo = mDlg->GetSetting().shopCashdestInfo;

	// 获得输入的门牌号
	Json::Value item;
	item["shopid"] = ui.shopNumTxt->text().toStdString();

	std::string itemVal = item.toStyledString();
	std::string::size_type rePos;
	while ((rePos = itemVal.find(" ")) != -1) {
		itemVal.replace(rePos, 1, "");
	}

	mDlg->GetDataFromServer("api/app/v1", SETTINGCOMMITAPI, itemVal, URL_SETTING_DLG_COMMIT);
}

void BindShopDlg::bindSlot()
{
	MainDlg *mDlg = (MainDlg *)parWidget;
	if (mCashNos.size() < 1)
	{
		showTipDialogOK(QMessageBox::Warning, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请先提交"));
		return;
	}
	//emit showBindTipSig(true);

	// shop id
	QString shopID = ui.shopNumTxt->text();
	Json::Value root;
	root["shop_id"] = shopID.toStdString();
	char str[25];
	int cashNo = mCashNos[ui.cboCashNo->currentIndex()];
	itoa(cashNo, str, 10);
	root["cashdesk_id"] = str;
	root["android_type"] = "1";
	char  mac[MACADDRLEN];  
	ZHFuncLib::NativeLog("", "SettingWidget::bindSlot", "a");
	mDlg->GetMAC(mac);
	root["android_no"] = mac;
	codeSetIO::ShopCashdeskInfo &deskInfo = mDlg->GetSetting().shopCashdestInfo;
	root["cashier_account"] = deskInfo.account;
	mDlg->GetDataFromServer1(URLCLOUND, SETTINGBINDAPI, "", root, URL_SETTING_DLG_BIND);
}

void BindShopDlg::onUrlBack(int urlType, const Json::Value &inVal)
{
	switch (urlType)
	{
	case URL_SETTING_DLG_COMMIT:
		urlbackOnCommit(inVal);
		break;
	case URL_SETTING_DLG_BIND:
		urlbackOnBind(inVal);
		break;
	default:
		break;
	}
}


inline void BindShopDlg::urlbackOnCommit(const Json::Value &inVal)
{
	MainDlg *mDlg = (MainDlg *)parWidget;
	bool isReturnSuc = mDlg->isReturnSuccessFromeServer(inVal);
	bool hasReturnMsg = inVal.isMember("return_msgs");
	if (isReturnSuc)
	{
		setCashInfo(inVal["data"]);
	}
	if (!hasReturnMsg)
	{
		Json::Value mValData;
		mValData[PRO_HEAD] = TO_SHOWTIP;
		mValData[PRO_TIPSTR] = (isReturnSuc ? QString::fromLocal8Bit("提交成功。").toStdString() : QString::fromLocal8Bit("提交失败。").toStdString());
		HWND hwnd = ::FindWindowW(NULL, FLOATWINTITLEW);
		ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
	}
}

inline void BindShopDlg::urlbackOnBind(const Json::Value &inVal)
{
	MainDlg *mDlg = (MainDlg *)parWidget;
	bool isReturnSuc = mDlg->isReturnSuccessFromeServer(inVal);
	std::string dcdev_mac = inVal["dcdev_mac"].asString();
	std::string dcdev_no = inVal["dcdev_no"].asString();
	if (isReturnSuc)
	{
		Json::Value mValData;
		mValData[PRO_HEAD] = TO_FLOATWIN_RECONNECT_SOCKET;
		HWND hwnd = ::FindWindowW(NULL, FLOATWINTITLEW);
		ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
	}
	else
	{
		ZHFuncLib::NativeLog("", inVal.toStyledString().c_str(), "a");
		return;
	}
	//CString tmpstr;
	//tmpstr.Format(L"dcdev mac: %s     ,dcdev no: %s ", pWnd->CharToWChar(dcdev_mac.c_str()), pWnd->CharToWChar(dcdev_no.c_str()));
	//ZHMainDlg::InitInstance(false)->SetStatus(tmpstr);

	codeSetIO::ShopCashdeskInfo &shopInfo = mDlg->GetSetting().shopCashdestInfo;
	const char *tmpChar = inVal["shop_code"].asCString();
	memcpy(shopInfo.shopCode, tmpChar, strlen(tmpChar));
	shopInfo.shopCode[strlen(tmpChar)] = 0;
	tmpChar = inVal["shop_name"].asCString();
	memcpy(shopInfo.shopName, tmpChar, strlen(tmpChar));
	shopInfo.shopName[strlen(tmpChar)] = 0;
	tmpChar = inVal["cashdesk_id"].asCString();
	memcpy(shopInfo.cashdeskId, tmpChar, strlen(tmpChar));
	shopInfo.cashdeskId[strlen(tmpChar)] = 0;
	tmpChar = inVal["cashdesk_name"].asCString();
	memcpy(shopInfo.cashdeskName, tmpChar, strlen(tmpChar));
	shopInfo.cashdeskName[strlen(tmpChar)] = 0;
	tmpChar = inVal["dcdev_no"].asCString();
	memcpy(shopInfo.dcdevNo, tmpChar, strlen(tmpChar));
	shopInfo.dcdevNo[strlen(tmpChar)] = 0;
	tmpChar = inVal["dcdev_mac"].asCString();
	memcpy(shopInfo.dcdevMac, tmpChar, strlen(tmpChar));
	shopInfo.dcdevMac[strlen(tmpChar)] = 0;
	shopInfo.shopid = ui.shopNumTxt->text().toInt();
	shopInfo.isBind = isReturnSuc;

	//ui.cashNoTxt->setText(shopInfo.cashdeskName); // maindlg里面
}


void BindShopDlg::setCashInfo(const Json::Value &inData)
{
	mCashNos.clear();
	mCashNames.clear();
	disconnect(ui.cboCashNo, SIGNAL(currentIndexChanged(int)), this, SLOT(cashNoChanged(int)));
	ui.cboCashNo->clear();
	connect(ui.cboCashNo, SIGNAL(currentIndexChanged(int)), this, SLOT(cashNoChanged(int)));

	const char *shopname = inData["SHOP_NAME"].asCString();
	//ui.shopNameTxt->setText(shopname); //maindlg里面

	Json::Value cashObj = inData["CASH_LIST"];
	int sz = cashObj.size();
	for (unsigned int i = 0; i < sz; i++)
	{
		Json::Value noInfo = cashObj[i];
		int id = noInfo["ID"].asInt();
		const char *cashNoName = noInfo["CASH_NAME"].asCString();
		mCashNos.push_back(id);
		mCashNames.push_back(cashNoName);
		char string[255];
		sprintf(string, "(%d)%s", id, cashNoName);
		ui.cboCashNo->addItem(string);
	}
	if (sz != 0)
	{
		ui.cboCashNo->setCurrentIndex(0);
		cashNoChanged(0);
	}
}

void BindShopDlg::cashNoChanged(int newIndex)
{
	MainDlg *mDlg = (MainDlg *)parWidget;
	codeSetIO::ShopCashdeskInfo &shopInfo = mDlg->GetSetting().shopCashdestInfo;
	if (newIndex < 0)
		return;
	sprintf(shopInfo.cashdeskId, "%d", mCashNos[newIndex]);
	memcpy(shopInfo.cashdeskName, mCashNames[newIndex].c_str(), mCashNames[newIndex].length());
}

void BindShopDlg::showTipDialogOK(int icon, const QString &inTitle, const QString &inTxt)
{
	QMessageBox box((QMessageBox::Icon)icon, inTitle, inTxt);
	box.setStandardButtons (QMessageBox::Ok);
	box.setButtonText (QMessageBox::Ok,QString::fromLocal8Bit("确 定"));
	box.setWindowFlags(Qt::WindowStaysOnTopHint);
	box.setWindowFlags(box.windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
	box.setDefaultButton(QMessageBox::Ok);

	QRect thisRect = this->geometry();
	int iWidth = thisRect.width();
	int iHeight = thisRect.height();
	QRect boxRect = box.geometry();
	int posx = x() + iWidth - boxRect.width() * 0.6;
	int posy = y() + iHeight - boxRect.height() * 0.6;
	box.setGeometry(posx, posy, box.width(), box.height());

	int typeBtn = box.exec();
}