#include "ZHSettingRW.h"
#include "zhfunclib.h"

#define ROOTNODE "root"
#define SCANCODENODE "CodeScan"
#define SETTINGPATH "bbqsetting.xml"
#define PAYTOOLNODE "PayTool"
#define ATTNAME "name"
#define FIXNUMBYTE "FixNumByte"
#define FIXNUM "FixNum"
#define NUMLEN "NumLength"
#define CARISHER "Carisher"
#define EXENAME "exe"
#define WINDOWNAME "WindowMame"
#define PRINTERNAME "PrinterName"
#define MONEYPOS "MoneyPos"
#define RELITIVETYPE "relitiveType"
#define SELECTX "X"
#define SELECTY "Y"
#define SELECTWIDTH "Width"
#define SELECTHEIGHT "Height"
#define SELECTSCALETAG "ScaleTag"
#define IMAGEBINARYZATION "ImageBinaryzation"
// 收银台信息
#define CDSHOPINFO "ShopCashDesk"
#define CDSHOPID /*"shopid"*/ NULL
#define CDID /*"id"*/  NULL
#define CDROLE /*"role"*/  NULL
#define CDSHOPTYPE /*"shoptype"*/ NULL
#define CDSHOPNAME /*"shopname"*/  NULL
#define CDSHOPCODE /*"shopcode"*/ NULL
#define CDCASHDESKID "cashdeskid"
#define CDCASHDESKNAME "cashdeskname"
#define CDDCDEVNO "dcdevno"
#define CDDCDEVMAC "dcdevmac"
#define CDISBIND "isbind"  // 是否绑定
#define CDISAUTOPRINTER "isAutoPrint"  // 是否自动打印
#define RESTARTTYPE "RestartType"  // 重启时间
#define ISUSEPAYGUN "isUsePayGun"  // 是否使用支付扫码枪
#define ACTUALTIMEGETTIME "ActualTimeGetPrice"

// 打印机信息
#define PRINTERTYPE "PrinterType"
#define PRINTERPAPERWIDTH58 "PrinterPaperWidth58"
#define PRINTERPAPERWIDTH80 "PrinterPaperWidth80"
#define PRINTERPAPERWIDTHSETTING "PrinterPaperWidthSetting"
#define PRINTERCOMMENTFONTSZ "CommentFontSZ"

// 快捷键设置
#define HOTKEYSET "HotKeySet"
#define SHORTPRINTHANDOVER "PrintHandover"
#define SHORTTRADEINDO "TradeInfo"
#define SHORTMODIFIERS "Modifiers"
#define SHORTKEY "Key"


ZHSettingRW::ZHSettingRW(codeSetIO::ZHIHUISETTING &outSetting) : mZHSetting(outSetting)
{
}

ZHSettingRW::~ZHSettingRW(void)
{
}

void ZHSettingRW::ReadZHSetting()
{
	TiXmlDocument doc;
	std::string settingpath = ZHFuncLib::GetWorkPath();
	settingpath += "/";
	settingpath += SETTINGPATH;
	if(!doc.LoadFile(settingpath.c_str())){
		doc.Clear();
		return;
	}
	TiXmlElement *root = doc.FirstChildElement(ROOTNODE); // root节点

	// 读取ScanCode节点
	TiXmlElement *scanCodeEle = root->FirstChildElement(SCANCODENODE);
	releaseZHPayCodeSetting();
	for (TiXmlElement *payToolEle = scanCodeEle->FirstChildElement(PAYTOOLNODE); payToolEle != NULL; payToolEle = payToolEle->NextSiblingElement(PAYTOOLNODE))
	{
		codeSetIO::PayCodeNode *payCodeNode = new codeSetIO::PayCodeNode; // 在FloatWindowDlg里销毁
		readPayToolSetting(payToolEle, *payCodeNode);
		mZHSetting.zhPayCode.push_back(payCodeNode);
	}

	// 读取Carisher节点
	TiXmlElement *cashierEle = root->FirstChildElement(CARISHER);
	readCarishSetting(cashierEle, mZHSetting.carishInfo);

	// 读取收银台节点
	TiXmlElement *deskEle = root->FirstChildElement(CDSHOPINFO);
	readShopCashdeskSetting(deskEle, mZHSetting.shopCashdestInfo);

	// 读取快捷键设置
	TiXmlElement *hotKeyEle = root->FirstChildElement(HOTKEYSET);
	readHotKeysEle(hotKeyEle, mZHSetting.hotKeys);

	doc.Clear();
}

