

#define ZH_SCREEN_SELECT_W 1
#define ZH_SCREENS_SELECT_P 1	

#define ZH_GET_PRINCE_W 2
#define ZH_GET_PRINCE_P 2

#define ZH_SHOW_TRADEDETAIL_W 3
#define ZH_SHOW_TRADEDETAIL_P 3

#define ZH_TRADE_REFUND_W 4

#define ZH_OPEN_MAINDLG_W 5

#define ZH_SAVE_SETTING 6

#define ZH_TIP_SHOW_W 7

#define ZH_MAINDLG_CLOSE_W 8

//#define ZH_SHOW_LOGINDLG_W 9

#define ZH_ENABLE_PAY_W 10

#define ZH_REMOVE_TIPDLG_W 11

#define ZH_SWIP_CARD_DLG_CLOSE_W 12

#define ZH_SHOW_MEMBERDETAIL_W 13

#define ZH_SETFOCUSONCASHIER_W 14

#define ZH_REMOVE_SHORTCUTDLG_W 15

#define ZHIHUI_CODE_MSG 5001
#define ZHIHUI_CODE_MSG_LPAR 8123

#define ZHIHUI_MANINPUT_MSG 5002
#define ZHIHUI_MANINPUT_MSG_LPAR 8124

#define HOOKAPI_CREATEFILEA 6001
#define HOOKAPI_CREATEFILEW 6002
#define HOOKAPI_READFILE 6003
#define HOOKAPI_READFILEEX 6004
#define HOOKAPI_WRITEFILE 6005
#define HOOKAPI_WRITEFILEEX 6006
#define HOOKAPI_CLOSEHANDLE 6007

#define NAME_FILE_MAPPINGL		L"bbq_Dll_INJECTION"
#define NAME_FILE_MAPPINGT		_T("bbq_Dll_INJECTION")
// name of file-mapping object that share with the dll and process
#define PRICECOMTAG 5
typedef struct  
{
	char priceCom[PRICECOMTAG];
} CONTENT_FILE_MAPPING, *PCONTENT_FILE_MAPPING;