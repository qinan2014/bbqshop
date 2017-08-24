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
};

#endif // EXCELFUNCLS_H
