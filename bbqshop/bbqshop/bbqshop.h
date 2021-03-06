﻿#ifndef BBQSHOP_H
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
		TIMER_MEMORYRECORD,
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
	int GetCommentFontSZ();
	int GetPrinterDeviceWidth();
	std::wstring StrToWStr(const char *inChar);

private:
	QApplication *mainApp;
	bool isShowingPayResult;
	codeSetIO::ZHIHUISETTING mZHSetting;
#ifdef USEKEYHOOK
	CKeyHook mKeyHook;
#endif
	BbqUrlServer *urlServer;
	//定时器
	std::map<int, int> timers;
	int getOCRPriceTimes;
	QString curTradeNo;
	QString curOutTradeno;
	bool isShowingHandoverDlg;
	bool isCtrlKeyDown;
	bool hasInjectDll;
	QString qaPrice;

	void createTray();
	inline void stopESCEvent();
	inline void resumeESCEvent();
	void messageLBtnClick();

	inline void parseProcessJsonData(QString inJson);
	inline void processJsonSaveLoginInfo(const Json::Value &value);
	inline void processJsonOnMainDlgClose(const Json::Value &value);
	void processJsonStartOCR();
	inline void processJsonShowMainDlg();
	inline void processJsonRereadSetting();
	inline void processJsonShowPrice(const Json::Value &value);
	inline void processJsonPayKeySet(const Json::Value &value);
	inline void processJsonOCRPrepared(const Json::Value &value);
	inline void startHook();
	inline void stopHook();
	inline bool isHooking(int hookKey);
	inline void hookManInputCodeMsg(MSG* msg);
	inline void onHookScanCodeMsg(MSG* msg);
	inline void hookManInputNum(bool isEnable);
	inline void hookScanCodeNum(bool isEnable);
	inline void hookESC(bool isEnable);
	inline void hookReturn(bool isEnable);
	inline void hookAttempToSetPayKey(bool isEnable);
	inline void onHookManInputNum(DWORD vkCode);
	inline void hookManInputShift(PKBDLLHOOKSTRUCT p);
	inline void hookManInputCtrl(PKBDLLHOOKSTRUCT p);
	void hookSettingPayKey(PKBDLLHOOKSTRUCT p);
	inline void setHookPayKeyValueFromZHSetting();
	inline void printPayResult(int pay_type, const char *trade_no, const char *orig_fee, const char *favo_fee, const char *pay_fee);
	void hookApiWriteFileData(void *inData, int dataLen, int dataType);
	inline QString comDataToPrice(void *inData, int dataLen);
	inline bool isPayCode(const QString &inCode);
	inline void clearRedundantLog();
	inline void getTimeByLogtxt(time_t &outTm, QString inLogTxt);

	void showPayDialog();
	bool isPriceNum(QString &ioPriceStr);
	void startGetOCRPrice();
	void stopGetOCRPriceTimer();
	inline bool isHasTargetTimer(int targetTimer);
	inline void killTargetTimer(int targetTimer);
	void getOCRPrice();
	bool isOperatorOtherDlg();
	void SwipCardPayURLBack(const Json::Value &value, std::string urlApi);
	
	void LogOut(int urlTag);
	void ExitFromServer(bool isHandover = false);
	void Handover();
	void PrintHandoverStatementRequest();
	void PrintHandoverStatement(const Json::Value & inVal);
	void setAutoRun(bool isAuto = true); // boot auto
	void createComFileMapping();

signals:
	void showTipStringSig(const QString &, const QString &);
	void returnFocusToCashier();
	void manInputEnter();
	void manInputESC();
	void checkPayResultSig();
	void netError(QString, int, int);
	void tradeResultSig(const Json::Value &);
	void paySuccesSig(const QString &);
	void payFailedSig();
	//void timedInjectDllSig();

private slots:
	void programQuit();
	void showSettingDlg();
	void showTipStringSlot(const QString &inTip, const QString &inTitle);
	void onCloseTipWin();
	void setFocusOnCashier();
	void onClosePayDlg(bool haspayed);
	void showLoginDialog();
	void sendCashInfo();
	void onESCEvent();
	void requestTradeInfoByNo();
	void saveCurrentTradeNo(QString tradeNo);
	void showPayResultDlg();
	void checkPayResultSlot();
	void tradeNoResult(const Json::Value & inVal);
	void autoInjectDll();
	//void timedInjectDll();
	//void onCreateBillSuccess();

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	void CurlError(std::string url, int res, int urlTag);
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void timerEvent(QTimerEvent * event);
};

#endif // BBQSHOP_H
