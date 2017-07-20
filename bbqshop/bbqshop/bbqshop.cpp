#include "bbqshop.h"
#include "AllWindowTitle.h"
#include <QTimer>
#include "zhfunclib.h"

bbqshop::bbqshop(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	setWindowTitle(FLOATWINTITLE);
	setGeometry(0, 0, 5, 5);
	// 定时器
	QTimer::singleShot(100,this, SLOT(hide()) );  // 隐藏自己

}

bbqshop::~bbqshop()
{

}
