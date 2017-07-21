#include "ScreenCatch.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include "zhfunclib.h"

ScreenCatch::ScreenCatch(QWidget *parent)
	: QDialog(parent)
{
	setWindowState(Qt::WindowFullScreen);

	isPressed = false;
	isMoved = false;
	rubberBand = NULL;
	_pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
}

ScreenCatch::~ScreenCatch()
{

}

void ScreenCatch::paintEvent(QPaintEvent *event)  
{  
	QPainter painter(this);  
	_pixmap = _pixmap.scaled(width(),height(),Qt::KeepAspectRatio);  

	//pixmap_没有 alpha通道 添加通道  
	QPixmap temp(_pixmap.size());  
	temp.fill(Qt::transparent);  

	QPainter p(&temp);  
	p.setCompositionMode(QPainter::CompositionMode_Source);  
	p.drawPixmap(0, 0, _pixmap);  
	p.setCompositionMode(QPainter::CompositionMode_DestinationIn);  
	//p.fillRect(temp.rect(), QColor(50, 50, 50, 100)); //把图片调 暗 以显示截图全屏  

	//水印
	painter.drawPixmap(0,0,temp);  
}  

void ScreenCatch::mousePressEvent(QMouseEvent *event)  
{  
	isPressed = true;
	//获取鼠标点
	pStart_ = event->pos();

	if (!rubberBand)
	{
		rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
	}
	rubberBand->setGeometry(QRect(pStart_,QSize()));
	rubberBand->show();
}  

void ScreenCatch::mouseMoveEvent(QMouseEvent *event)
{  
	if (rubberBand)
	{
		isMoved = true;
		rubberBand->setGeometry(QRect(pStart_, event->pos()).normalized());
	}
}

void ScreenCatch::mouseReleaseEvent(QMouseEvent *event)  
{  
	isPressed = false;
	if (rubberBand)
	{
		//获取橡皮筋框的终止坐标
		if (isMoved)
		{
			pEnd = event->pos();
			selectedRect = CoordinateCorrecte(pStart_, pEnd);
			isMoved = false;
		}
	}
}

// 坐标修正函数
QRect ScreenCatch::CoordinateCorrecte(QPoint startPoint, QPoint endPoint)
{
	QRect * rect;
	// 情况一：左上 至 右下
	if (startPoint.x()<endPoint.x() && startPoint.y()<endPoint.y())
	{
		rect = new QRect(startPoint.x(), startPoint.y(), endPoint.x()-startPoint.x(), endPoint.y()-startPoint.y());
	}

	// 情况二：右下 至 左上
	if (startPoint.x()>endPoint.x() && startPoint.y()>endPoint.y())
	{
		rect = new QRect(endPoint.x(), endPoint.y(), startPoint.x()-endPoint.x(), startPoint.y()-endPoint.y());
	}

	// 情况二：左下 至 右上
	if (startPoint.x()<endPoint.x() && startPoint.y()>endPoint.y())
	{
		rect = new QRect(startPoint.x(), endPoint.y(), endPoint.x()-startPoint.x(), startPoint.y()-endPoint.y());
	}

	// 情况四：右上 至 左下
	if (startPoint.x()>endPoint.x() && startPoint.y()<endPoint.y())
	{
		rect = new QRect(endPoint.x(), startPoint.y(), startPoint.x()-endPoint.x(), endPoint.y()-startPoint.y());
	}

	return * rect;
}

void ScreenCatch::mouseDoubleClickEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		rubberBand->hide();
		emit selectRectsig(selectedRect);
		// QScreen 对象
		//QScreen * screen = QGuiApplication::primaryScreen();
		//// 截图
		//QPixmap img = screen->grabWindow(0,selectedRect.x(),selectedRect.y(),selectedRect.width(),selectedRect.height());
		//QString fllpath = ZHFuncLib::GetWorkPath().c_str();
		//fllpath += "/test.png";
		//// 保存
		//img.save(fllpath);
		accept();
	}
}
