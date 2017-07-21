#ifndef TIPEXTENDBTN_H
#define TIPEXTENDBTN_H

#include <QPushButton>

class TipExtendBtn : public QPushButton
{
	Q_OBJECT

public:
	TipExtendBtn(QWidget *parent, const QString &normalIcon, const QString &hoverIcon);
	~TipExtendBtn();

protected:
	virtual void leaveEvent(QEvent *event);
	virtual void enterEvent(QEvent *event);

private:
	QString thisNormalIcon, thisHoverIcon;
};

#endif // TIPEXTENDBTN_H
