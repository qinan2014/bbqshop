#ifndef PAYSUCCESSSHOWDLG_H
#define PAYSUCCESSSHOWDLG_H

#include <QDialog>
#include "ui_PaySuccessShowDlg.h"
#include "json/json.h"

class PaySuccessShowDlg : public QDialog
{
	Q_OBJECT

public:
	PaySuccessShowDlg(QString iconPath, QWidget *parent = 0);
	~PaySuccessShowDlg();

	void SetPaySuccessInfo(const char *trade_no, const char *pay_fee);
private:
	Ui::PaySuccessShowDlg ui;
	QString iconPath;
	QWidget *floatWidget;

	QString tradeNO;
	QString tradeTime;
	//QString tradeType;
	//QString tradeStatus;
	QString tradeMoney;

	void setClientIcon(const QString &clientIcon);

private slots:
	void showClientIcon(const QString &clientIconType);
	void showUsrClientInfoSlot(const QString &nickName, int payTimes, int isMember);
};

#endif // PAYSUCCESSSHOWDLG_H