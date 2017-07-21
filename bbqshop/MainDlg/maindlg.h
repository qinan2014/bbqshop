#ifndef MAINDLG_H
#define MAINDLG_H

#include <QtWidgets/QDialog>
#include "ui_maindlg.h"

class MainDlg : public QDialog
{
	Q_OBJECT

public:
	MainDlg(QApplication *pApp, char *account = NULL, QWidget *parent = 0);
	~MainDlg();

private:
	Ui::MainDlgClass ui;

	QWidget *parWidget;
	QApplication *mainApp;

	inline void setTopBtn();

public slots:
	void closeMainDlg();
};

#endif // MAINDLG_H
