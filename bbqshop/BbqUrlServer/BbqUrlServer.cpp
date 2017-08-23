#include "MD5.h"
#include "BbqUrlServer.h"
#include <time.h>
#include <curl/curl.h>
#include "zhfunclib.h"
#include "psapi.h"
#include "ZBase64.h"
#pragma comment(lib,"psapi.lib")
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")

#include <Iphlpapi.h>
#include <iostream>
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库

BbqUrlServer::BbqUrlServer(AccessServerResult *serRes) : serverInfoDeal(serRes)
{
	mPostThread = NULL;
	postURLNum = 0;
	//InitializeCriticalSection(&mHttpPostDatasSection);
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
	m_mutexBusyURL = ::CreateMutex(NULL, FALSE, NULL);
	NoImportentOperating();
	mZHSetting = NULL;
}

BbqUrlServer::BbqUrlServer(AccessServerResult *serRes, codeSetIO::ZHIHUISETTING *inSetting) : serverInfoDeal(serRes)
{
	mPostThread = NULL;
	postURLNum = 0;
	//InitializeCriticalSection(&mHttpPostDatasSection);
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
	m_mutexBusyURL = ::CreateMutex(NULL, FALSE, NULL);
	NoImportentOperating();
	mZHSetting = inSetting;
}

BbqUrlServer::~BbqUrlServer(void)
{
	//DeleteCriticalSection(&mHttpPostDatasSection);
	::CloseHandle(m_mutex); 
	::CloseHandle(m_mutexBusyURL); 
}

void BbqUrlServer::SetZHSetting(codeSetIO::ZHIHUISETTING *inSetting)
{
	mZHSetting = inSetting;
}

void BbqUrlServer::GetDataFromServer(std::string inSecondAddr, std::string inApi, std::string inData, int urlTag)
{
	//mCurApi = inApi;
	// 时间戳
	time_t timeStamp = time(0) * 1000;
	char stampStr[15];
	memset(stampStr, 0, 15);
	_i64toa(timeStamp, stampStr, 10);  

	//LogError("GetDataFromServer func", "a");
	Json::Value root;
	root["timestamp"] = stampStr;
	root["sign"] = getSign(stampStr, inApi, inData);
	root["api"] = inApi;
	root["data"] = inData;

	sendJSONData(SERVERIP, inSecondAddr, root, urlTag, inApi);
}

void BbqUrlServer::GetDataFromServer1(std::string inUrl, std::string inSecondAddr, std::string inApi, Json::Value &ioRootVal, int urlTag)
{
	//mCurApi = inSecondAddr;
	// 时间戳
	time_t timeStamp = time(0) * 1000;
	char stampStr[15];
	memset(stampStr, 0, 15);
	_i64toa(timeStamp, stampStr, 10);  

	//Json::Value root;
	ioRootVal["timestamp"] = stampStr;
	std::string sign = getSign1(ioRootVal);
	ioRootVal["sign"] = sign;

	sendJSONData(inUrl, inSecondAddr, ioRootVal, urlTag, inSecondAddr);
}


std::string BbqUrlServer::getSign(char *timeStamp, std::string inApi, std::string inData)
{
	std::string oriStr = "api=";
	oriStr += inApi;
	oriStr += "&data=";
	oriStr += inData;
	oriStr += "&timestamp=";
	oriStr += timeStamp;
	oriStr += ZHHUIKEY;
	//LogError(oriStr.c_str(), "a");
	return md5(oriStr);
}

std::string BbqUrlServer::getSign1(const Json::Value &inRootVal)
{
	std::string oriStr = inRootVal.toStyledString();
	jsonValToSignVal(oriStr);
	trimSpace(oriStr);
	oriStr += "BQHHSPBOWTXZNFDUCXKBRBPNRPEGSTKT";
	std::string md5str = md5(oriStr);

	return md5str;
}


std::string mRecvJsonStr;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);

	char *jsonVal = (char *)malloc(nmemb + 1);
	memcpy(jsonVal, ptr, nmemb);
	jsonVal[nmemb] = 0;
	mRecvJsonStr += jsonVal;
	free(jsonVal);

	return nmemb;
}

int ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)    
{    
	if ( dltotal > -0.1 && dltotal < 0.1 )    
	{  
		return 0;  
	}  
	int nPos = (int)((dlnow / dltotal) * 100);  

	static double tmpNow = 0.0;
	static int sametimes = 0;
	if (dlnow > tmpNow)
		return 0;
	else if (sametimes > 6)
	{
		char tmpbuf[300];
		sprintf(tmpbuf, "ProgressCallback dltotal %f, dlnow %f, ultotal %f, ulnow %f", dltotal, dlnow, ultotal, ulnow);
		ZHFuncLib::NativeLog("", tmpbuf, "a");

		tmpNow = 0.0;
		sametimes = 0;
		return -2; //返回非0值就会终止 curl_easy_perform 执行   
	}
	else
		++sametimes;
	return 0;
}

bool BbqUrlServer::PostDataToServer(std::string url, std::string urlParams, int urlTag, std::string inApi)
{
	bool sendsuc = false;
	char szJsonData[10240];  
	memcpy(szJsonData, urlParams.c_str(), urlParams.length());
	szJsonData[urlParams.length()] = 0;
	CURL *pCurl = NULL;  
	CURLcode res;  
	// In windows, this will init the winsock stuff  
	// get a curl handle  
	pCurl = curl_easy_init();
	if (NULL != pCurl)   
	{  
		// 设置超时时间为6秒  
		curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 6);  
		// First set the URL that is about to receive our POST.   
		// This URL can just as well be a   
		// https:// URL if that is what should receive the data.  
		curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str());   
		//// 设置http发送的内容类型为JSON  
		curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");  
		curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, plist);  
		// 设置要POST的JSON数据  
		curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, szJsonData);  
		curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_data);

		// Perform the request, res will get the return code   
		std::string pagefilename = ZHFuncLib::GetWorkPath();
		
		pagefilename += "/respone.txt";
		FILE *pagefile;
		//写入到文件
		//fopen_s(&pagefile,pagefilename.c_str(), "wb");
		fopen_s(&pagefile,pagefilename.c_str(), "wb");
		if (pagefile)
			curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, pagefile);
		else
			ZHFuncLib::NativeLog("", "open respone.txt error", "a");

		curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 0);    
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
		curl_easy_setopt(pCurl, CURLOPT_PROGRESSDATA,this);  
		char urlbuf[300];
		sprintf(urlbuf, "post http request: url: %s / %s", url.c_str(), inApi.c_str());
		ZHFuncLib::NativeLog("", urlbuf, "a");
		// 执行一次POST
		res = curl_easy_perform(pCurl);
		ZHFuncLib::NativeLog("", "post url back", "a");
		if (res != CURLE_OK)
		{
			curlError(url, res, urlTag);
			NoImportentOperating();
			char tmpbuf[200];
			sprintf(tmpbuf, "curl_easy_perform url: %s res: %d   %s\n", url.c_str(), res, curl_easy_strerror(res));
			ZHFuncLib::NativeLog("", tmpbuf, "a");
		}
		else
			sendsuc = true;

		if (pagefile)
			fclose(pagefile);

		curl_easy_cleanup(pCurl); 
		curl_slist_free_all(plist);
	}  
	curl_global_cleanup();

	return sendsuc;
}

// 数据发送线程
DWORD WINAPI PostServerProc(LPVOID pM)  
{  
	BbqUrlServer *pWnd = static_cast<BbqUrlServer *>(pM);
	bool isPosting = true;
	while (isPosting)
	{
		//EnterCriticalSection(&pWnd->mHttpPostDatasSection);
		WaitForSingleObject(pWnd->m_mutex, INFINITE);  
		if (pWnd->mUrls.size() == 0)
		{
			//LeaveCriticalSection(&pWnd->mHttpPostDatasSection);
			::ReleaseMutex(pWnd->m_mutex); 
			//pWnd->LogError("suspend post thread", "a");
			pWnd->NoImportentOperating();
			pWnd->mPostThread = NULL;
			isPosting = false;
			//SuspendThread(GetCurrentThread());
			continue;
		}
		std::string url = pWnd->mUrls.front();
		std::string urlPara = pWnd->mUrlParams.front();
		int urlTag = pWnd->mUrlTag.front();
		std::string urlApi = pWnd->mApis.front();
		pWnd->mUrls.pop_front();
		pWnd->mUrlParams.pop_front();
		pWnd->mUrlTag.pop_front();
		pWnd->mApis.pop_front();
		//LeaveCriticalSection(&pWnd->mHttpPostDatasSection);
		::ReleaseMutex(pWnd->m_mutex); 
		Sleep(200);
		//EnterCriticalSection(&pWnd->mHttpPostingSection);
		pWnd->YesImportentOperating();
		bool sendsuc = pWnd->PostDataToServer(url, urlPara, urlTag, urlApi);
		//LeaveCriticalSection(&pWnd->mHttpPostingSection);
		if (sendsuc)
			pWnd->DealWithJSONFrServer(urlTag, urlApi);
		pWnd->DecreasePostURLNum();
	}
	return 0;  
}

