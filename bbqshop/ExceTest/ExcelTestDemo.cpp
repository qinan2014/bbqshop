#include "ExcelTestDemo.h"
#include <QAxObject>

ExcelTestDemo::ExcelTestDemo(QString filePath, QObject *parent)
	: QObject(parent)
{
	QAxObject excel("Excel.Application"); 
	excel.setProperty("Visible", false); //false不显示窗体
	QAxObject *work_books = excel.querySubObject("WorkBooks"); 
	work_books->dynamicCall("Open (const QString&)", filePath);
	//QVariant title_value = excel.property("Caption");  //获取标题
}

ExcelTestDemo::~ExcelTestDemo()
{

}
