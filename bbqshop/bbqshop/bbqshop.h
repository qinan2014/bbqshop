#ifndef BBQSHOP_H
#define BBQSHOP_H

#include <QtWidgets/QWidget>
#include <QSystemTrayIcon>
#include <QApplication>

class bbqshop : public QWidget
{
	Q_OBJECT

public:
	bbqshop(QApplication *pApp, QWidget *parent = 0);
	~bbqshop();

private:
	QApplication *mainApp;

	void createTray();
	void showLoginDialog();

private slots:
	void programQuit();
	
};

#endif // BBQSHOP_H
