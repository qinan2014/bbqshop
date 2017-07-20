#include "bbqshop.h"
#include <QtWidgets/QApplication>
#include "zhfunclib.h"
#include "AllExeName.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	bbqshop w;
	w.show();
	return a.exec();
}
