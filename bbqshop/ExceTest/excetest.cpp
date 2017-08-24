#include "excetest.h"
#include <QFileDialog>
#include <QAxObject>
#include "ExcelFunCls.h"
#include "ExcelTestDemo.h"

ExceTest::ExceTest(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.pbtSelect, SIGNAL(released()), this, SLOT(onFileSelect()));
	connect(ui.pbtDemo, SIGNAL(released()), this, SLOT(onDemo()));
}

ExceTest::~ExceTest()
{

}

void ExceTest::onFileSelect()
{
	QString filepath=QFileDialog::getSaveFileName(this,tr("Save orbit"),".",tr("Microsoft Office 2007 (*.xls)"));//获取保存路径
	if(filepath.isEmpty())
		return;
	ExcelFunCls(filepath, this);
}

void ExceTest::onDemo()
{
	QString filepath=QFileDialog::getSaveFileName(this,tr("Save orbit"),".",tr("Microsoft Office 2007 (*.xls)"));//获取保存路径
	if(filepath.isEmpty())
		return;
	ExcelTestDemo(filepath, this);
}