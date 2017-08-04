#include "detourinjection.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DetourInjection w;
	w.show();
	return a.exec();
}
