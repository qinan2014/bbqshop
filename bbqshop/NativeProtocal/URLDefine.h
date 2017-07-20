#ifndef URL_DEFINE_H
#define URL_DEFINE_H

#include "AllWindowTitle.h"

//#define PROJ_TEST

#ifdef PROJ_TEST
	#if VERSIONINT == VERSIONBIGOLD
		#define URLCLOUND "http://cloud.bboqi.com/"
		#define URLTRADE "http://t.trade.bboqi.com/"
		#define SERVERIP "http://t.cm.bboqi.com/"
	#else
		#define URLCLOUND "http://t.api.bboqi.com/"
		#define URLTRADE "http://t.api.bboqi.com/"
		#define SERVERIP "http://t.cm.bboqi.com/"
	#endif
//#define SERVERIP "192.168.1.7:8088/"
#else
	#if VERSIONINT == VERSIONBIGOLD
		#define URLCLOUND "http://cloud.bboqi.com/"
		#define URLTRADE "http://trade.bboqi.com/"
		#define SERVERIP "http://cm.bboqi.com/"
	#else
		#define URLCLOUND "http://api.bboqi.com/"
		#define URLTRADE "http://api.bboqi.com/"	
		#define SERVERIP "http://cm.bboqi.com/"
	#endif
#endif

#define ZHHUIKEY "9F86772CD61F440680D11CB2B9EC3168"

#define MONEY_MAX_COUNT 50000.00
#define MEITUAN

// 当前是请求属于哪个对话框
//#define URL_DEFAULT_DLG -1  // 默认对话框
//#define URL_UPGRDE_DLG_CHECKBTN 1  // 更新对话框
//#define URL_LOGIN_DLG 2  // 登陆对话框
//#define URL_LOGOUT_DLG 3  // 退出登陆对话框
//#define URL_SWIP_CARD_DLG 4 // 刷卡支付对话框
//#define URL_QR_PAY_DLG 5  // 二维码支付对话框
//#define URL_UPLOAD_LOG 6  // 上传日志
//#define URL_UPLOAD_LAST_LOG_EXIT 7 // 退出日志，上传后退出
//#define URL_HANDOVER 8  // 交班对话框，跟退出登陆是同一个
//#define URL_UPLOAD_LAST_LOG_HANDOVER 9 // 交班日志，上传后交班
//#define URL_UPGRDE_DLG_LOGIN 10  // 更新对话框
//#define URL_TRADESTATIC_SHORTCUT 11 // 交易统计  快捷方式
//#define URL_PRINTHANDOVER_STATEMENT 12 // 打印交班对账单
//#define URL_HANDOVER_AUTO 13  // 自动交班
////#define URL_BINDSETTING_BTN 14	// 绑定服务器
//#define URL_SETTING_DLG 15  // Setting Dialog

enum URL_SOURCE_DLG
{
	URL_DEFAULT_DLG, // 默认对话框
	URL_UPGRDE_DLG_LOGIN,  // 更新对话框
	URL_UPGRDE_DLG_CHECKBTN,  //更新对话框
	URL_LOGIN_DLG, // 登陆对话框
	URL_LOGOUT_DLG,  // 退出登陆对话框
	URL_SWIP_CARD_DLG, // 刷卡支付对话框
	URL_QR_PAY_DLG,  // 二维码支付对话框
	URL_UPLOAD_LOG,  // 上传日志
	URL_UPLOAD_LAST_LOG_EXIT, // 退出日志，上传后退出
	URL_HANDOVER,  // 交班对话框，跟退出登陆是同一个
	URL_UPLOAD_LAST_LOG_HANDOVER, // 交班日志，上传后交班
	URL_TRADESTATIC_SHORTCUT, // 交易统计  快捷方式
	URL_TRADESTATIC_MAINWIDGET,  // 交易统计 主界面里面
	URL_TRADEINFODETAIL, // 交易详情
	URL_PRINTHANDOVER_STATEMENT, // 打印交班对账单
	URL_HANDOVER_AUTO,  // 自动交班
	URL_SETTING_DLG,  // Setting Dialog
	URL_VERIFICATION, // verification
//#ifdef MEITUAN
	URL_MEITUAN_VERIFICATION_HISTORY, // 美团券核销历史
	URL_MT_VERIFICATION_CONSUME_PREPARE, // 美团验券准备
	URL_MT_CONSUME_CANCEL, // 美团验券撤销
	URL_MT_CONSUME_RESULTDLG_CANCEL, // 美团验券撤销
//#endif
	URL_MEMBER_CRM, // member crm
	URL_MEMBER_CRM_DETAIL, // member detail
	URL_MEMBER_STOREHISTORY, // store value history
	URL_CLIENT_INFO, // get client infomation after pay success

