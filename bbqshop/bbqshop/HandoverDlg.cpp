#include "HandoverDlg.h"
#include <QDesktopWidget>

HandoverDlg::HandoverDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	urlServer = new BbqUrlServer(this);
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
}

HandoverDlg::~HandoverDlg()
{

}

bool HandoverDlg::DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi)
{
	Json::Reader reader;
	Json::Value value;
	bool suc = reader.parse(mRecvJsonStr, value);
	if (suc)
	{
		//if (urlTag == URL_LOGIN_DLG)
		//{
		//	std::string retCode = value["return_code"].asString();
		//	std::string resCode = value["result_code"].asString();
		//	std::string retmsg = value["return_msgs"].asString();
		//	if (retCode == "FAIL" || resCode == "FAIL" || retCode == "fail" || resCode == "fail")
		//	{
		//		const char *msg = value["return_msgs"].asCString();
		//		bbqpay *parPay = (bbqpay *)parWidget;
		//		parPay->ShowTipString(msg);
		//	}else
		//	{
		//		Json::Value data = value["data"];
		//		bbqpay *parPay = (bbqpay *)parWidget;
		//		parPay->LoginInfoStore(data);

		//		emit loginStatus(true);
		//	}
		//}
	}
	return suc;
}

void HandoverDlg::CurlError(std::string url, int res, int urlTag)
{
	//ui.pbtLogin->setText(QString::fromLocal8Bit("登录"));
	//isLogining = false;
	//bbqpay *parPay = (bbqpay *)parWidget;
	//parPay->ShowTipString(QString::fromLocal8Bit("网络异常，请检查网络！"));
}
