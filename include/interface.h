/**
 *author : wanghe
 *
 *date   : 2012-8-20
 *
 ***/
/*
 * Prevent multiple inclusion...
 */

# ifndef _INTERFACE_H
# define _INTERFACE_H

/*
 * Include necessary headers...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h> 
#include <mxml.h>
#include <curl/curl.h> 

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "cgi.h"
#include "envfile.h"
#include "dfgap_cfg_act.h"
//#include "prog/log/cfg2xml.h"
#include"misc.h"
#include"packet.h"
#include"mempool.h"
#include"common.h"
#include<time.h> 


/*
 * Constants...
 */

/*
 * Constants...
 */

/*
 * Data types...
 */

#define MAXSIZE 4096
#define SMSADDRPORT_LENGTH 10
#define DEVINFO_ABNORMAL_LENGTH 50
#define ADMINLOG_LENGTH 10
#define DESC_LENGTH 50
#define REMARK_LENGTH 50
#define SRCLIST_LENGTH 50
#define MAXSIZE_RULE  160000
#define CENTER_URL "deviceURI/request"

typedef struct DATA_s     // request header message. 
{
	char direct[2]; /* attribute ...*/
	char cmd[5]; /* attribute ...*/
	char subcmd[2]; /* attribute ...*/
	char recode[3]; /*response code*/

	char src_sms_id[50]; /*4.1*/
	char dest_sms_id[50];
	char dev_id[50];        // dev id


	int smsaddrport_ip;   // 非直接管理安管IP
	int smsaddrport_port;  // 非直接管理安管port

	size_t heartbeatinterval;

	char id[50];
	char ip[20];
	int port;

	char smsip[20];
	char ccip[20];
	char smsid[50];

	/* 4.5 基本信息设置 */
	char smsaddr[20] ;
	size_t smsport;
	char deviceid[50] ; /*deviceid , 即为dev id*/

	char new_ccenter_addrip[20]; /*4.4*/

	char errordesc[1024]; /* return back error*/

} Data;

/*   获取基本信息 */
/*可以有多个smsaddrport，表示不止一个非直接管理的安管系统*/

typedef struct SMSaddrport_t
{
	char ip[16];
	size_t port;
	char sms_id[50];
} SMSAddrport;

typedef struct limit_t /*事件流量限制（每秒事件条数）*/
{
	char type[30];
	char level[2];
} Limit;

typedef struct reportevent_t  /*上报事件*/
{
	char enable[10];
	char rawdata[10];
}Reportevent;

typedef struct SysInfo_t
{
	char serialnum[50]; /*设备序列号*/
	char hardwareversion[50]; /*硬件版本号*/
	char softwareversion[50]; /*软件版本号*/
	char manufacturer[50]; /*厂商名称*/
	char managerip[20]; /*设备管理口的IP地址*/
	char managermask[16]; /*设备管理口子网掩码*/
	char managergateway[16]; /*设备管理口网关*/
	char managername[50]; /*设备管理口名称(例如：eth0)*/
	char devicetype[50]; /*设备类型见3.2.2 */
	char validrate[100]; //流经设备数据量bytes

} SysInfo;

typedef struct SysInt_t
{
	char smsaddr[16]; /*安全管理系统IP地址*/
	size_t smsport; /*安全管理系统监听端口号*/
	char smsid[50]; /*所属安管id*/
	char deviceid[22]; /*设备本身标识*/
	char devicecode[10]; /*厂家设备型号编码，该字段需要设备厂商在设备初始化时填入*/
	size_t localport; /*安全设备接收安管命令的监听端口*/
	size_t heartbeatinterval; /*状态上报心跳间隔，单位：秒*/
	char devicename[50]; /*设备名称描述*/
//	Limit limit;
	Reportevent reportevent;
	SMSAddrport smsaddrport[SMSADDRPORT_LENGTH]; /*可以有多个smsaddrport，表示不止一个非直接管理的安管系统*/
	int smsaddrportlength;

} SysInt;



typedef struct DeviceInfo_t
{
	char id[30];  					// dev 的属性
	char src_sms_id[50];// dev 的属性
	SysInt sysint;
	SysInfo sysinfo;
}DeviceInfo,BaseInfo;

/*******状态上报报文*********/

typedef struct StatusReport_sr
{
	char event_type[50];
	char dev_id[50]; /*设备唯一标识，一个UUID字符串*/
	char ip[16]; /* 设备IP*/
	char mac[20]; /*设备mac*/
	char status[50]; /*设备工作状态(备无状态标识，在发送此字段时默认发送正常标识*/
	char deviceabnormalreason[DEVINFO_ABNORMAL_LENGTH]; /*用于说明设备不正常的原因*/
	size_t cpuusage; /*数字0~100， CPU当前使用率*/
	size_t memusage; /*使用量/总量， 内存使用率， 单位KB*/
	size_t diskusage; /*使用量/总量, 硬盘空间使用率， 单位MB */
    char validrate[100] ;
} StatusReport;

