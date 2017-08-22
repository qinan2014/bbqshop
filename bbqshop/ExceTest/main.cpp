#include "excetest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ExceTest w;
	w.show();
	return a.exec();
}
