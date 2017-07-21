#ifndef SCREENCATCH_H
#define SCREENCATCH_H

#include <QDialog>
#include <QResizeEvent>
#include <QRubberBand>

class ScreenCatch : public QDialog
{
	Q_OBJECT

public:
	ScreenCatch(QWidget *parent);
	~ScreenCatch();

protected:  
	void paintEvent(QPaintEvent *event); 
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);
	
private:  
	QPixmap _pixmap;
	bool isPressed;
	bool isMoved;
	QPoint pStart_;
	QPoint pEnd;
	QRubberBand *rubberBand;
	QRect selectedRect;

	QRect CoordinateCorrecte(QPoint startPoint, QPoint endPoint);

signals:
	void selectRectsig(const QRect &);
};

#endif // SCREENCATCH_H
