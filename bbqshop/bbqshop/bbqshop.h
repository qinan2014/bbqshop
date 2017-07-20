#ifndef BBQSHOP_H
#define BBQSHOP_H

#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>
#include <QApplication>
#include "json/json.h"
#include "PayCodeNode.h"

class bbqshop : public QWidget
{
	Q_OBJECT

public:
	bbqshop(QApplication *pApp, QWidget *parent = 0);
	~bbqshop();

	void ShowTipString(QString inTip, QString inTitle = QString::fromLocal8Bit("ÌבÊ¾"));
private:
	QApplication *mainApp;
	bool isShowingPayResult;
	codeSetIO::ZHIHUISETTING mZHSetting;

	void createTray();
	void showLoginDialog();
	void parseProcessJsonData(QString inJson);
	void processJsonSaveLoginInfo(const Json::Value &value);

signals:
	void showTipStringSig(const QString &, const QString &);
	void returnFocusToCashier();

private slots:
	void programQuit();
	void showTipStringSlot(const QString &inTip, const QString &inTitle);
	void onCloseTipWin();
	void setFocusOnCashier();

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);
	
};

#endif // BBQSHOP_H
