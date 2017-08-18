#ifndef BINDSHOPDLG_H
#define BINDSHOPDLG_H

#include <QDialog>
#include "ui_BindShopDlg.h"
#include "json/json.h"

class BindShopDlg : public QDialog
{
	Q_OBJECT

public:
	BindShopDlg(QWidget *parent = 0);
	~BindShopDlg();

private:
	Ui::BindShopDlg ui;
	QWidget *parWidget;
	std::vector<int > mCashNos;
	std::vector<std::string >mCashNames;

	inline void urlbackOnCommit(const Json::Value &value);
	inline void urlbackOnBind(const Json::Value &value);

	void setCashInfo(const Json::Value &inData);
	void showTipDialogOK(int icon, const QString &inTitle, const QString &inTxt);

private slots:
	void commitSlot();
	void bindSlot();
	void onUrlBack(int urlType, const Json::Value &inVal);
	void cashNoChanged(int newIndex);
};

#endif // BINDSHOPDLG_H