void BbqUrlServer::sendJSONData(std::string serverURL, std::string inSecondAddr, const Json::Value &inRootVal, int urlTag, std::string inApi)
{
	std::string url = serverURL + inSecondAddr;
	std::string rootStr = inRootVal.toStyledString();
	trimSpace(rootStr);

	//ZHFuncLib::NativeLog("", rootStr.c_str(), "a");

	if (inRootVal.isMember("data"))
		TimeFormatRecover(rootStr, inRootVal["data"].asCString());
	//EnterCriticalSection(&mHttpPostDatasSection);
	WaitForSingleObject(m_mutex, INFINITE);
	//bool isUrlEmpty = (mUrls.size() == 0);
	mUrls.push_back(url);
	mUrlParams.push_back(rootStr);
	mUrlTag.push_back(urlTag);
	mApis.push_back(inApi);
	++postURLNum;
	//LeaveCriticalSection(&mHttpPostDatasSection);
	::ReleaseMutex(m_mutex);

	char tmpbuf[100];
	// 创建线程
	if (mPostThread == NULL){
		sprintf(tmpbuf, "create thread sendJSONData urlTag = %d, postUrlNum = %d", urlTag, postURLNum);
		ZHFuncLib::NativeLog("", tmpbuf, "a");
		mPostThread = CreateThread(NULL, 0, PostServerProc, this, 0, NULL);
	}
	else{
		ResumeThread(mPostThread);
		sprintf(tmpbuf, "resume thread sendJSONData urlTag = %d, postUrlNum = %d", urlTag, postURLNum);
		ZHFuncLib::NativeLog("", tmpbuf, "a");
	}
}

void BbqUrlServer::DecreasePostURLNum()
{
	//EnterCriticalSection(&mHttpPostDatasSection);
	WaitForSingleObject(m_mutex, INFINITE);
	--postURLNum;
	if (postURLNum < 0)
		postURLNum = 0;
	//LeaveCriticalSection(&mHttpPostDatasSection);
	::ReleaseMutex(m_mutex);
}

int BbqUrlServer::GetPostURLNum()
{
	int tmpNum;
	//EnterCriticalSection(&mHttpPostDatasSection);
	WaitForSingleObject(m_mutex, INFINITE);
	tmpNum = postURLNum;
	//LeaveCriticalSection(&mHttpPostDatasSection);
	::ReleaseMutex(m_mutex);
	return tmpNum;
}


void BbqUrlServer::trimSpace(std::string &outStr)
{
	std::string::size_type rePos;
	while ((rePos = outStr.find(" ")) != -1) {
		outStr.replace(rePos, 1, "");
	}
	while ((rePos = outStr.find("\\")) != -1) {
		outStr.replace(rePos, 1, "");
	}
	while ((rePos = outStr.find("\"{")) != -1) {
		outStr.replace(rePos, 2, "{");
	}
	while ((rePos = outStr.find("}\"")) != -1) {
		outStr.replace(rePos, 2, "}");
	}
	while ((rePos = outStr.find("\"[")) != -1) {
		outStr.replace(rePos, 2, "[");
	}
	while ((rePos = outStr.find("]\"")) != -1) {
		outStr.replace(rePos, 2, "]");
	}
}

void BbqUrlServer::jsonValToSignVal(std::string &outJSONVal)
{
	std::string::size_type rePos;
	while ((rePos = outJSONVal.find('{')) != -1) {
		outJSONVal.replace(rePos, 1, 1, ' ');
	}
	while ((rePos = outJSONVal.find('}')) != -1) {
		outJSONVal.replace(rePos, 1, 1, ' ');
	}
	while ((rePos = outJSONVal.find('"')) != -1) {
		outJSONVal.replace(rePos, 1, 1, ' ');
	}
	while ((rePos = outJSONVal.find(':')) != -1) {
		outJSONVal.replace(rePos, 1, 1, '=');
	}
	while ((rePos = outJSONVal.find(',')) != -1) {
		outJSONVal.replace(rePos, 1, 1, '&');
	}
	while ((rePos = outJSONVal.find(" ")) != -1) {
		outJSONVal.replace(rePos, 1, "");
	}
}