/******策略修改事件和日志事件上报*******/

/*日志类型*/

typedef enum LogType_s
{

	USER_MODIFY_RULE = 1,            //用户修改策略
	USER_ADD_RULE = 2,            //用户添加策略
	USER_DEL_RULE = 3,            //用户删除策略
	USER_IMPORT_RULE = 4	           //用户导入策略

} LogType;

typedef struct Adminlog_s
{
	char event_id[60]; 								//uuuid 随机生成,事件的唯一标识，uuid字符串
	char time[20];	                      // 操作时间，2012-04-09 12:00:00
	char user[50];                       // 操作用户
	char log_type[3];						            // 日志类型
	char description[200];              // 事件描述（长度待定）
	char operation[200];                // 具体操作
	char remark[200];                   // 备注
	//struct Adminlog_s *next;                // 日后优化时， 将之改成链表

} Adminlog;

/*  策略修改事件和日志事件报文 */
typedef struct RuleLog_s
{
	char event_type[10];           //事件类型：标识这是一个状态的报文
	char dev_id[50];                //设备唯一标识，一个UUID字符串
	Adminlog adminlog[ADMINLOG_LENGTH];
	int loglength;
} RuleLog;

/*旁路、策略违规与空策略和离线或异常事件上报(隔离板单闸只提供策略违规的事件上报)v  */

typedef struct Info_s
{
	char channel[50];
	char ruleName[50];
	char description[DESC_LENGTH];

} Info;

typedef struct RuleError_s
{

	char event_type[50];
	char dev_id[50];
	char time[30];
	char event_id[50];
	char infoType[4];
	char remark[REMARK_LENGTH];
	Info info;

} RuleError;

/**************包过滤策略获取 ****************/

typedef struct Src_Dst
{
	char hostName[50];
	char ipAddr[16];
	char mask[16];
	char mac[30];
	char natIp[16];
	char remark[REMARK_LENGTH];
	char gateway[4];            // 是否为网关， G是, H否
	char macList[512];          // 字符串，0~24个MAC地址  ，用逗号隔开

} Src, Dst;

typedef struct Rule_s
{
	char channel[2];
	char id[4];
	char protocol[20];
	int port;
	char flag[2];
	Src srcList[SRCLIST_LENGTH];
	size_t srcListLength;
	Dst dstList[SRCLIST_LENGTH];
	size_t dstListLength;

} Rule;

//------------------------------

/*
 * C++ support...
 */

#  ifdef __cplusplus
extern "C"
{
#  endif /* __cplusplus */

/*
 * Function  Prototypes...
 */

extern Data dopost(char *reqstr); // this will saved rquest information to Data Structor.

extern void println(mxml_node_t *node); //  // print specifying node or its child node. 

extern void print_text(mxml_node_t *node); // print specifying node value what is text type.

extern void print_integer(mxml_node_t *node); // print specifying node value what is integer type.

extern int saveToFile(char *xmlfilepath, char *xmldata); // save data to xml file.

extern size_t writer(void *buff, size_t size, size_t nmem, void *usrp);

extern int postUrl(const char *url, char *xmlStr); /*发送请求信息*/

extern int disp_baseinfo(BaseInfo *bi); /* 显示设备基本信息*/

extern int BaseInfo_to_XML(BaseInfo *bi, char *xmldata); /*基本信息转换成xml数据 */

extern int StatusReport_to_XML(StatusReport *sr, char *xmldata); /* 状态报文转换成xml数据,实现在xmloper.c 中*/

extern int RuleLog_to_XML(RuleLog *rl, char *xmldata);      // 策略修改和日志事件报文转换至xml

extern int RuleError_to_XML(RuleError *re, char *xmldata); 			//策略违规上报报文

extern int Rule_to_XML(Rule *rl, char *xmldata, int rulLen,Rule *r2,int rulLen2);  //包过滤策略获取转换xml 字符串

extern int parse_deviceinfoxml(char *filepath, BaseInfo *bi); // parse information to BaseInfo structor.

extern int LoadXMLFile(char *filepath, char *xmldata); 		// 加载xml文件， 并保存成字符串

extern int ruleshow(char* xmlStr, char *src, char *dst, char *rule, char *src2, char *dst2, char *rule2); /* 包过滤策略获策略列表， 组装成xml*/

extern int getlocalhostip(char *ip);

extern int getlocalmask(char *mask);

extern void getSysTime(char *tm); /* 获取当前系统时间*/

extern void getSysTime1(char *tm) ;

extern int getUUID(char *uuid );

extern void getRate(char *validrate) ;

extern void getBytes(char *res, char *bytes);

extern void stopReportSh() ;

extern void runReportSh(int timeSec);

extern int getRandom();
#endif
