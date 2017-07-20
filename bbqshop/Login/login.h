#ifndef LOGIN_H
#define LOGIN_H

#include <QtWidgets/QDialog>
#include "ui_login.h"
#include "BbqUrlServer.h"
#include "AccountNumber.h"
#include "AccessServerResult.h"
#include "json/json.h"

class Login : public QDialog, public AccessServerResult
{
	Q_OBJECT

public:
	Login(QWidget *parent = 0);
	~Login();

private:
	Ui::LoginClass ui;
	std::vector<AccountNumber::USERPWD *> mUsrs;
	BbqUrlServer *urlServer;
	bool isLogining;
	QWidget *parWidget;

	void initFrame();
	void getLoginInfo();
	void showTipString(const QString &inTip);
	void loginInfoStore(const Json::Value &value);

protected:
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void CurlError(std::string url, int res, int urlTag);

signals:
	void loginStatus(bool needShow);

private slots:
	void clickLogin();
	void onLoginStatus(bool isLogined);
};

#endif // LOGIN_H
