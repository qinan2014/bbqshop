#include "maindlg.h"
#include <QtWidgets/QApplication>
#include "zhfunclib.h"
#include "AllExeName.h"

int main(int argc, char *argv[])
{
	// 关闭除本进程外的其他进程
	ZHFuncLib::TerminateProcessExceptCurrentOne(MAINDLGEXE);
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	MainDlg w(&a);
	w.show();
	return a.exec();
}
