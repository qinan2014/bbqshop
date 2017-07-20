#ifndef BBQ_URL_SERVER_H
#define BBQ_URL_SERVER_H

#include <string>
#include "AccessServerResult.h"
#include "json/json.h"
//#include "afxmt.h"
#include <Windows.h>
#include <list>
#include "PayCodeNode.h"
#include "URLDefine.h"

#define ZHDOWNLOADPATH "BBQ-Down-Updata"

class __declspec(dllexport) BbqUrlServer
{
public:
	BbqUrlServer(AccessServerResult *serRes);
	BbqUrlServer(AccessServerResult *serRes, codeSetIO::ZHIHUISETTING *inSetting);
	virtual ~BbqUrlServer(void);

	//CRITICAL_SECTION mHttpPostDatasSection;  // http请求数据临界区
	HANDLE m_mutex;
	HANDLE m_mutexBusyURL;
	std::list<std::string > mUrls;
	std::list<std::string > mUrlParams;
	std::list<int > mUrlTag;
	std::list<std::string > mApis;
	HANDLE mPostThread;

	void GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag);
	void GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag);
	void TimeFormatRecover(std::string &outStr, std::string inOriTimeStr);
	void DecreasePostURLNum();
	int GetPostURLNum();
	bool PostDataToServer(std::string url, std::string urlParams, int urlTag, std::string inApi);
	bool IsImportentOperateNow();
	void YesImportentOperating();
	void NoImportentOperating();
	void DealWithJSONFrServer(int urlTag, std::string urlApi);
	void RecordMemoryInfo(const char *pTag, const char *logLevel = LOG_DEBUG, const char *logModule = LOG_MEMEORY, int urlTag = -1);
	void SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag = -1);
	int GetMAC(char *mac);
	std::string GetPayTool(int inType);
	std::string SearchBBQDownloadDir(int &outSearchRes);
	void SetZHSetting(codeSetIO::ZHIHUISETTING *inSetting);

	std::string GetMD5(const std::string& strFilePath);

private:
	AccessServerResult *serverInfoDeal;
	int postURLNum; // 请求计数器
	bool isImportOperating; // 是否正在执行某些重要操作，比如请求
	codeSetIO::ZHIHUISETTING *mZHSetting;

	std::string getSign(char *timeStamp, std::string inApi, std::string inData);
	std::string getSign1(const Json::Value &inRootVal);
	void sendJSONData(std::string serverURL, std::string inSecondAddr, const Json::Value &inRootVal, int urlTag, std::string inApi);
	void trimSpace(std::string &outStr);
	void jsonValToSignVal(std::string &outJSONVal);
	void curlError(std::string url, int res, int urlTag);
};

#endif 