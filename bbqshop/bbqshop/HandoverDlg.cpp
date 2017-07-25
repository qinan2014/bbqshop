#include "HandoverDlg.h"

HandoverDlg::HandoverDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	urlServer = new BbqUrlServer(this);
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
	//ui.pbtLogin->setText(QString::fromLocal8Bit("µÇÂ¼"));
	//isLogining = false;
	//bbqpay *parPay = (bbqpay *)parWidget;
	//parPay->ShowTipString(QString::fromLocal8Bit("ÍøÂçÒì³££¬Çë¼ì²éÍøÂç£¡"));
}
