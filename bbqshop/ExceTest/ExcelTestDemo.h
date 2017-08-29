#ifndef EXCELTESTDEMO_H
#define EXCELTESTDEMO_H

#include <QObject>
#include <QAxObject>

class ExcelTestDemo : public QObject
{
	Q_OBJECT

public:
	ExcelTestDemo(QString filePath, QObject *parent);
	~ExcelTestDemo();

private:
	QAxObject *applicationExcel;
};

#endif // EXCELTESTDEMO_H