void BbqUrlServer::TimeFormatRecover(std::string &outStr, std::string inOriTimeStr)
{
	if (inOriTimeStr.length() > outStr.length())
		return;
	std::string noemptyStr = inOriTimeStr;
	int resPos = 0;
	//while ((resPos = noemptyStr.find(" ")) != -1) {
	//	noemptyStr.replace(resPos, 1, "");
	//}

	trimSpace(noemptyStr);

	resPos = outStr.find(noemptyStr);
	if (resPos < 0)
	{
		return;
	}
	outStr.replace(resPos, noemptyStr.length(), inOriTimeStr);
}

bool BbqUrlServer::IsImportentOperateNow()
{
	bool isoperating = false;
	WaitForSingleObject(m_mutexBusyURL, INFINITE);
	isoperating = isImportOperating;
	//char tmpbuf[100];
	//sprintf(tmpbuf, "isoperating: %d", isoperating);
	//ZHFuncLib::NativeLog("", tmpbuf, "a");
	::ReleaseMutex(m_mutexBusyURL);

	return isoperating;
}

void BbqUrlServer::YesImportentOperating()
{
	WaitForSingleObject(m_mutexBusyURL, INFINITE);
	isImportOperating = true;
	//ZHFuncLib::NativeLog("", "set operating true", "a");
	::ReleaseMutex(m_mutexBusyURL);
}

void BbqUrlServer::NoImportentOperating()
{
	WaitForSingleObject(m_mutexBusyURL, INFINITE);
	isImportOperating = false;
	//ZHFuncLib::NativeLog("", "set operating false", "a");
	::ReleaseMutex(m_mutexBusyURL);
}

inline void BbqUrlServer::DealWithJSONFrServer(int urlTag, std::string urlApi)
{
	if (urlTag == URL_RECORE_LOGIN_MEMORY)
	{
		mRecvJsonStr = "";
		return;
	}
	//ZHFuncLib::NativeLog("",  mRecvJsonStr.c_str(), "a"); 
	if (serverInfoDeal == NULL)
		return;
	bool jsonRight = serverInfoDeal->DealWithJSONFrServer(mRecvJsonStr, urlTag, urlApi);
	if (jsonRight)
		mRecvJsonStr = "";
}

inline void BbqUrlServer::curlError(std::string url, int res, int urlTag)
{
	if (urlTag == URL_RECORE_LOGIN_MEMORY)
		return;
	if (serverInfoDeal == NULL)
		return;
	serverInfoDeal->CurlError(url, res, urlTag);
}

void BbqUrlServer::RecordMemoryInfo(const char *pTag, const char *logLevel, const char *logModule, int urlTag)
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	unsigned int workingsz = pmc.WorkingSetSize / 2048;
	char memeryInfo[200];
	sprintf(memeryInfo, "%s, WorkingSetSize: %d k", pTag,
		workingsz);
	SendToURLRecord(logLevel, logModule, memeryInfo, urlTag);
	if (workingsz > 70000)
		ZHFuncLib::NativeLog("", memeryInfo, "a");
}

void BbqUrlServer::SendToURLRecord(const char *logLevel, const char *logModule, const char *logMessage, int urlTag)
{
	std::string tmpMsg = logMessage;
	time_t tmptm = time(0);
	tmpMsg += "---";
	tmpMsg += ctime(&tmptm);

	std::string::size_type rePos;
	while ((rePos = tmpMsg.find(" ")) != -1) {
		tmpMsg.replace(rePos, 1, "");
	}

	if (mZHSetting == NULL)
		return;
	codeSetIO::ShopCashdeskInfo &shopInfo = mZHSetting->shopCashdestInfo;
	Json::Value mValData;
	mValData["shopcode"] = shopInfo.shopCode;
	char mac[MACADDRLEN];
	ZHFuncLib::NativeLog("", "BbqUrlServer::SendToURLRecord", "a");
	GetMAC(mac);
	mValData["kxdevno"] = mac;
	mValData["loglevel"] = logLevel;
	mValData["logmodule"] = logModule;
	ZBase64 base64;
	mValData["logmessage"] = base64.Encode(tmpMsg.c_str(), tmpMsg.length());

	std::string itemVal = mValData.toStyledString();
	while ((rePos = itemVal.find(" ")) != -1) {
		itemVal.replace(rePos, 1, "");
	}
	GetDataFromServer("api/app/v1", UPLOADLOGAPI, itemVal, urlTag);
}


