#ifndef HANDOVERDLG_H
#define HANDOVERDLG_H

#include <QDialog>
#include "ui_HandoverDlg.h"
#include "AccessServerResult.h"
#include "BbqUrlServer.h"
#include "json/json.h"

class HandoverDlg : public QDialog, public AccessServerResult
{
	Q_OBJECT

public:
	HandoverDlg(QWidget *parent = 0);
	~HandoverDlg();

protected:
	virtual bool DealWithJSONFrServer(std::string mRecvJsonStr, int urlTag, std::string urlApi);  // 返回值 结构是否正确
	virtual void CurlError(std::string url, int res, int urlTag);

private:
	Ui::HandoverDlg ui;
	BbqUrlServer *urlServer;
	QWidget *parWidget;

	inline void setJSONInfo(Json::Value &inData);
	void getStatics();
	void setTradeStatic(const Json::Value & inVal);
};

#endif // HANDOVERDLG_H
