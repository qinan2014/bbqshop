#ifndef BBQ_ZH_SETTING_RW_H
#define BBQ_ZH_SETTING_RW_H

#include "tinyxml.h"
#include "PayCodeNode.h"

class __declspec(dllexport) ZHSettingRW
{
public:
	ZHSettingRW(codeSetIO::ZHIHUISETTING &outSetting);
	virtual ~ZHSettingRW(void);

	void ReadZHSetting();
	void WriteZHSetting();
private:
	codeSetIO::ZHIHUISETTING &mZHSetting;

	void releaseZHPayCodeSetting();
	bool readPayToolSetting(TiXmlElement *payToolEle, codeSetIO::PayCodeNode &outSetting);
	void readEleNum(TiXmlElement *txtEle, short &outVal);
	void readEleNum(TiXmlElement *txtEle, int &outVal);
	bool readCarishSetting(TiXmlElement *carishEle, codeSetIO::CarishDesk &outSetting);
	void readEleVal(TiXmlElement *txtEle, char *outVal);
	bool readShopCashdeskSetting(TiXmlElement *deskEle, codeSetIO::ShopCashdeskInfo &outSetting);
	bool readHotKeysEle(TiXmlElement * deskEle,codeSetIO::HOTKEYS &ioHotKeys);
	bool readZHCashdeskAccountPrivateInfo();
	TiXmlElement *writePayToolSetting(codeSetIO::PayCodeNode &inSetting);
	inline void createEleText(TiXmlElement *parEle, char *eleName, const char *eleVal);
	TiXmlElement *writeCarishSetting(codeSetIO::CarishDesk &inSetting);
	TiXmlElement *writeShopCashdeskSetting(codeSetIO::ShopCashdeskInfo &inSetting);
	TiXmlElement* writeHotKeySetting(codeSetIO::HOTKEYS &inHotKeys);
	TiXmlElement* createHotKeySetting(TiXmlElement *parEle, codeSetIO::HOTKEY &inHotKey);
};

#endif 