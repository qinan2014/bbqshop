
#include <QtCore/QCoreApplication>
#include "zhfunclib.h"
#include "AllExeName.h"
#include <QProcess>

int main(int argc, char *argv[])
{
	ZHFuncLib::TerminateProcessExceptCurrentOne(RELOGINEXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(MAINDLGEXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(OCREXE);
	ZHFuncLib::TerminateProcessExceptCurrentOne(BBQSHOPEXE);

	QCoreApplication a(argc, argv);
	QString program = ZHFuncLib::GetWorkPath().c_str();
	program += "/";
	program += BBQSHOPEXE;
	QProcess *process = new QProcess();
	QStringList args;
	process->start(program, args);
	//a.quit();

	//return a.exec();
	return 1;
}
