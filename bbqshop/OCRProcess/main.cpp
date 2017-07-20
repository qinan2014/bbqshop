#include "ocrprocess.h"
#include <QtWidgets/QApplication>
#include "zhfunclib.h"
#include "AllExeName.h"

int main(int argc, char *argv[])
{
	// 关闭除本进程外的其他进程
	ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);

	QApplication a(argc, argv);
	OCRProcess w;
	w.show();
	return a.exec();
}
