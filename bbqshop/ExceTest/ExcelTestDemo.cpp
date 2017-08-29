#include "ExcelTestDemo.h"
#include <QAxObject>

ExcelTestDemo::ExcelTestDemo(QString filePath, QObject *parent)
	: QObject(parent)
{
	applicationExcel = new QAxObject("Excel.Application", parent);
	applicationExcel->setProperty("Visible", false); //false不显示窗体
	QAxObject *work_books = applicationExcel->querySubObject("WorkBooks"); 
	work_books->dynamicCall("Open (const QString&)", filePath);
	//QVariant title_value = applicationExcel->property("Caption");  //获取标题
	QAxObject *workBook = applicationExcel->querySubObject("ActiveWorkBook");
	//QAxObject *workSheets = workBook->querySubObject("WorkSheets");
	//QAxObject *curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");
	QAxObject *worksheet = workBook->querySubObject("Worksheets(int)", 1);  
	QAxObject *cell1 = worksheet->querySubObject("Cells(int,int)", 1, 1 ); 
	QString val1 = cell1->property("Value").toString();//property是返回QVariant结构体
	bool issuc = cell1 ->setProperty("Value", QString::fromLocal8Bit("时间2人员"));  //设置单元格值
}

ExcelTestDemo::~ExcelTestDemo()
{
	if (applicationExcel != NULL)
	{
		applicationExcel->dynamicCall("Quit()");
		applicationExcel = NULL;
	}
}
