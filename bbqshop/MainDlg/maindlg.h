#ifndef MAINDLG_H
#define MAINDLG_H

#include <QtWidgets/QDialog>
#include "ui_maindlg.h"
#include "PayCodeNode.h"
#include "json/json.h"
#include "BbqUrlServer.h"
#include "AccessServerResult.h"

class MainDlg : public QDialog, public AccessServerResult
{
	Q_OBJECT

public:
	MainDlg(QApplication *pApp, char *account = NULL, QWidget *parent = 0);
	~MainDlg();

	//void SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag = -1);
	//void GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag = -1);
	//void GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag = -1);
	//void GetMAC(char *mac);
	//void TimeFormatRecover(std::string &outStr, std::string inOriTimeStr);
	//std::string GetPayTool(int inType);
	//bool IsImportentOperateNow();

	std::vector<QString > mWinClassNames;
	std::vector<QString > mWinWindowNames;

private:
	Ui::MainDlgClass ui;
	QWidget *parWidget;
	QApplication *mainApp;
	codeSetIO::ZHIHUISETTING mZHSetting;
	BbqUrlServer *urlServer;
	std::vector<int > mCashNos;
	std::vector<std::string >mCashNames;

	inline void setTopBtn();
	inline void initFrame();
	inline void asciiIntoIndex(QStringList &ioHotkeyLs, int tabNum, int *inASCII, int *outIndex);
	int getImageScaleTag(float &outScale);
	bool checkSoft();
	void setActualTimeGetPrice(bool isActualTime);
	void showTipDialogOK(int icon, const QString &inTitle, const QString &inTxt);
	void catchScreenInfo();
	inline void parseProcessJsonData(QString inJson);
	inline void showPrice(const Json::Value &value);
	inline void urlbackOnCommit(const Json::Value &value);
	inline void urlbackOnBind(const Json::Value &value);
	bool isReturnSuccessFromeServer(const Json::Value &pjsonVal);
	void setCashInfo(const Json::Value &inData);

protected:
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void hideEvent(QHideEvent * event);
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);

private slots:
	void closeMainDlg();
	void cashToolChanged(int newIndex);
	void printerChanged(int newIndex);
	void catchScreen();
	void catchScreen(const QRect &inselect);
	void checkPrice();
	void commitSlot();
	void cashNoChanged(int newIndex);
	void bindSlot();
	void showTipSlot(bool isShow);

signals:
	void showBindTipSig(bool );
};

#endif // MAINDLG_H
