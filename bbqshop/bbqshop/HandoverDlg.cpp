#include "HandoverDlg.h"
#include <QDesktopWidget>
#include "bbqshop.h"

HandoverDlg::HandoverDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	parWidget = parent;
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
	// 访问url
	getStatics();
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
		if (urlTag == URL_TRADESTATIC_MAINWIDGET)
		{
			std::string retCode = value["return_code"].asString();
			std::string resCode = value["result_code"].asString();
			std::string retmsg = value["return_msgs"].asString();
			if (retCode == "FAIL" || resCode == "FAIL" || retCode == "fail" || resCode == "fail")
			{
				const char *msg = value["return_msgs"].asCString();
				bbqshop *parPay = (bbqshop *)parWidget;
				parPay->ShowTipString(msg);
			}else
			{
				Json::Value data = value["data"];
				setTradeStatic(data);
			}
		}
	}
	return suc;
}

void HandoverDlg::CurlError(std::string url, int res, int urlTag)
{
	bbqshop *mainDlg = (bbqshop *)parWidget;
	mainDlg->ShowTipString(QString::fromLocal8Bit("网络异常，请检查网络！"));
}

void HandoverDlg::getStatics()
{
	// 交易统计
	Json::Value mValData;
	setJSONInfo(mValData);
	std::string itemVal = mValData.toStyledString();
	std::string::size_type rePos;
	while ((rePos = itemVal.find(" ")) != -1) {
		itemVal.replace(rePos, 1, "");
	}
	urlServer->TimeFormatRecover(itemVal, mValData["logintime"].asCString());

	urlServer->GetDataFromServer("api/app/v1", TRADEINFOSTATICAPI, itemVal, URL_TRADESTATIC_MAINWIDGET);
}

inline void HandoverDlg::setJSONInfo(Json::Value &inData)
{
	bbqshop *mainDlg = (bbqshop *)parWidget;
	codeSetIO::ShopCashdeskInfo &shopInfo = mainDlg->GetSetting().shopCashdestInfo;
	inData["shopid"] = shopInfo.shopid;
	inData["id"] = shopInfo.id;
	inData["role"] = shopInfo.role;
	inData["shoptype"] = shopInfo.shoptype;
	inData["logintime"] = shopInfo.loginTime;

	ui.labName->setText(shopInfo.userName);
}

void HandoverDlg::setTradeStatic(const Json::Value & inData)
{
	bbqshop *mainDlg = (bbqshop *)parWidget;
	int orderNum = inData["ordernum"].asInt(); // 今日交易笔数
	double payNum = inData["paynum"].asDouble(); // 今日交易总价格
	int ordernumback = inData["ordernumback"].asInt(); // 今日退款笔数
	double payNumBack = inData["paynumback"].asDouble(); // 退款总金额
	int alipayNum = inData["aliPayTotal"].asInt(); // 今日支付宝交易笔数
	double alipayVal = inData["aliPayTotalFee"].asDouble(); // 今日支付宝交易金额
	int balanceNum = inData["balancePayTotal"].asInt(); // 今日储值卡交易笔数
	double balanceVal = inData["balancePayTotalFee"].asDouble(); // 今日储值卡交易金额
	int wxpayNum = inData["wxPayTotal"].asInt(); // 今日微信交易笔数
	double wxpayVal = inData["wxPayTotalFee"].asDouble(); // 今日微信交易金额
	int jdpayNum = inData["jdPayTotal"].asInt(); // 今日京东交易笔数
	double jdpayVal = inData["jdPayTotalFee"].asDouble(); // 今日京东交易金额

	ui.labStaticTradeVal->setText(QString::number(payNum) + QString::fromLocal8Bit(" / 元"));
	ui.labStaticTradeTms->setText(QString::number(orderNum) + QString::fromLocal8Bit(" / 笔"));
	ui.labStaticRefundVal->setText(QString::number(payNumBack) + QString::fromLocal8Bit(" / 元"));
	ui.labStaticRefundTms->setText(QString::number(ordernumback) + QString::fromLocal8Bit(" / 笔"));
}
