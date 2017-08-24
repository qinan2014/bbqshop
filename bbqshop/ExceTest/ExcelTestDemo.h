#ifndef EXCELTESTDEMO_H
#define EXCELTESTDEMO_H

#include <QObject>

class ExcelTestDemo : public QObject
{
	Q_OBJECT

public:
	ExcelTestDemo(QString filePath, QObject *parent);
	~ExcelTestDemo();

private:
	
};

#endif // EXCELTESTDEMO_H