void ZHSettingRW::releaseZHPayCodeSetting()
{
	int sz = mZHSetting.zhPayCode.size();
	for (int i = 0; i < sz; ++i)
	{
		codeSetIO::PayCodeNode *codeNode = mZHSetting.zhPayCode.at(i);
		delete codeNode;
		codeNode = NULL;
	}
	mZHSetting.zhPayCode.clear();
}

bool ZHSettingRW::readPayToolSetting(TiXmlElement *payToolEle, codeSetIO::PayCodeNode &outSetting)
{
	const char *toolName = payToolEle->Attribute(ATTNAME);
	memcpy(outSetting.toolname, toolName, strlen(toolName));

	readEleNum(payToolEle->FirstChildElement(FIXNUMBYTE), outSetting.fixNumByte);

	//for (TiXmlElement *fixNumEle = payToolEle->FirstChildElement(FIXNUM); fixNumEle != NULL; fixNumEle = fixNumEle->NextSiblingElement(FIXNUM))
	//{
	//	int fixNum = 0;
	//	readEleNum(fixNumEle, fixNum);
	//	outSetting.fixNums.push_back(fixNum);
	//}
	outSetting.fixNums.push_back(10);
	outSetting.fixNums.push_back(11);
	outSetting.fixNums.push_back(12);
	outSetting.fixNums.push_back(13);
	outSetting.fixNums.push_back(14);
	outSetting.fixNums.push_back(15);
	outSetting.fixNums.push_back(28);
	outSetting.fixNums.push_back(18);

	readEleNum(payToolEle->FirstChildElement(NUMLEN), outSetting.numLen);

	return true;
}


void ZHSettingRW::readEleNum(TiXmlElement *txtEle, short &outVal)
{
	if (txtEle != NULL)
	{ 
		TiXmlElement *xmlEle = txtEle; 
		TiXmlNode* txtNode = xmlEle->FirstChild();     
		if (txtNode == NULL) return;
		const char *strVal = txtNode->ToText()->Value();          
		outVal = atoi(strVal);                           
	} 
}

void ZHSettingRW::readEleNum(TiXmlElement *txtEle, int &outVal)
{
	if (txtEle != NULL)
	{ 
		TiXmlElement *xmlEle = txtEle; 
		TiXmlNode* txtNode = xmlEle->FirstChild();       
		if (txtNode == NULL) return;
		const char *strVal = txtNode->ToText()->Value();          
		outVal = atoi(strVal);                           
	} 
}

void ZHSettingRW::readEleVal(TiXmlElement *txtEle, char *outVal)
{
	if (txtEle != NULL)  
	{
		TiXmlElement *xmlEle = txtEle;
		TiXmlNode* txtNode = xmlEle->FirstChild();  
		if (txtNode == NULL) 
		{
			outVal[0] = 0;
			return;
		}
		const char *strVal = txtNode->ToText()->Value();  
		int strl = strlen(strVal);
		//memset(outVal, 0, strlen(outVal));  
		memcpy(outVal, strVal, strlen(strVal));    
		outVal[strlen(strVal)] = 0;
	} 
}

bool ZHSettingRW::readCarishSetting(TiXmlElement *carishEle, codeSetIO::CarishDesk &outSetting)
{
	readEleVal(carishEle->FirstChildElement(EXENAME), outSetting.exeName);
	readEleVal(carishEle->FirstChildElement(WINDOWNAME), outSetting.windowName);
	readEleVal(carishEle->FirstChildElement(PRINTERNAME), outSetting.printerName);
	TiXmlElement *moneyEle = carishEle->FirstChildElement(MONEYPOS);
	readEleNum(moneyEle->FirstChildElement(RELITIVETYPE), outSetting.selectRange.relitiveType);
	readEleNum(moneyEle->FirstChildElement(SELECTX), outSetting.selectRange.xCenterDistance);
	readEleNum(moneyEle->FirstChildElement(SELECTY), outSetting.selectRange.yCenterDistance);
	readEleNum(moneyEle->FirstChildElement(SELECTWIDTH), outSetting.selectRange.widImage);
	readEleNum(moneyEle->FirstChildElement(SELECTHEIGHT), outSetting.selectRange.heightImage);
	readEleNum(moneyEle->FirstChildElement(IMAGEBINARYZATION), outSetting.selectRange.imageBinaryzation);
	readEleNum(moneyEle->FirstChildElement(SELECTSCALETAG), outSetting.selectRange.priceImageScaleTag);
	readEleNum(carishEle->FirstChildElement(PRINTERPAPERWIDTH58), outSetting.deviceWidth58);
	readEleNum(carishEle->FirstChildElement(PRINTERPAPERWIDTH80), outSetting.deviceWidth80);
	readEleNum(carishEle->FirstChildElement(PRINTERTYPE), outSetting.printerType);
	readEleNum(carishEle->FirstChildElement(PRINTERPAPERWIDTHSETTING), outSetting.printerPaperWidthSet);
	readEleNum(carishEle->FirstChildElement(PRINTERCOMMENTFONTSZ), outSetting.commentFontSZ);

	return true;
}

