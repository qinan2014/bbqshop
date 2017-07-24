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
	bool isShowingPayResult;
	codeSetIO::ZHIHUISETTING mZHSetting;
#ifdef USEKEYHOOK
	CKeyHook mKeyHook;
#endif
	BbqUrlServer *urlServer;

	void createTray();
	
	inline void parseProcessJsonData(QString inJson);
	inline void processJsonSaveLoginInfo(const Json::Value &value);
	inline void processJsonOnMainDlgClose(const Json::Value &value);
	inline void processJsonStartOCR();
	inline void processJsonShowMainDlg();
	inline void processJsonRereadSetting();
	inline void processJsonShowPrice(const Json::Value &value);
	inline void startHook();
	inline void stopHook();
	inline void hookManInputCodeMsg(MSG* msg);
	inline void hookScanCodeMsg(MSG* msg);
	inline void hookNum(bool isEnable);
	inline void hookManInputNum(DWORD vkCode);

	void showPayDialog();
	bool isPriceNum(QString &ioPriceStr);

signals:
	void showTipStringSig(const QString &, const QString &);
	void returnFocusToCashier();
	void manInputEnter();
	void manInputESC();

private slots:
	void programQuit();
	void showSettingDlg();
	void showTipStringSlot(const QString &inTip, const QString &inTitle);
	void onCloseTipWin();
	void setFocusOnCashier();
	void closeHookNum();
	void showLoginDialog();
	void sendCashInfo();

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	void CurlError(std::string url, int res, int urlTag);
};

#endif // BBQSHOP_H
