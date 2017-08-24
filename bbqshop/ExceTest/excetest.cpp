#include "excetest.h"
#include <QFileDialog>
#include <QAxObject>
#include "ExcelFunCls.h"

ExceTest::ExceTest(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.pbtSelect, SIGNAL(released()), this, SLOT(onFileSelect()));
}

ExceTest::~ExceTest()
{

}

void ExceTest::onFileSelect()
{
	QString filepath=QFileDialog::getSaveFileName(this,tr("Save orbit"),".",tr("Microsoft Office 2007 (*.xls)"));//获取保存路径
	if(filepath.isEmpty())
	{
		return;
	}

	ExcelFunCls(filepath, this);
}
