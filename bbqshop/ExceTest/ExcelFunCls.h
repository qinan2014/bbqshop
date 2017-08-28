#ifndef EXCELFUNCLS_H
#define EXCELFUNCLS_H

#include <QObject>
#include <QAxObject>

class ExcelFunCls : public QObject
{
	Q_OBJECT

public:
	ExcelFunCls(QString filePath, QObject *parent);
	~ExcelFunCls();

private:
	QAxObject *applicationExcel;

	bool createExcel(QAxObject *excelAX, QString file);
	void mergeCells(QAxObject *workSheet, int beginX, int beginY, int endX, int endY, QString inVal);
	void insertMyData(QAxObject *workSheets);
	void insertCell(QAxObject *workSheet, int posx, int posy, int inW, int inH, QString inVal);
};

#endif // EXCELFUNCLS_H