bool ZHSettingRW::readZHCashdeskAccountPrivateInfo()
{
	// 读取账户相关的私有配置 顺序 isBind isAutoPrint
	codeSetIO::ShopCashdeskInfo &outSetting = mZHSetting.shopCashdestInfo;
	FILE * fp = NULL;
	std::string accountPath = ZHFuncLib::GetWorkPath() + "/accountPrivate/";
	accountPath += outSetting.account;
	//outSetting.isBind = 0;
	outSetting.isAutoPrint = 0;
	if((fp = fopen(accountPath.c_str(), "r")) != NULL)
	{
		fread(&outSetting.isBind, sizeof(short), 1, fp);
		fread(&outSetting.isAutoPrint, sizeof(short), 1, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool ZHSettingRW::readShopCashdeskSetting(TiXmlElement *deskEle, codeSetIO::ShopCashdeskInfo &outSetting)
{
	if (deskEle == NULL)
		return false;
	// 读取账户相关的私有配置 顺序 isBind isAutoPrint
	if (!readZHCashdeskAccountPrivateInfo())
	{
		readEleNum(deskEle->FirstChildElement(CDISAUTOPRINTER), outSetting.isAutoPrint);
	}
	readEleNum(deskEle->FirstChildElement(CDISBIND), outSetting.isBind);
	readEleNum(deskEle->FirstChildElement(ACTUALTIMEGETTIME), outSetting.isGetPriceActualTime);
	readEleNum(deskEle->FirstChildElement(RESTARTTYPE), outSetting.restartType);
	outSetting.isUsePayGun = 1;
	readEleNum(deskEle->FirstChildElement(ISUSEPAYGUN), outSetting.isUsePayGun);
	readEleNum(deskEle->FirstChildElement(CDSHOPID), outSetting.shopid);
	readEleNum(deskEle->FirstChildElement(CDID), outSetting.id);
	readEleNum(deskEle->FirstChildElement(CDROLE), outSetting.role);
	readEleNum(deskEle->FirstChildElement(CDSHOPTYPE), outSetting.shoptype);
	readEleVal(deskEle->FirstChildElement(CDSHOPNAME), outSetting.shopName);
	readEleVal(deskEle->FirstChildElement(CDSHOPCODE), outSetting.shopCode);
	readEleVal(deskEle->FirstChildElement(CDCASHDESKID), outSetting.cashdeskId);
	readEleVal(deskEle->FirstChildElement(CDCASHDESKNAME), outSetting.cashdeskName);
	readEleVal(deskEle->FirstChildElement(CDDCDEVNO), outSetting.dcdevNo);
	readEleVal(deskEle->FirstChildElement(CDDCDEVMAC), outSetting.dcdevMac);

	return true;
}

bool ZHSettingRW::readHotKeysEle(TiXmlElement * hotkeyEle, codeSetIO::HOTKEYS &ioHotKeys)
{
	if (hotkeyEle == NULL)
		return false;

	TiXmlElement *tmpHotKeyEle = hotkeyEle->FirstChildElement(SHORTPRINTHANDOVER);
	if (tmpHotKeyEle == NULL)
		return false;
	readEleNum(tmpHotKeyEle->FirstChildElement(SHORTMODIFIERS), ioHotKeys.hPrintHandover.modifier);
	readEleNum(tmpHotKeyEle->FirstChildElement(SHORTKEY), ioHotKeys.hPrintHandover.qtkey);
	tmpHotKeyEle = hotkeyEle->FirstChildElement(SHORTTRADEINDO);
	readEleNum(tmpHotKeyEle->FirstChildElement(SHORTMODIFIERS), ioHotKeys.hTradeInfo.modifier);
	readEleNum(tmpHotKeyEle->FirstChildElement(SHORTKEY), ioHotKeys.hTradeInfo.qtkey);

	return true;
}

void ZHSettingRW::WriteZHSetting()
{
	//int len = WideCharToMultiByte(CP_ACP,0,targetCarish,-1,NULL,0,NULL,NULL); 
	//WideCharToMultiByte(CP_ACP,0,targetCarish,-1,mZHSetting.carishInfo.exeName,len,NULL,NULL ); 

	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement(ROOTNODE);
	doc.LinkEndChild(root); // 根节点

	// 写入CodeScan节点
	TiXmlElement* codeScanEle = new TiXmlElement(SCANCODENODE);
	root->LinkEndChild(codeScanEle);
	// 可能有多个PayCodeNode
	int sz = mZHSetting.zhPayCode.size();
	for (int i = 0; i < sz; ++i)
	{
		codeSetIO::PayCodeNode *codeNode = mZHSetting.zhPayCode.at(i);
		codeScanEle->LinkEndChild(writePayToolSetting(*codeNode));
	}

	// 写入Carisher节点
	root->LinkEndChild(writeCarishSetting(mZHSetting.carishInfo));

	// 写入商店收银台信息
	root->LinkEndChild(writeShopCashdeskSetting(mZHSetting.shopCashdestInfo));

	// 写入快捷键
	root->LinkEndChild(writeHotKeySetting(mZHSetting.hotKeys));

	std::string settingpath = ZHFuncLib::GetWorkPath();
	settingpath += "/";
	settingpath += SETTINGPATH;

	bool success = doc.SaveFile(settingpath.c_str());
	doc.Clear();
}


TiXmlElement *ZHSettingRW::writePayToolSetting(codeSetIO::PayCodeNode &inSetting)
{
#define NUMCHARLEN 32
	TiXmlElement *payToolEle = new TiXmlElement(PAYTOOLNODE);
	payToolEle->SetAttribute(ATTNAME, inSetting.toolname);

	char numstring[NUMCHARLEN];
	memset(numstring, 0, NUMCHARLEN);
	_itoa(inSetting.fixNumByte, numstring, 10);
	createEleText(payToolEle, FIXNUMBYTE, numstring);

	int sz = inSetting.fixNums.size();
	for (int i = 0; i < sz; ++i)
	{
		memset(numstring, 0, NUMCHARLEN);
		_itoa(inSetting.fixNums[i], numstring, 10);
		createEleText(payToolEle, FIXNUM, numstring);
	}

	memset(numstring, 0, NUMCHARLEN);
	_itoa(inSetting.numLen, numstring, 10);
	createEleText(payToolEle, NUMLEN, numstring);

	return payToolEle;
#undef NUMCHARLEN
}

inline void ZHSettingRW::createEleText(TiXmlElement *parEle, char *eleName, const char *eleVal)
{
	if (eleName == NULL) return;
	TiXmlElement *attributeEle = new TiXmlElement(eleName); 
	TiXmlText *text = new TiXmlText(eleVal);
	attributeEle->LinkEndChild(text);
	parEle->LinkEndChild(attributeEle);   
}


TiXmlElement *ZHSettingRW::writeCarishSetting(codeSetIO::CarishDesk &inSetting)
{
#define NUMCHARLEN 32
	TiXmlElement *carisherEle = new TiXmlElement(CARISHER);
	createEleText(carisherEle, EXENAME, inSetting.exeName);
	createEleText(carisherEle, WINDOWNAME, inSetting.windowName);
	createEleText(carisherEle, PRINTERNAME, inSetting.printerName);

	TiXmlElement *moneyPosEle = new TiXmlElement(MONEYPOS);
	carisherEle->LinkEndChild(moneyPosEle);
	char numstring[NUMCHARLEN];
	memset(numstring, 0, NUMCHARLEN);
	_itoa(inSetting.selectRange.relitiveType, numstring, 10);
	createEleText(moneyPosEle, RELITIVETYPE, numstring);
	_itoa(inSetting.selectRange.xCenterDistance, numstring, 10);
	createEleText(moneyPosEle, SELECTX, numstring);
	_itoa(inSetting.selectRange.yCenterDistance, numstring, 10);
	createEleText(moneyPosEle, SELECTY, numstring);
	_itoa(inSetting.selectRange.widImage, numstring, 10);
	createEleText(moneyPosEle, SELECTWIDTH, numstring);
	_itoa(inSetting.selectRange.heightImage, numstring, 10);
	createEleText(moneyPosEle, SELECTHEIGHT, numstring);
	_itoa(inSetting.selectRange.priceImageScaleTag, numstring, 10);
	createEleText(moneyPosEle, SELECTSCALETAG, numstring);
	_itoa(inSetting.selectRange.imageBinaryzation, numstring, 10);
	createEleText(moneyPosEle, IMAGEBINARYZATION, numstring);

	_itoa(inSetting.deviceWidth58, numstring, 10);
	createEleText(carisherEle, PRINTERPAPERWIDTH58, numstring);
	_itoa(inSetting.deviceWidth80, numstring, 10);
	createEleText(carisherEle, PRINTERPAPERWIDTH80, numstring);
	_itoa(inSetting.printerType, numstring, 10);
	createEleText(carisherEle, PRINTERTYPE, numstring);
	_itoa(inSetting.printerPaperWidthSet, numstring, 10);
	createEleText(carisherEle, PRINTERPAPERWIDTHSETTING, numstring);
	_itoa(inSetting.commentFontSZ, numstring, 10);
	createEleText(carisherEle, PRINTERCOMMENTFONTSZ, numstring);

	return carisherEle;
#undef NUMCHARLEN
}


TiXmlElement *ZHSettingRW::writeShopCashdeskSetting(codeSetIO::ShopCashdeskInfo &inSetting)
{
	TiXmlElement *deskEle = new TiXmlElement(CDSHOPINFO);
#define NUMCHARLEN 32
	char numstring[NUMCHARLEN];
	memset(numstring, 0, NUMCHARLEN);
	_itoa(inSetting.isBind, numstring, 10);
	createEleText(deskEle, CDISBIND, numstring);
	// 写入账户相关的私有配置 顺序 isBind isAutoPrint
	FILE * fp = NULL;
	std::string accountPath = ZHFuncLib::GetWorkPath() + "/accountPrivate/";
	accountPath += inSetting.account;
	if((fp = fopen(accountPath.c_str(), "w")) != NULL)
	{
		fwrite(&inSetting.isBind, sizeof(short), 1, fp);
		fwrite(&inSetting.isAutoPrint, sizeof(short), 1, fp);
		fclose(fp);
	}
	_itoa(inSetting.isGetPriceActualTime, numstring, 10);
	createEleText(deskEle, ACTUALTIMEGETTIME, numstring);
	_itoa(inSetting.restartType, numstring, 10);
	createEleText(deskEle, RESTARTTYPE, numstring);
	_itoa(inSetting.isUsePayGun, numstring, 10);
	createEleText(deskEle, ISUSEPAYGUN, numstring);
	_itoa(inSetting.shopid, numstring, 10);
	createEleText(deskEle, CDSHOPID, numstring);
	_itoa(inSetting.id, numstring, 10);
	createEleText(deskEle, CDID, numstring);
	_itoa(inSetting.role, numstring, 10);
	createEleText(deskEle, CDROLE, numstring);
	_itoa(inSetting.shoptype, numstring, 10);
	createEleText(deskEle, CDSHOPTYPE, numstring);
	createEleText(deskEle, CDSHOPNAME, inSetting.shopName);
	createEleText(deskEle, CDSHOPCODE, inSetting.shopCode);
	//_itoa(inSetting.cashdeskId, numstring, 10);
	//createEleText(deskEle, CDCASHDESKID, numstring);
	createEleText(deskEle, CDCASHDESKID, inSetting.cashdeskId);
	createEleText(deskEle, CDCASHDESKNAME, inSetting.cashdeskName);
	createEleText(deskEle, CDDCDEVNO, inSetting.dcdevNo);
	createEleText(deskEle, CDDCDEVMAC, inSetting.dcdevMac);
	return deskEle;
#undef NUMCHARLEN
}

TiXmlElement *ZHSettingRW::writeHotKeySetting(codeSetIO::HOTKEYS &inHotKeys)
{
	TiXmlElement *hotKeysEle = new TiXmlElement(HOTKEYSET);
	TiXmlElement *tmpHotKeyEle = NULL;
	tmpHotKeyEle = createHotKeySetting(new TiXmlElement(SHORTPRINTHANDOVER), inHotKeys.hPrintHandover);
	hotKeysEle->LinkEndChild(tmpHotKeyEle);
	tmpHotKeyEle = createHotKeySetting(new TiXmlElement(SHORTTRADEINDO), inHotKeys.hTradeInfo);
	hotKeysEle->LinkEndChild(tmpHotKeyEle);

	return hotKeysEle;
}

TiXmlElement *ZHSettingRW::createHotKeySetting(TiXmlElement *parEle, codeSetIO::HOTKEY &inHotKey)
{
#define NUMCHARLEN 32
	char numstring[NUMCHARLEN];
	_itoa(inHotKey.modifier, numstring, 10);
	createEleText(parEle, SHORTMODIFIERS, numstring);
	_itoa(inHotKey.qtkey, numstring, 10);
	createEleText(parEle, SHORTKEY, numstring);

#undef NUMCHARLEN
	return parEle;
}