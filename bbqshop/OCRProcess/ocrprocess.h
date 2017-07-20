#ifndef OCRPROCESS_H
#define OCRPROCESS_H

#include <QtWidgets/QWidget>
#include <QTimerEvent>
#include "PayCodeNode.h"
#include "json/json.h"

#ifndef _DEBUG
#define _OCR_START_
#endif
#ifdef _OCR_START_
#include <tesseract/baseapi.h>
#include <tesseract/strngs.h>
#include <leptonica/allheaders.h>
#endif

class OCRProcess : public QWidget
{
	Q_OBJECT

public:
	OCRProcess(QWidget *parent = 0);
	~OCRProcess();

protected:
	virtual bool nativeEvent(const QByteArray & eventType, void * message, long * result);

private:
	std::wstring cashSoftName;
	codeSetIO::SelectRange mImageSel;
	int sourceHand;
#ifdef _OCR_START_
	tesseract::TessBaseAPI *g_ocrLib;
#endif
	QString GetOCRString(const char *picPath);
	QString GetPrice(int &outError);
	void parseProcessJsonData(const QString &strMessage);
	void saveCashInfo(const Json::Value &value);
	void sendBackPrice();
	float getImageScaleFactor();
};

#endif // OCRPROCESS_H
