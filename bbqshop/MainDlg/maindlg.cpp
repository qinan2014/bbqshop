#include "maindlg.h"
#include "AllWindowTitle.h"
#include "TipExtendBtn.h"

MainDlg::MainDlg(QApplication *pApp, char *account, QWidget *parent)
	: QDialog(parent), parWidget(parent), mainApp(pApp)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	ui.setupUi(this);
	setWindowTitle(MAINDLGTITLE);

	setStyleSheet("QDialog{background-color:#F9F7F7;border: 1px solid #112D4E;border-width:1px;border-radius:2px}"); 
	setTopBtn();
}

MainDlg::~MainDlg()
{

}

inline void MainDlg::setTopBtn()
{
	QRect parRect = this->geometry();
	int xpos = parRect.width() - 30;
	int ypos = 5;

	TipExtendBtn *closeBtn = new TipExtendBtn(this, "/res/close.png", "/res/closered.png");
	closeBtn->setGeometry(QRect(xpos, ypos, 25, 25));
	closeBtn->setToolTip(QString::fromLocal8Bit("关闭"));

	// 关闭
	connect(closeBtn, SIGNAL(pressed()), this, SLOT(closeMainDlg()));
}

void MainDlg::closeMainDlg()
{
	hide();
	mainApp->quit();
}