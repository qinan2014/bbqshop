#ifndef PAYSUCCESSSHOWDLG_H
#define PAYSUCCESSSHOWDLG_H

#include <QDialog>
#include "ui_PaySuccessShowDlg.h"
#include "json/json.h"

class PaySuccessShowDlg : public QDialog
{
	Q_OBJECT

public:
	PaySuccessShowDlg(QWidget *parent = 0);
	~PaySuccessShowDlg();

	void SetPaySuccessInfo(const char *trade_no, const char *pay_fee);
private:
	Ui::PaySuccessShowDlg ui;
	QWidget *floatWidget;
	
	void payWait();
public slots:
	void paySuccess(const QString &inPrice);
	void payFailed();
};

#endif // PAYSUCCESSSHOWDLG_H
