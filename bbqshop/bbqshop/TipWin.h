#ifndef TIPWIN_H
#define TIPWIN_H

#include <QDialog>
#include "ui_TipWin.h"

class TipWin : public QDialog
{
	Q_OBJECT

public:
	TipWin(QString inTip, QString inTitle, QWidget *parent = 0);
	~TipWin();

private:
	Ui::TipWin ui;

signals:
	void closeTipWin();
};

#endif // TIPWIN_H
