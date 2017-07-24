#ifndef BBQ_PROCESS_PROTOCAL_H
#define BBQ_PROCESS_PROTOCAL_H

// catch screen image scale times
#define IMAGE_SCALE_FACTOR0 0.0f
#define IMAGE_SCALE_FACTOR1 1.2f
#define IMAGE_SCALE_FACTOR2 1.3f
#define IMAGE_SCALE_FACTOR3 1.5f
#define IMAGE_SCALE_FACTOR4 2.0f

#define PRO_HEAD "headTag"
#define PRO_DLG_STATUS "dlgStatus" // 0表示关闭 1 表示打开
#define PRO_IS_QR "isQr" // 是否是QR 0否 1是
#define PRO_PRICE "price"
#define PRO_TIPSTR "tipstr"
#define PRO_SCANCODE "scancode"

// data will send to maindlg from floatwindow
#define PRO_SHOP_CODE "shopCode"
#define PRO_ROLE "role"
#define PRO_USERNAME "userName"
#define PRO_ID "id"
#define PRO_SHOP_NAME "shopName"
#define PRO_SHOPID "shopid"
#define PRO_SHOPTYPE "shoptype"
#define PRO_WORKSTATUS "workStatus"
#define PRO_ACCOUNT "account"
#define PRO_LOGINTIME "loginTime"
#define PRO_EXITTIME "exitTime"

// data that ocr need
#define PRO_OCR_FROM_FLOATWIN 1
#define PRO_OCR_FROM_MAINDLG 2
#define PRO_OCR_FROMDLG "fromdlg"
#define PRO_OCR_CASHNAME "cashName"
#define PRO_OCR_REALITIVETYPE "relitiveType"
#define PRO_OCR_SELX "selx"
#define PRO_OCR_SELY "sely"
#define PRO_OCR_SELW "selw"
#define PRO_OCR_SELH "selh"
#define PRO_OCR_SCALETAG "scaletag"
#define PRO_OCR_BINA "isbina"
#define PRO_OCR_PRICE "price"
#define PRO_OCR_ERROR "priceError"

#define PRO_OCR_ERROR_NAME_EMPTY -1
#define PRO_OCR_ERROR_SOFT_NOTOPEN -2
#define PRO_OCR_ERROR_SOFT_NOTVISIBLE -3
#define PRO_OCR_ERROR_SOFT_MIX -4
#define PRO_OCR_ERROR_SOFT_SELECTRANGEERROR -5
#define PRO_OCR_ERROR_SOFT_NOERROR 0

// install
#define PRO_SOFT_PATH "path"

// MainDlg Tab index
#define PRO_MAIN_TABINDEX "tabIndex"
#define PRO_MAIN_NETSTATUS "isNetOK"

// upgrade client icon
#define PRO_CLIENT_ICON_URL "clientIconUrl"
#define PRO_CLIENT_ICON_TYPE "clientIconType"
#define CLIENT_ICON_NAME "tmpClientIconName"

enum ProtocalHeader
{
	TO_FLOAT_SHOWMAINDLG,
	TO_FLOATWIN_CLOSEMAINDLG,
	TO_FLOATWIN_LOGININFO,
	TO_FLOATWIN_REWRITESETTING,
	TO_FLOATWIN_STARTOCR,
	TO_FLOATWIN_HANDOVER,
	TO_FLOATWIN_HANDOVERPRINT,
	TO_FLOATWIN_CURTAB,
	TO_FLOATWIN_CHECKUPGRADE,
	TO_FLOATWIN_RECONNECT_SOCKET,
	TO_SHOWPAYDLG,
	TO_SHOWTIP,
	TO_SHOWTIP_NETERROR,
	TO_MAINDLGHEADER,
	TO_MAINDLG_IMPORTANTDATA,
	TO_MAINDLG_CLOSE_MAIN,
	TO_MAINDLG_SHOW_PAYDLG,
	TO_MAINDLG_SHOWSELF,
	TO_MAINDLG_NETSTATUS,
	TO_OCR_CASHINFO,
	TO_OCR_GETPRICE,
	RETURN_PRICE,
	TO_FLOAT_INSTALL,
	TO_UPDATE_CHECK,
	TO_UPDATE_CLIENT_ICON,
	TO_SHOW_CLIENT_ICON,
};

#include "URLDefine.h"
enum TABTYPE
{
	ERRORTAB,
	CASH,
	TRADELIST,
	CARDVERIFICATE,
	MEMBER_MANAGE,
	STORE_RECORD,
	SETTING,
#ifdef MEITUAN
	TAB_METURNHISTORY,
#endif
};

#endif