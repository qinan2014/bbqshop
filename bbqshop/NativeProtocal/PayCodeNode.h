#include <string>
#include <vector>
//#include "tinyxml.h"

#ifndef _PAY_CODE_NODE_H
#define _PAY_CODE_NODE_H

#define SHORTCTRLASSIC 162
#define SHORTCTRLSTR "Ctrl"
#define SHORTSHIFTASSIC 160
#define SHORTSHIFTSTR "Shift"

namespace codeSetIO
{
#define NAMELEN 125
#define NUMLEN 50
	struct PayCodeNode 
	{
		char toolname[25];
		std::vector<unsigned short> fixNums;
		short fixNumByte;
		short numLen;
		PayCodeNode()
		{
			memset(toolname, 0, 25);
			fixNumByte = 0;
			numLen = 0;
			
		}
	};

	struct SelectRange
	{
		int relitiveType;
		// 选框范围
		int xCenterDistance;
		int yCenterDistance;
		int widImage;
		int heightImage;
		int priceImageScaleTag;  // 截图放大倍数
		short imageBinaryzation;  // 是否二值化
	};

	struct PMousePos
	{
		int xpos;
		int ypos;
		PMousePos()
		{
			xpos = 5;
			ypos = 5;
		}
	};

	struct CarishDesk
	{
		char exeName[NUMLEN];
		char priceCom[NUMLEN];
		char windowName[NAMELEN];
		char printerName[NAMELEN];
		SelectRange selectRange;
		PMousePos mousePos;
		int deviceWidth58;
		int deviceWidth80;
		int printerType;  // 0表示58打印机，1标识80打印机
		int printerPaperWidthSet;
		int commentFontSZ;
		CarishDesk()
		{
			priceCom[0] = 0;
			memset(exeName, 0, NUMLEN);
			memset(windowName, 0, NAMELEN);
			memset(printerName, 0, NAMELEN);
			memset(&selectRange, 0, sizeof(SelectRange));
			deviceWidth58 = -1;
			deviceWidth80 = -1;
			printerType = 0;
			printerPaperWidthSet = 0;
			commentFontSZ = 28;
		}
	};

	struct ShopCashdeskInfo
	{
		int shopid;  // 门店主键ID
		int id;   // 登陆用户的主键id
		int role;    // 当前登录用户的角色（1店长2收银员）
		int shoptype;  // 当前登录用户所在店的类型（1总店2分店）
		char shopName[NAMELEN];
		char shopCode[NUMLEN];   // 门店编号
		char cashdeskId[NUMLEN];  // 收银台编号
		char cashdeskName[NAMELEN];  // 收银台名字
		char dcdevNo[NUMLEN];  // 数据采集服务器编号
		char dcdevMac[NUMLEN];  // 数据采集服务器MAC

		char userName[NAMELEN];  // 昵称
		char account[NAMELEN];   // 登陆账号
		char loginTime[NAMELEN];  // 上次登录时间
		char exitTime[NAMELEN];  // 用户上次退出时间
		short workStatus;    // 工作状态（0下班，1上班）

		short isBind;   // 是否已经绑定
		short isAutoPrint;  // 是否自动打印
		short restartType;  // 定时重启类型
		short isUsePayGun;  // 是否使用支付扫码枪
		short isGetPriceActualTime;  // 是否需要实时截图 1标识实时，0标识不截图
		ShopCashdeskInfo()
		{
			account[0] = 0;
		}
	};

	struct HOTKEY 
	{
		int modifier;
		int qtkey;
		HOTKEY()
		{
			modifier = -1;
			qtkey = -1;
		}
	};

	struct HOTKEYS 
	{
		HOTKEY hWXKey;
		HOTKEY hAlipayKey;
	};

	struct ZHIHUISETTING
	{
		std::vector<codeSetIO::PayCodeNode *> zhPayCode;
		CarishDesk carishInfo;
		ShopCashdeskInfo shopCashdestInfo;
		HOTKEYS hotKeys;
	};
#undef NAMELEN
#undef NUMLEN
}

#endif

