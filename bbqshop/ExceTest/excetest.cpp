#include "excetest.h"
#include <QFileDialog>
#include <QAxObject>

ExceTest::ExceTest(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//applicationExcel = new QAxObject("Excel.Application", parent);
	//workBooks = applicationExcel->querySubObject("Workbooks");
	//QFile file("D:\\test2.xls");
	//if (file.exists())
	//{
	//	workBooks->dynamicCall("Open(const QString&)", "D:\\test2.xls");
	//	workBook = applicationExcel->querySubObject("ActiveWorkBook");
	//	workSheets = workBook->querySubObject("WorkSheets");
	//}
	//else
	//{
	//	if(createExcel(applicationExcel, "D:\\test2.xls"))
	//	{
	//		workBooks->dynamicCall("Open(const QString&)", "D:\\test2.xls");
	//		workBook = applicationExcel->querySubObject("ActiveWorkBook");
	//		workSheets = workBook->querySubObject("WorkSheets");
	//	}
	//}



	connect(ui.pbtSelect, SIGNAL(released()), this, SLOT(onFileSelect()));
}

ExceTest::~ExceTest()
{
	applicationExcel->dynamicCall("Quit()");
}

void ExceTest::onFileSelect()
{
	QString filepath=QFileDialog::getSaveFileName(this,tr("Save orbit"),".",tr("Microsoft Office 2007 (*.xls)"));//获取保存路径
	if(filepath.isEmpty())
	{
		return;
	}
	filepath.replace(QString("/"), QString("\\"));
	applicationExcel = new QAxObject("Excel.Application", this);
	QAxObject *workBooks = applicationExcel->querySubObject("Workbooks");
	createExcel(applicationExcel, filepath);
	workBooks->dynamicCall("Open(const QString&)", filepath);
	QAxObject *workBook = applicationExcel->querySubObject("ActiveWorkBook");
	workSheets = workBook->querySubObject("WorkSheets");

	// 开始工作
	curSheet = workSheets->querySubObject("Item(const QString&)", "Sheet1");
	curSheet->setProperty("Name", QString::fromLocal8Bit("服务安排"));
	QAxObject *curCell = curSheet->querySubObject("Cells(int,int)", 1, 1);
	curCell->dynamicCall("SetValue(const QString&)", "test");
	workBook->dynamicCall("Save()");
}

bool ExceTest::createExcel(QAxObject *excelAX, QString file)
{
	QDir  dTemp;

	if(dTemp.exists(file))
	{
		//qDebug()<<" QExcel::CreateExcel: exist file"<<file;
		return false;
	}

	//qDebug()<<" QExcel::CreateExcel: succes";

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