int BbqUrlServer::GetMAC(char *mac)   
{
#define MACINFO "macc.bbqinfo"
	//FILE *fp = NULL;
	//char tmpBuf[300];
	//sprintf(tmpBuf, "%s/%s", WCharToChar(GetCurExeDir()), MACINFO);
	//fp = fopen(tmpBuf, "r");
	//int readsz = 0;
	//if (fp != NULL)
	//{
	//	readsz = fread(mac, 1, MACADDRLEN, fp);
	//	fclose(fp);
	//}
	//if (readsz <= 0)
	//{
	//	for (int i = 0; i < MACADDRLEN - 1; i++)
	//	{
	//		sprintf(mac + i, "%02X", rand());
	//	}
	//	mac[MACADDRLEN - 1] = 0;

	//	if((fp = fopen(tmpBuf, "w")) != NULL)
	//	{
	//		fwrite(mac, MACADDRLEN, 1, fp);
	//	}
	//	fclose(fp);

	//}

	//return 0;
	static char macMemory[MACADDRLEN];
	static bool macInMemory = false;
	if (macInMemory)
	{
		memcpy(mac, macMemory, MACADDRLEN -1);
		mac[MACADDRLEN -1] = 0;

		char tmpbuf1[300];
		sprintf(tmpbuf1, "read memory mac: %s", mac);
		ZHFuncLib::NativeLog("", tmpbuf1, "a");

		return 0;
	}

	FILE *fp = NULL;
	char tmpBuf[300];
	sprintf(tmpBuf, "%s/%s", ZHFuncLib::GetWorkPath().c_str(), MACINFO);
	fp = fopen(tmpBuf, "r");
	if (NULL != fp)
	{
		UCHAR txtmac[6];
		for (DWORD i = 0; i < 6; i++)
		{
			fread(&txtmac[i], 1, sizeof(UCHAR), fp);
			sprintf(macMemory + i * 2, "%02X", txtmac[i]);
		}
		fclose(fp);
		macMemory[MACADDRLEN -1] = 0;
		macInMemory = true;

		memcpy(mac, macMemory, MACADDRLEN -1);
		mac[MACADDRLEN -1] = 0;

		char tmpbuf1[300];
		sprintf(tmpbuf1, "read local mac: %s", mac);
		ZHFuncLib::NativeLog("", tmpbuf1, "a");

		return 0;
	}

	// 新的MAC地址获取方法
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
	//得到结构体大小,用于GetAdaptersInfo参数
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);
	//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
	int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	int IPnumPerNetCard = 0;
	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
		//这也是说明为什么stSize既是一个输入量也是一个输出量
		//释放原来的内存空间
		delete pIpAdapterInfo;
		//重新申请内存空间用来存储所有网卡信息
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
		//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
		nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
	}
	if (ERROR_SUCCESS == nRel)
	{
		//输出网卡信息
		//可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			if (pIpAdapterInfo->Next != NULL && pIpAdapterInfo->Address[0] == 0)
			{
				pIpAdapterInfo = pIpAdapterInfo->Next;
				continue;
			}

			fp = fopen(tmpBuf, "w");
			for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
			{
				sprintf(macMemory + i * 2, "%02X", pIpAdapterInfo->Address[i]);
				if(fp != NULL)
					fwrite(&pIpAdapterInfo->Address[i], sizeof(pIpAdapterInfo->Address[i]), 1, fp);
			}
			if (fp != NULL)
				fclose(fp);
			break;
		}
		macMemory[MACADDRLEN -1] = 0;
		macInMemory = true;
	}
	//释放内存空间
	if (pIpAdapterInfo)
	{
		delete pIpAdapterInfo;
	}

	memcpy(mac, macMemory, MACADDRLEN -1);
	mac[MACADDRLEN -1] = 0;

	char tmpbuf1[300];
	sprintf(tmpbuf1, "read first mac: %s", mac);
	ZHFuncLib::NativeLog("", tmpbuf1, "a");

	return 0;  
