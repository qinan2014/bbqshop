#ifndef EXCETEST_H
#define EXCETEST_H

#include <QtWidgets/QWidget>
#include "ui_excetest.h"
#include <QAxObject>

class ExceTest : public QWidget
{
	Q_OBJECT

public:
	ExceTest(QWidget *parent = 0);
	~ExceTest();

private:
	Ui::ExceTestClass ui;
	QString targetDir;
	QAxObject *applicationExcel;
	QAxObject *workSheets;
	QAxObject *curSheet;

	bool createExcel(QAxObject *excelAX, QString file);

private slots:
	void onFileSelect();
};

#endif // EXCETEST_H
