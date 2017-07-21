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
#ifdef _DEBUG
	MainDlg w(&a, "13111111111");
#else
	MainDlg w(&a, argv[1]);
#endif
	w.show();
	return a.exec();
}
