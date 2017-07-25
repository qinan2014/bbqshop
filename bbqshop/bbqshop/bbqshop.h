#ifndef BBQSHOP_H
#define BBQSHOP_H

#ifndef _DEBUG
#define USEKEYHOOK
#define ENABLE_PRINTER
#endif

#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>
#include <QApplication>
#include "json/json.h"
#include "PayCodeNode.h"
#ifdef USEKEYHOOK
#include "KeyHook.h"
#else
#include <Windows.h>
#pragma comment(lib ,"user32.lib")
#endif
#include "BbqUrlServer.h"
#include "AccessServerResult.h"

class bbqshop : public QWidget, public AccessServerResult
{
	Q_OBJECT

public:
	enum TIMERINDEX
	{
		TIMER_GETPRICE,
	};

	bbqshop(QApplication *pApp, QWidget *parent = 0);
	~bbqshop();

	void ShowTipString(QString inTip, QString inTitle = QString::fromLocal8Bit("提示"));
	codeSetIO::ZHIHUISETTING &GetSetting();
	bool CreateGoodBillRequest(double inOriPrice, double inFavoPrice, int dlgTag);
	void SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag = -1);
	void GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag);
	void GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag = -1);
	std::string GetPayTool(int inType);
	bool IsImportentOperateNow();

private:
	QApplication *mainApp;
	//bool isShowingPayResult;
	codeSetIO::ZHIHUISETTING mZHSetting;
#ifdef USEKEYHOOK
	CKeyHook mKeyHook;
#endif
	BbqUrlServer *urlServer;
	//定时器
	std::map<int, int> timers;
	int getOCRPriceTimes;
	QString curTradeNo;
	bool isShowingHandoverDlg;

	void createTray();
	
	inline void parseProcessJsonData(QString inJson);
	inline void processJsonSaveLoginInfo(const Json::Value &value);
	inline void processJsonOnMainDlgClose(const Json::Value &value);
	void processJsonStartOCR();
	inline void processJsonShowMainDlg();
	inline void processJsonRereadSetting();
	inline void processJsonShowPrice(const Json::Value &value);
	inline void startHook();
	inline void stopHook();
	inline void hookManInputCodeMsg(MSG* msg);
	inline void hookScanCodeMsg(MSG* msg);
	inline void hookNum(bool isEnable);
	inline void hookESC(bool isEnable);
	inline void hookReturn(bool isEnable);
	inline void hookManInputNum(DWORD vkCode);
	inline void hookManInputShift(PKBDLLHOOKSTRUCT p);

	void showPayDialog();
	bool isPriceNum(QString &ioPriceStr);
	void startGetOCRPrice();
	void stopGetOCRPriceTimer();
	inline bool isHasTargetTimer(int targetTimer);
	inline void killTargetTimer(int targetTimer);
	void getOCRPrice();
	bool isOperatorOtherDlg();
	void SwipCardPayURLBack(const Json::Value &value, std::string urlApi);
	void tradeNoResult(const Json::Value & inData);

signals:
	void showTipStringSig(const QString &, const QString &);
	void returnFocusToCashier();
	void manInputEnter();
	void manInputESC();
	void checkPayResultSig();
	void netError(QString, int, int);

private slots:
	void programQuit();
	void showSettingDlg();
	void showTipStringSlot(const QString &inTip, const QString &inTitle);
	void onCloseTipWin();
	void setFocusOnCashier();
	void closeHookNum();
	void showLoginDialog();
	void sendCashInfo();
	void onESCEvent();
	void requestTradeInfoByNo();
	void saveCurrentTradeNo(QString tradeNo);
	void checkPayResultSlot();

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	void CurlError(std::string url, int res, int urlTag);
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void timerEvent(QTimerEvent * event);
};

#endif // BBQSHOP_H