	URL_RECORD_SOCKETINIT,
	URL_RECORD_GETQR,
	URL_RECORD_STARTOCR,
	URL_RECORD_PAYPUSH,
	URL_RECORE_LOGIN_MEMORY,
};


// 日志级别
#define LOG_ERROR "error"
#define LOG_INFO "info"
#define LOG_DEBUG "debug"
// 日志模块
#define LOG_MEMEORY "M_MEMORY"
#define LOG_LOGIN "M_LOGIN_IN"
#define LOG_LOGOUT "M_LOGIN_OUT"
#define LOG_SOCKET "M_SOCKET"
#define LOG_PAY1 "M_SCANCODEPAY"
#define LOG_PAY2 "M_QRCODEPAY"
#define LOG_PUSHPAYRES "M_PSUHPAYMSG"
#define LOG_RESTARTOCR "M_RESTARTOCR"


#define SETTINGBINDAPI "api/dev/kx/install/v1"  // 绑定
#define SETTINGCOMMITAPI "api/cashdesk/list/v1"  // 提交
#define TRADEINFOQUESTAPI "api/order/list/v1"     // 交易查询
#define TRADEINFOSTATICAPI "api/order/statis/v1"   // 交易统计
#define TRADEINFODETAILAPI "api/order/info/v1"    // 交易详情
#define TRADEREFUNDAPI "api/payway/applyrefund/v1"   // 退款
#define TRADEREFUNDPWDAPI "api/user/validatempw/v1"  // 交易退款密码验证
#define TRADINFODETAILPRINT "api/print/order/printpayoffice/v1"  // 打印存根
#define VERIFICATIONCANCELAPI "api/wechat/card/consume/v1"  // 卡券核销
#define VERIFICATIONHISTORY "api/wechatcard/cardconsumelog/v1"     // 卡券核销历史
#define CARDPAYUNIFIEDORDER "api/payway/unifiedorder/v1"  // 统一下单
#define CARDPAYMICROPAYAPI "api/payway/micropay/v1"  //刷卡支付
#define USERLOGINAPI "api/user/login/v1"    // 用户登陆
#define PRINTSTATEMENTAPI "api/dev/get/statement/v1"  // 打印对账单
#define USERLOGOUTAPI "api/user/logout/v1"  // 用户退出
#define SOFTUPGRADE "api/softversion/upsoft/uppcsoft/v1" // 软件升级
#define QRSUBMITPAYAPI "api/payway/qrcodepay/v1"  // 提交二维码支付
#define UPLOADLOGAPI "api/log/kxdevrun/v1"  // 上传日志
#define MEMBERSTATICAPI "api/wechatmember/memberstatis/v1"  // 会员统计
#define MEMBERLISTAPI "api/wechatmember/memberlist/v1"  // 会员列表
#define MEMBERDETAILAPI "api/wechatmember/memberinfo/v1"    // 会员详情
#define MEMBERBONUSAPI "api/wechatmember/updateBonus/v1"   // 会员卡积分修改
#define MEMBERSTOREVALUESTATICAPI "api/wechatmember/memberbalancestatis/v1"  // 储值统计
#define MEMBERSTOREVALUELISTCAPI "api/wechatmember/memberbalancerecord/v1" // 储值列表
#define PAYRESCLIENTINFO "api/customer/paycustomerinfo/v1" // client infomation
//#ifdef MEITUAN
#define MEITUANVERIFICATIONHISTORY "api/mtcoupon/consumehistory/v1"  // 美团核销历史
#define MEITUANCONSUMEPREPARE "api/mtcoupon/consumeprepare/v1"  // 美团验券准备
#define MEITUANCONSUMEEXECUTE "api/mtcoupon/consume/v1"  // 执行美团验券
#define MEITUANCONSUMECANCEL "api/mtcoupon/consumecancel/v1"  // 美团撤销验券
//#endif

#define MACADDRLEN 13



#endif