#undef MACINFO
}

std::string BbqUrlServer::GetPayTool(int inType)
{
	std::string payTool = "NULL";
	switch (inType)
	{
	case 0:
		payTool = "现金";
		break;
	case 1:
		payTool = "支付宝";
		break;
	case 2:
		payTool = "微信";
		break;
	case 3:
		payTool = "百度";
		break;
	case 4:
		payTool = "京东";
		break;
	case 9:
		payTool = "储值卡";
		break;
	default:
		break;
	}
	return payTool;
}

//std::wstring StringToWstring(const std::string str)
//{// string转wstring
//	unsigned len = str.size() * 2;// 预留字节数
//	setlocale(LC_CTYPE, "");     //必须调用此函数
//	wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
//	mbstowcs(p,str.c_str(),len);// 转换
//	std::wstring str1(p);
//	delete[] p;// 释放申请的内存
//	return str1;
//}

std::string BbqUrlServer::SearchBBQDownloadDir(int &outSearchRes)
{
	char systemInfo[512];
	memset(systemInfo,0,512);
	_ULARGE_INTEGER lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
	DWORD serialNumber, maxComponentLength, fsFlags;
	TCHAR szFileSystem[12];
	TCHAR szVolumeName[24];
	char szRoot[4];

	DWORD AllDrives = GetLogicalDrives();     
	if (AllDrives == 0)
	{
		outSearchRes = -1;
		return "";
	}

	char drive;  
	int cnt = 0;  
	unsigned __int64 maxAvailableToCaller = 0;
	char maxRoot[4];
	for(int i = 0; i < 26; ++i)  
	{
		drive = i + 'A';
		sprintf(szRoot,"%c:\\",drive);
		//wchar_t *wszRoot = StringToWstring(szRoot);
		if(GetDriveType(szRoot) == DRIVE_FIXED)
		{
			std::string dir = szRoot;
			dir += ZHDOWNLOADPATH;
			if (PathIsDirectory(dir.c_str()))
			{
				outSearchRes = 0;
				return dir;
			}

			cnt = cnt + 1; 
			if (!GetVolumeInformation(
				szRoot,
				szVolumeName,//卷标
				sizeof(szVolumeName),
				&serialNumber,//卷序列号
				&maxComponentLength,
				&fsFlags,
				szFileSystem,
				sizeof(szFileSystem)))
			{
				printf("Failed to retrieve drive information\n");
			}

			GetDiskFreeSpaceEx(szRoot, &lpFreeBytesAvailableToCaller, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes);

			//printf("分区总空间大小: %d MB\n",lpTotalNumberOfBytes.QuadPart / (1024 *1024));
			//printf("分区剩余空间大小: %d MB\n",lpTotalNumberOfFreeBytes.QuadPart / (1024 *1024));
			//printf("调用者可用的空间大小: %d MB\n",lpFreeBytesAvailableToCaller.QuadPart / (1024 *1024));
			//sprintf(systemInfo, "%s\n%c:%dMB", systemInfo ,drive, lpFreeBytesAvailableToCaller.QuadPart / (1024 *1024));
			if (lpFreeBytesAvailableToCaller.QuadPart > maxAvailableToCaller)
			{
				maxAvailableToCaller = lpFreeBytesAvailableToCaller.QuadPart;
				sprintf(maxRoot, "%c:\\", drive);
			}
		}
	}
	outSearchRes = 1;
	return maxRoot;
}

std::string BbqUrlServer::GetMD5(const std::string& strFilePath)
{
	FILE *file;  
	file = fopen(strFilePath.c_str(), "r");  
	if(!file)  
		return "";  
	MD5 MD5Checksum;   //checksum object  
	int nLength = 0;       //number of bytes read from the file  
	const int nBufferSize = 1024; //checksum the file in blocks of 1024 bytes  
	BYTE Buffer[nBufferSize];   //buffer for data read from the file  

	//checksum the file in blocks of 1024 bytes  
	while ((nLength =fread(Buffer, 1, nBufferSize, file)) > 0 )  
	{  
		MD5Checksum.update( Buffer, nLength );  
	}  

	fclose(file);  

	//finalise the checksum and return it  
	MD5Checksum.finalize();
	return MD5Checksum.hexdigest();
	//return MD5Checksum.Final();
}