/*
 * _checkPolicy.c
 *
 *  Created on: 2012-9-8
 *      Author: Administrator
 */
#include <stdio.h>
#include"interface.h"

#define EVENT_TYPE_MOD "mod"
#define EVENT_TYPE_ADD "add"
#define EVENT_TYPE_DEL "del"
#define EVENT_TYPE_IMP "imp"

#define CHANNEL0_SRC "0SRC_MACT"
#define CHANNEL0_DST "0DST_MACT"
#define CHANNEL1_SRC "1SRC_MACT"
#define CHANNEL1_DST "1DST_MACT"
#define CHANNEL0_RULE "0RULE"
#define CHANNEL1_RULE "1RULE"

void getDescription(char *policyStr, char* des) ; 
void getOperation(char *policyStr, char* oper) ;


/***
 *	argv[1]: 策略修改类型
 *		mod		用户策略修改
 *		add		用户策略添加
 *		del		用户策略删除
 *		imp		用户策略导入
 *
 *	argv[2]：传入的策略信息字符串，需要由此字符串转换成XML并发出
 *		，在修改类型为"imp"时，不对此字符串进行解析
 */
int main(int argc, char **argv) {
	
	 
 	if(2 > argc){
		return 0;
	}

	/*
	 * mod  	0;RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
	 * add		0;RULE;0;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
	 * del		0;RULE;2;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
	 *
	 * import
	 */
	char des[200] = {0};
	char oper[200] = {0};
	RuleLog *rl; 
	rl = (RuleLog *) malloc(sizeof(RuleLog)); 
	
	BaseInfo *bi; 
	bi = (BaseInfo *) malloc(sizeof(BaseInfo)); 
	

	if(!strcmp(EVENT_TYPE_MOD, argv[1]))  // 修改事件
		{
				// 取得日志事件类型
			strcpy(rl->adminlog[0].log_type, "01");
		}
		
	if(!strcmp(EVENT_TYPE_ADD, argv[1]))  // 添加事件
		{
				// 取得日志事件类型
			strcpy(rl->adminlog[0].log_type, "02");
		}
		
	if(!strcmp(EVENT_TYPE_DEL, argv[1]))  // 删除事件
		{
				// 取得日志事件类型
			strcpy(rl->adminlog[0].log_type, "03");
		}	
	 
		
		/*去配置文件拿设备id,（deviceinfo.xml)*/
	 	 
		if( parse_deviceinfoxml(DEVICEINFO, bi) < 0)
			{
				free(bi); 
				free(rl); 
				perror("error"); 
				return -1; 
			} 
		  strcpy(rl->dev_id, bi->id);
		//取得事件类型
		strcpy(rl->event_type, "3202");
		
		// 事件id
		char uuid[50];
		getUUID(uuid);
		strcpy(rl->adminlog[0].event_id, uuid);
		 
		// 获取事件发生的本地时间
		char timestr[20];
		getSysTime(timestr); 
		strcpy(rl->adminlog[0].time, timestr); 
		
		// 取得当前事件操作用户
		strcpy(rl->adminlog[0].user, "admin"); 
		
		// 获取事件的描述信息
		getDescription(argv[2], des);
		//getDescription("0;RULE;2;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|", des);  
			strcpy(rl->adminlog[0].description, des); 
		
		// 获取事件的打操作信息
		getOperation(argv[2], oper);
		//getOperation("0;RULE;2;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|", oper);
		strcpy(rl->adminlog[0].operation, oper);
		
		// 日志信息备注
		strcpy(rl->adminlog[0].remark, "");
		
		rl->loglength=1;  // 目前只支持一个事件的上报
	 
	 
	 
	if(!strcmp(EVENT_TYPE_IMP, argv[1])){  // 导入事件
		printf("this is  an import event.\n"); 
	}
	 
	
	char xmldata[MAXSIZE];
	RuleLog_to_XML(rl, xmldata);
	printf(xmldata);
	char url[50];
	int port ;
//strcpy(ip, bi->sysint.smsaddr); 
	sprintf(url, "https://%s:%d/%s", bi->sysint.smsaddr, bi->sysint.smsport,  CENTER_URL);
	puts(url);
	postUrl(url, xmldata); 
	free(rl); 
	free(bi); 
	return 0;
}

 
/***
 *
 */
void getDescription(char *policyStr, char* des){
	char tmp[50] = {0};
	char channel[10]={0};
	char type[10]={0};
	char dealStr[20]={0};
	char *channelType;
	strcpy(tmp, policyStr);

	strcpy(channel,strtok (tmp, ";"));//读取通道数
	strcpy(type,strtok (NULL, ";"));//读取类型
	strcpy(dealStr, strtok(NULL,";"));//读取操作类型 0为添加 1为修改 2为删除

	channelType = strcat(channel,type);

	strcpy(des, "");

	if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("0", dealStr)){
//		strcat(des, "添加数据通道1中的规则");
		strcat(des, "添加数据通道1中的规则");
	}else if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("1", dealStr)){
		strcat(des, "修改数据通道1中的规则");
//		strcat(des, "modify data channel1 rule");
	}else if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("2", dealStr)){
		strcat(des, "删除数据通道1中的规则");
//		strcat(des, "delete data channel1 rule");

	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("0", dealStr)){
		strcat(des, "添加数据通道2中的规则");
//		strcat(des, "add data channel2 rule");
	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("1", dealStr)){
		strcat(des, "修改数据通道2中的规则");
//		strcat(des, "modify data channel2 rule");
	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("2", dealStr)){
		strcat(des, "删除数据通道2中的规则");
//		strcat(des, "delete data channel2 rule");

	}else{
	}

}



void getOperation(char *policyStr, char* oper){
	char tmp[50] = {0};
	char channel[10]={0};
	char type[10]={0};
	char dealStr[20]={0};
	char data[50] = {0};
	char *channelType;

	strcpy(tmp, policyStr);

	strcpy(channel,strtok (tmp, ";"));//读取通道数
	strcpy(type,strtok (NULL, ";"));//读取类型
	strcpy(dealStr, strtok(NULL,";"));//读取操作类型 0为添加 1为修改
	strcpy(data, strtok(NULL,";"));//读取具体数据

	channelType = strcat(channel,type);

	strcpy(oper, "");

	if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("0", dealStr)){
		strcat(oper, "添加数据通道1中的规则");
//		strcat(oper, "add data channel1 rule");
	}else if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("1", dealStr)){
		strcat(oper, "修改数据通道1中的规则");
//		strcat(oper, "modify data channel1 rule");
	}else if(!strcmp(CHANNEL0_RULE,channelType) && !strcmp("2", dealStr)){
		strcat(oper, "删除数据通道1中的规则");
//		strcat(oper, "delete data channel1 rule");

	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("0", dealStr)){
		strcat(oper, "添加数据通道2中的规则");
//		strcat(oper, "add data channel2 rule");
	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("1", dealStr)){
		strcat(oper, "修改数据通道2中的规则");
//		strcat(oper, "modify data channel2 rule");
	}else if(!strcmp(CHANNEL1_RULE,channelType) && !strcmp("2", dealStr)){
		strcat(oper, "删除数据通道2中的规则");
//		strcat(oper, "delete data channel2 rule");

	}else{
	}

	strcat(oper, "，规则信息为：");
//	strcat(oper, ",MSG:");
	strcat(oper, data);
}
