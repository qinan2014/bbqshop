#ifndef MAINDLG_H
#define MAINDLG_H

#include <QtWidgets/QDialog>
#include "ui_maindlg.h"
#include "PayCodeNode.h"
#include "json/json.h"
#include "BbqUrlServer.h"
#include "AccessServerResult.h"

#ifdef _DEBUG
#define INDEPENDENTLOGIN
#endif

class MainDlg : public QDialog, public AccessServerResult
{
	Q_OBJECT

public:
	MainDlg(QApplication *pApp, char *account = NULL, QWidget *parent = 0);
	~MainDlg();

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
	inline void indexIntoAscii(int tabNum, int *inIdex, int *outASCII);
	int getImageScaleTag(float &outScale);
	bool checkSoft();
	void setActualTimeGetPrice(bool isActualTime);
	void showTipDialogOK(int icon, const QString &inTitle, const QString &inTxt);
	void catchScreenInfo();
	inline void parseProcessJsonData(QString inJson);
	inline void saveLoginData(const Json::Value &inJson);
	inline void showPrice(const Json::Value &value);
	inline void urlbackOnCommit(const Json::Value &value);
	inline void urlbackOnBind(const Json::Value &value);
	bool isReturnSuccessFromeServer(const Json::Value &pjsonVal);
	void setCashInfo(const Json::Value &inData);
	void memeryPrintName();
	void SavePrintFont(int printerType, int printerFont);
	void SaveAllSetting();
	int GetPrinterDeviceWidth();

#ifdef INDEPENDENTLOGIN
	void login();
	void LoginInfoStore(const Json::Value &data);
#endif

protected:
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void hideEvent(QHideEvent * event);
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	virtual void showEvent(QShowEvent * event);

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
	void saveSetting();
	void checkCashSoftCorrect();
	void printerTest();
	void clickClear();
	void onSettingInfoFinished();

signals:
	void showBindTipSig(bool );
	void settingInfoFinished();
};

#endif // MAINDLG_H
