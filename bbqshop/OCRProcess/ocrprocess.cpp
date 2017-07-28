#include "ocrprocess.h"
#include <QTimer>
#include "zhfunclib.h"
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include "AllWindowTitle.h"
#include "ZhuiHuiMsg.h"
#include "ProcessProtocal.h"
#include "ZBase64.h"
#include <QFile>

OCRProcess::OCRProcess(QWidget *parent)
	: QWidget(parent)
{
	setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
	setWindowTitle(OCRDLGTITLE);
	sourceHand = -1;
	setGeometry(0, 0, 5, 5);
	// 定时器
	QTimer::singleShot(100,this, SLOT(hide()) );  // 隐藏自己
	ZHFuncLib::NativeLog("", "ocr process creat", "a");

#ifdef _OCR_START_
	//#define DIRPATH "D:/QinAn/CompanyProgram/DemoProject/OCRDemo/Win32/Release" 
#define DIRPATH NULL
	g_ocrLib = new tesseract::TessBaseAPI();
	int initRes = g_ocrLib->Init(DIRPATH, "eng", tesseract::OEM_DEFAULT);  // 返回0才算成功
	initRes = (initRes == 0 ? initRes : g_ocrLib->Init(ZHFuncLib::GetWorkPath().c_str(), "eng", tesseract::OEM_DEFAULT));
	if (initRes) {
		QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("OCR初始化错误"));
		return;
	}
#endif
}

OCRProcess::~OCRProcess()
{
#ifdef _OCR_START_
	if (g_ocrLib != NULL)
	{
		g_ocrLib->End();
		g_ocrLib = NULL;
	}
#endif
}

bool OCRProcess::nativeEvent(const QByteArray & eventType, void * message, long * result)
{
	MSG *param = static_cast<MSG *>(message);

	switch (param->message)
	{
	case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT*>(param->lParam);
			if (cds->dwData == ZHIHUI_CODE_MSG)
			{
				QString strMessage = QString::fromUtf8(reinterpret_cast<char*>(cds->lpData), cds->cbData);
				parseProcessJsonData(strMessage);
				*result = 1;
				return true;
			}
		}
	}

	return QWidget::nativeEvent(eventType, message, result);
}

void OCRProcess::parseProcessJsonData(const QString &inJson)
{
	Json::Reader reader;
	Json::Value value;
	bool suc = reader.parse(inJson.toStdString(), value);
	if (!suc)
		return;
	int header = value[PRO_HEAD].asInt();
	char tmpbuf[100];
	sprintf(tmpbuf, "ocr recv process json data %d", header);
	ZHFuncLib::NativeLog("ocr", tmpbuf, "a");
	switch (header)
	{
	case TO_OCR_CASHINFO:
		saveCashInfo(value);
		break;
	case TO_OCR_GETPRICE:
		sendBackPrice();
		break;
	}
}

QString OCRProcess::GetOCRString(const char *picPath)
{
	QString outStr("ocr init");

#ifdef _OCR_START_
	Pix *image = pixRead(picPath);
	g_ocrLib->SetImage(image);
	char *outText = g_ocrLib->GetUTF8Text();

	outStr = outText;

	pixDestroy(&image);
#endif
	return outStr;
}


QString OCRProcess::GetPrice(int &outError)
{
	QString priceStr("init");
	// 获得当下收银软件的位置
	if (cashSoftName.empty())
	{
		priceStr = "cash soft name is empty";
		outError = PRO_OCR_ERROR_NAME_EMPTY;
		return priceStr;
	}
	//const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(cashSoftName.utf16());   
	HWND hwnd = ::FindWindow(NULL, cashSoftName.c_str());
	if (hwnd == NULL) 
	{
		priceStr = "cash soft did not open";
		outError = PRO_OCR_ERROR_SOFT_NOTOPEN;
		return priceStr;
	}
	if (::IsWindowVisible(hwnd) == FALSE)
	{
		priceStr = "cash soft is not visible";
		outError = PRO_OCR_ERROR_SOFT_NOTVISIBLE;
		return priceStr;
	}
	if(::IsIconic(hwnd))
	{
		priceStr = "cash soft is mix";
		outError = PRO_OCR_ERROR_SOFT_MIX;
		return priceStr;
		//::ShowWindow(hprocess,SW_SHOWNORMAL);
	}
	if (mImageSel.widImage == 0 || mImageSel.heightImage == 0){
		outError = PRO_OCR_ERROR_SOFT_SELECTRANGEERROR;
		return "select range is not setted";
	}

	RECT cashierRect;
	::GetWindowRect(hwnd, &cashierRect);
	RECT priceRec;

	int centerPosx = 0, centerPosy = 0;
	switch (mImageSel.relitiveType)
	{
	case 0:
		centerPosx = cashierRect.left + mImageSel.xCenterDistance;
		centerPosy = cashierRect.top + mImageSel.yCenterDistance;
		break;
	case 1:
		centerPosx = cashierRect.right - mImageSel.xCenterDistance;
		centerPosy = cashierRect.bottom - mImageSel.yCenterDistance;
		break;
	case 2:
		centerPosx = cashierRect.left + mImageSel.xCenterDistance;
		centerPosy = cashierRect.bottom - mImageSel.yCenterDistance;
		break;
	case 3:
		centerPosx = cashierRect.right - mImageSel.xCenterDistance;
		centerPosy = cashierRect.bottom - mImageSel.yCenterDistance;
		break;
	case 4:
		centerPosx = (cashierRect.left + cashierRect.right) * 0.5 + mImageSel.xCenterDistance;
		centerPosy = (cashierRect.bottom + cashierRect.top) * 0.5 + mImageSel.yCenterDistance;
		break;
	default:
		break;
	}
	priceRec.left = centerPosx - mImageSel.widImage * 0.5;
	priceRec.top = centerPosy - mImageSel.heightImage * 0.5;
	priceRec.right = priceRec.left + mImageSel.widImage;
	priceRec.bottom = priceRec.top + mImageSel.heightImage;
	QString filePath = ZHFuncLib::GetWorkPath().c_str();
	filePath += "/img_price_zhi_hui.png";
	// 删除上一次保存下来的价格截图
	QFile::remove(filePath);

	// QScreen 对象
	QScreen * screen = QGuiApplication::primaryScreen();
	// 截图
	QPixmap img = screen->grabWindow(0, priceRec.left, priceRec.top, mImageSel.widImage, mImageSel.heightImage);
	if (mImageSel.imageBinaryzation > 0)
	{
		
		unsigned int nByte = 0;
		QImage ss = img.toImage();
		int nBytesPerPixel = ss.depth() / 8; 
		int nHeight = ss.height();
		int nWidth = ss.width();
		unsigned char *pPixelLine = NULL, cNewPixelValue = NULL;

		for (int i = 0; i < nHeight; ++i)
		{  
			pPixelLine = ss.scanLine(i);  
			if (pPixelLine == NULL)
				break;
			nByte = 0;  
			for (int j = 0; j < nWidth; ++j)
			{      
				cNewPixelValue = (BYTE)(0.11 * pPixelLine[nByte] + 0.59 * pPixelLine[nByte + 1] + 0.30 * pPixelLine[nByte + 2]);  
				pPixelLine[nByte] = pPixelLine[nByte + 1] = pPixelLine[nByte + 2] = cNewPixelValue;  
				nByte += nBytesPerPixel;  
			}    
		}
		img.convertFromImage(ss);
	}
	float imageScale = getImageScaleFactor();
	if (imageScale != IMAGE_SCALE_FACTOR0)
	{
		QSize imgsz = img.size();
		img = img.scaled(imgsz * imageScale);
	}

	// 保存
	img.save(filePath);
	std::string pstr = filePath.toStdString();
	priceStr = GetOCRString(pstr.c_str());
	outError = PRO_OCR_ERROR_SOFT_NOERROR;

	return priceStr;
}

