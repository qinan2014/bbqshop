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
	QAxObject *workSheets = workBook->querySubObject("WorkSheets");
	//QAxObject *curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");

}

ExcelTestDemo::~ExcelTestDemo()
{
	if (applicationExcel != NULL)
	{
		applicationExcel->dynamicCall("Quit()");
		applicationExcel = NULL;
	}
}
