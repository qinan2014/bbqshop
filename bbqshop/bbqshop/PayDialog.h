#ifndef PAYDIALOG_H
#define PAYDIALOG_H

#include <QDialog>
#include "ui_PayDialog.h"
#include <QResizeEvent>
#include "AccessServerResult.h"
#include "json/json.h"

class PayDialog : public QDialog
{
	Q_OBJECT

public:
	~PayDialog();
	static PayDialog *InitInstance(bool mustCreate, QWidget *parent = NULL, QString imagePath = "/res/scan_code.png");
	static void CloseInstance(bool selfclose = false);
	void SetScanCode(QString inCode);
	void SetMoney(QString inMoney);
	bool hasPayed;

//protected:
//	virtual void resizeEvent(QResizeEvent *event);

private:
	Ui::PayDialog ui;
	QString pngPath;
	QWidget *parWidget;
	bool isPaying;
	QString mcurtradeNo;
	bool isSelfClose;
	
	
	PayDialog(QString imgPath, QWidget *parent = 0);
	static PayDialog* instance;
	void moneyChanged(const QString &newMoney);
	void addNumBtn();

signals:
	void enablePaySig(bool enablePay);
	void closeThisDlg(bool );
	void micropaySucess(QString payTradeNo);

	private slots:
		
		void EnablePay(bool enablePay);

public slots:
	void ClickPay();
	void closeSelf();
	void onNetError(QString url, int res, int urlTag);

public:
	void CreatePayBillSucess(bool isReturnSuc, const Json::Value & value);
	void CardPayInfo(bool isReturnSuc, const Json::Value & value);
	//void QRPayInfo(bool isReturnSuc, const Json::Value & pjsonVal);
	void CardPayQueryResult(const Json::Value & pjsonVal);
};

#endif // PAYDIALOG_H