float OCRProcess::getImageScaleFactor()
{
	switch (mImageSel.priceImageScaleTag)
	{
	case 0:  // 表示不放大
		return IMAGE_SCALE_FACTOR0;
	case 1:
		return IMAGE_SCALE_FACTOR1;
	case 2:
		return IMAGE_SCALE_FACTOR2;
	case 3:
		return IMAGE_SCALE_FACTOR3;
	case 4:
		return IMAGE_SCALE_FACTOR4;
		break;
	}
	return IMAGE_SCALE_FACTOR0;
}

void OCRProcess::saveCashInfo(const Json::Value &value)
{
	sourceHand = value[PRO_OCR_FROMDLG].asInt();
	
	std::string recvsoftname = value[PRO_OCR_CASHNAME].asString();
	ZBase64 base64;
	int recvOriLen = 0;
	std::string dname = base64.Decode(recvsoftname.c_str(), recvsoftname.length(), recvOriLen).c_str();

	ZHFuncLib::NativeLog("ocr", dname.c_str(), "a");

	cashSoftName = ZHFuncLib::StringToWstring(dname);
	mImageSel.relitiveType = value[PRO_OCR_REALITIVETYPE].asInt();
	mImageSel.xCenterDistance = value[PRO_OCR_SELX].asInt();
	mImageSel.yCenterDistance = value[PRO_OCR_SELY].asInt();
	mImageSel.widImage = value[PRO_OCR_SELW].asInt();
	mImageSel.heightImage = value[PRO_OCR_SELH].asInt();
	mImageSel.priceImageScaleTag = value[PRO_OCR_SCALETAG].asInt();
	mImageSel.imageBinaryzation = value[PRO_OCR_BINA].asInt();

	ocrPreparedReflect();
}

void OCRProcess::sendBackPrice()
{
	int ourError;
	QString ocrPrice = GetPrice(ourError);

	Json::Value mValData;
	mValData[PRO_HEAD] = RETURN_PRICE;
	mValData[PRO_OCR_PRICE] = ocrPrice.toStdString();
	mValData[PRO_OCR_ERROR] = ourError;
	
	std::wstring winTitle = FLOATWINTITLEW;
	if (sourceHand == PRO_OCR_FROM_MAINDLG)
		winTitle = MAINDLGTITLEW;

	HWND hwnd = ::FindWindow(NULL, winTitle.c_str());

	//if (!::IsWindow(hwnd))
	//{
	//	ZHFuncLib::NativeLog("", "ocr cannot find float win", "a");
	//}

	bool suc = ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
	if (!suc)
	{
		ZHFuncLib::NativeLog("", "ocr process send back price failed", "a");
	}
	else
		ZHFuncLib::NativeLog("ocr", mValData.toStyledString().c_str(), "a");
}

void OCRProcess::ocrPreparedReflect()
{
	Json::Value mValData;
	mValData[PRO_HEAD] = OCR_PREPARED;
	mValData[PRO_OCR_PREPARE_STATUS] = 1;

	HWND hwnd = ::FindWindow(NULL, FLOATWINTITLEW);
	bool suc = ZHFuncLib::SendProcessMessage((HWND)this->winId(), hwnd, ZHIHUI_CODE_MSG, mValData.toStyledString());
	if (!suc)
	{
		ZHFuncLib::NativeLog("", "ocr process prepare reflect failed", "a");
	}
	else
		ZHFuncLib::NativeLog("ocr", mValData.toStyledString().c_str(), "a");
}
