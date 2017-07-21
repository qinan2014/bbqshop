#include "maindlg.h"
#include "AllWindowTitle.h"
#include "TipExtendBtn.h"
#include "ZHSettingRW.h"

MainDlg::MainDlg(QApplication *pApp, char *account, QWidget *parent)
	: QDialog(parent), parWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	ui.setupUi(this);
	setWindowTitle(MAINDLGTITLE);

	setStyleSheet("QDialog{background-color:#F9F7F7;border: 1px solid #112D4E;border-width:1px;border-radius:2px}"); 
	setTopBtn();

	urlServer = new BbqUrlServer(this);
	// 保存Account
	codeSetIO::ShopCashdeskInfo &deskInfo = mZHSetting.shopCashdestInfo;
	memcpy(deskInfo.account, account, strlen(account));
	deskInfo.account[strlen(account)] = 0;

	ZHSettingRW settingRW(mZHSetting);
	settingRW.ReadZHSetting();
}

MainDlg::~MainDlg()
{

}

void MainDlg::SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag)
{
	urlServer->SendToURLRecord(logLevel, logModule, logMessage, urlTag);
}

void MainDlg::GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag)
{
	urlServer->GetDataFromServer(inSecondAddr, inApi, inData, urlTag);
}

void MainDlg::GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag)
{
	urlServer->GetDataFromServer1(inUrl, inSecondAddr, inApi, ioRootVal, urlTag);
}

void MainDlg::GetMAC(char *mac)
{
	urlServer->GetMAC(mac);
}

void MainDlg::TimeFormatRecover(std::string &outStr, std::string inOriTimeStr)
{
	urlServer->TimeFormatRecover(outStr, inOriTimeStr);
}

std::string MainDlg::GetPayTool(int inType)
{
	return urlServer->GetPayTool(inType);
}

bool MainDlg::IsImportentOperateNow()
{
	return urlServer->IsImportentOperateNow();
}

inline void MainDlg::setTopBtn()
{
	QRect parRect = this->geometry();
	int xpos = parRect.width() - 30;
	int ypos = 5;

	TipExtendBtn *closeBtn = new TipExtendBtn(this, "/res/close.png", "/res/closered.png");
	closeBtn->setGeometry(QRect(xpos, ypos, 25, 25));
	closeBtn->setToolTip(QString::fromLocal8Bit("关闭"));

	// 关闭
	connect(closeBtn, SIGNAL(pressed()), this, SLOT(closeMainDlg()));
}

void MainDlg::closeMainDlg()
{
	hide();
	mainApp->quit();
}

bool MainDlg::DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi)
{
	return true;
}