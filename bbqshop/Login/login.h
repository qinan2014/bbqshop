#ifndef LOGIN_H
#define LOGIN_H

#include <QtWidgets/QDialog>
#include "ui_login.h"

class Login : public QDialog
{
	Q_OBJECT

public:
	Login(QWidget *parent = 0);
	~Login();

private:
	Ui::LoginClass ui;
};

#endif // LOGIN_H
