#include "ExcelFunCls.h"
#include <QDir>

ExcelFunCls::ExcelFunCls(QString filePath, QObject *parent)
	: QObject(parent)
{
	filePath.replace(QString("/"), QString("\\"));
	applicationExcel = new QAxObject("Excel.Application", parent);
	QAxObject *workBooks = applicationExcel->querySubObject("Workbooks");
	createExcel(applicationExcel, filePath);
	workBooks->dynamicCall("Open(const QString&)", filePath);
	QAxObject *workBook = applicationExcel->querySubObject("ActiveWorkBook");
	QAxObject *workSheets = workBook->querySubObject("WorkSheets");

	// 开始工作
	QAxObject *curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");
	curSheet->setProperty("Name", QString::fromLocal8Bit("服务安排"));
	QAxObject *curCell = curSheet->querySubObject("Cells(int,int)", 1, 1);
	curCell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("时间  人员"));
	workBook->dynamicCall("Save()");
}

ExcelFunCls::~ExcelFunCls()
{
	if (applicationExcel != NULL)
	{
		applicationExcel->dynamicCall("Quit()");
		applicationExcel = NULL;
	}
}

bool ExcelFunCls::createExcel(QAxObject *excelAX, QString file)
{
	QDir  dTemp;

	if(dTemp.exists(file))
	{
		return false;
	}
	/**< create new excel sheet file.*/
	QAxObject * workSheet = excelAX->querySubObject("WorkBooks");
	workSheet->dynamicCall("Add");

	/**< save Excel.*/
	QAxObject * workExcel= excelAX->querySubObject("ActiveWorkBook");
	excelAX->setProperty("DisplayAlerts", 1);
	workExcel->dynamicCall("SaveAs (const QString&,int,const QString&,const QString&,bool,bool)",file,56,QString(""),QString(""),false,false);
	excelAX->setProperty("DisplayAlerts", 1);
	workExcel->dynamicCall("Close (Boolean)", false);

	/**< exit Excel.*/
	//excel->dynamicCall("Quit (void)");

	return true;
}