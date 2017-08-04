#ifndef DETOURINJECTION_H
#define DETOURINJECTION_H

#include <QtWidgets/QWidget>
#include "ui_detourinjection.h"

class DetourInjection : public QWidget
{
	Q_OBJECT

public:
	DetourInjection(QWidget *parent = 0);
	~DetourInjection();

private:
	Ui::DetourInjectionClass ui;
};

#endif // DETOURINJECTION_H
