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
	insertMyData(workSheets);

	//QAxObject *curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");
	//curSheet->setProperty("Name", QString::fromLocal8Bit("服务安排"));
	//QAxObject *curCell = curSheet->querySubObject("Cells(int,int)", 1, 1);
	//curCell->dynamicCall("SetValue(const QString&)", QString::fromLocal8Bit("时间  人员"));
	//设置单元格内容，并合并单元格（第5行第3列-第8行第5列）
	//QAxObject *cell_5_6 = curSheet->querySubObject("Cells(int,int)", 5, 3);
	//cell_5_6->setProperty("Value", "Java");  //设置单元格值
	//QString merge_cell;
	//merge_cell.append(QChar(3 - 1 + 'A'));  //初始列
	//merge_cell.append(QString::number(5));  //初始行
	//merge_cell.append(":");
	//merge_cell.append(QChar(5 - 1 + 'A'));  //终止列
	//merge_cell.append(QString::number(8));  //终止行
	//QAxObject *merge_range = curSheet->querySubObject("Range(const QString&)", merge_cell);
	//merge_range->setProperty("HorizontalAlignment", -4108);
	//merge_range->setProperty("VerticalAlignment", -4108);
	//merge_range->setProperty("WrapText", true);
	//merge_range->setProperty("MergeCells", true);  //合并单元格

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

void ExcelFunCls::mergeCells(QAxObject *workSheet, int beginX, int beginY, int endX, int endY, QString inVal)
{
	QAxObject *cell_5_6 = workSheet->querySubObject("Cells(int,int)", beginY, beginX);
	cell_5_6->setProperty("Value", inVal);  //设置单元格值
	QString merge_cell;
	merge_cell.append(QChar(beginX - 1 + 'A'));  //初始列
	merge_cell.append(QString::number(beginY));  //初始行
	merge_cell.append(":");
	merge_cell.append(QChar(endX - 1 + 'A'));  //终止列
	merge_cell.append(QString::number(endY));  //终止行
	QAxObject *merge_range = workSheet->querySubObject("Range(const QString&)", merge_cell);
	merge_range->setProperty("HorizontalAlignment", -4108);
	merge_range->setProperty("VerticalAlignment", -4108);
	merge_range->setProperty("WrapText", true);
	merge_range->setProperty("MergeCells", true);  //合并单元格
}

void ExcelFunCls::insertMyData(QAxObject *workSheets)
{
	QAxObject *curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");
	curSheet->setProperty("Name", QString::fromLocal8Bit("服务安排"));
	mergeCells(curSheet, 1, 1, 1, 2, QString::fromLocal8Bit("时间  人员"));
	// 服务时间的插入
	QString yearMonth = "2017-9-";
	QString fourAclockTime = QString::fromLocal8Bit(" : 4点");
	QString sixAclockTime = QString::fromLocal8Bit(" : 6点");
	QString oriService = QString::fromLocal8Bit("  原先安排  ");
	QString realService = QString::fromLocal8Bit("  实际服务  ");
	int theFirstSundy = 3;
	int beginX = 2;
	const int cellWidth = 10;
	for (int i = 0; i < 4; ++i)
	{
		mergeCells(curSheet, beginX, 1, beginX + 1, 1, yearMonth + QString::number(theFirstSundy) + fourAclockTime);
		insertCell(curSheet, beginX, 2, cellWidth, 0, oriService);
		insertCell(curSheet, beginX + 1, 2, cellWidth, 0, realService);
		mergeCells(curSheet, beginX+2, 1, beginX + 3, 1, yearMonth + QString::number(theFirstSundy) + sixAclockTime);
		insertCell(curSheet, beginX + 2, 2, cellWidth, 0, oriService);
		insertCell(curSheet, beginX + 3, 2, cellWidth, 0, realService);
		beginX += 4;
		theFirstSundy += 7;
	}
}

void ExcelFunCls::insertCell(QAxObject *workSheet, int posx, int posy, int inW, int inH, QString inVal)
{
	QAxObject *cell1 = workSheet->querySubObject("Cells(int,int)", posy, posx);
	cell1 ->setProperty("Value", inVal);  //设置单元格值
	if (inW > 0)
		cell1->setProperty("ColumnWidth", inW);  //设置单元格列宽
	if (inH > 0)
		cell1->setProperty("RowHeight", 50);  //设置单元格行高
}