/*
 * _checkException.c
 *
 *  Created on: 2012-9-8
 *      Author: Administrator
 */

#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "envfile.h"
#include "dfgap_cfg_act.h"

#define EVENT_TYPE_MOD "mod"
#define EVENT_TYPE_ADD "add"
#define EVENT_TYPE_IMP "imp"

/***
 *
 * 	argv[1]: 策略修改类型
 *		mod		用户策略修改
 *		add		用户策略添加
 *		imp		用户策略导入
 *
 *	argv[2]：传入的策略信息字符串，需要由此字符串转换成XML并发出
 *		，在修改类型为"imp"时，不对此字符串进行解析
 *
 *
 */
int main(int argc, char **argv) {
//	if(2 > argc)
//		return 0;

	//对应<channel>节点
	char *channelName = malloc(20);
	//对应<ruleName>节点
	char *ruleName = malloc(30);
	//对应<description>节点
	char *des = malloc(100);

	int flag = -1;

	/*
	 * mod		0RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
	 * add		0RULE;0;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
	 *
	 * import
	 */
		flag = getNodesInfo(channelName, ruleName, des);
//	if(!strcmp(EVENT_TYPE_IMP, argv[1])){
//	}else{
////		getNodeInfo("0RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|", channelName, ruleName, des);
//		flag = getNodeInfo(argv[2], channelName, ruleName, des);
//	}

	printf(" 通道名称： %s\n", channelName);
	printf("规则名： is %s\n", ruleName);
	printf("描述信息： %s\n", des);

//	getRate();

	BaseInfo *bi ;
	bi = malloc(sizeof(BaseInfo) ) ;
	RuleError *re ;
	re = malloc(sizeof(RuleError));
	char infoType[5];
	char channel[30];

	strcpy(infoType, "01");
		strcpy(channel, channelName);

		if (parse_deviceinfoxml(DEVICEINFO, bi) < 0)
		{
			free(bi);
			free(re);
			return -1;
		}

		strcpy(re->infoType, infoType);
		strcpy(re->dev_id, bi->id);

		//取得事件类型
		strcpy(re->event_type, "3204");

		// 异常事件id
		char uuid[50];
		getUUID(uuid);
		printf("uuid=======================[%s]\n", uuid);
		strcpy(re->event_id, uuid);

		// 获取异常事件发生的本地时间
		char timestr[20];
		getSysTime(timestr);
		strcpy(re->time, timestr);

		// 获取异常事件的通道号
		strcpy(re->info.channel, channel);
		// 获取异常事件的规则名称
		strcpy(re->info.ruleName, ruleName);

		// 获取异常事件的详细描述信息 e
		strcpy(re->info.description, des);

		// 备注
		strcpy(re->remark, "");

        char xmldata[MAXSIZE];
		if (RuleError_to_XML(re, xmldata) < 0)
		{
			free(bi);
			free(re);
			return -1;
		}
		printf(xmldata);
		char url[50] =
		{ 0 };
		sprintf(url, "https://%s:%d/%s", bi->sysint.smsaddr,bi->sysint.smsport, CENTER_URL);

		if(flag)
		{
			postUrl(url, xmldata);
		}

	return 0;
}

void changeStatCode(int stat){
	//320110000000000000000000  ------24
	//1	FAIL
	//0 SUC
	FILE *fp = fopen("/home/jiekou/src/statuscode.txt", "r");
	if(fp == NULL)
	{
		printf("fail to open the file1");
		return ;
	}
	char line[30]={0};
	fgets(line, 25, fp);
	fclose(fp);

	int i;
	if(stat == 0){
		if(line[6] == '0')
			line[4] = '1';
		line[7] = '0';
	}else {
		line[4] = '2';
		line[7] = '1';
	}

	printf("****************line is [%s]\n", line);

	fp = fopen("/home/jiekou/src/statuscode.txt","w");
	if(fp == NULL)
		{
			printf("fail to open the file2");
			return ;
		}
	fputs(line, fp);

	fclose(fp);

}

/***
 * 	校验所有端口信息， 并对channelName、ruleName、des进行赋值
 *
 *
 *
 * 	return
 * 		0:	校验成功，端口信息存在
 * 		1:	校验失败，端口信息不存在
 */

int getNodesInfo(char *channelName, char *ruleName, char *des){

	int returnInt = 1;
	init_dfgap_cfg ();
	get_dfgap_cfg ("/usr/dfgap/dfgap-cfg");

	int ruleLen;//规则结构体数组长度
	struct rule_struct *returnPri;//规则结构体数组头指针

	int i;
	returnPri = getRuleStruct(CHANNEL0_RULE);
	ruleLen = getRuleStructLen(CHANNEL0_RULE);

	int channel1Flag = 0;
	char *portList1 = malloc(200);
	for(i=0 ; i<ruleLen ; i++ ){
		char rulePort[64] = {};
		sprintf(rulePort,"%d",returnPri->dport);
		if(checkPortInFile(rulePort) != 0){
			//not in file
			channel1Flag = 1;
			strcat(portList1,",");
			strcat(portList1, rulePort);
		}
		returnPri++;
	}
	printf("端口列表1： [%s]\n",portList1);
	returnPri = getRuleStruct(CHANNEL1_RULE);
	ruleLen = getRuleStructLen(CHANNEL1_RULE);
	int channel2Flag = 0;
	char *portList2 = malloc(200);
	for(i=0 ; i<ruleLen ; i++ ){
		char rulePort[64] = {};
		sprintf(rulePort,"%d",returnPri->dport);
		if(checkPortInFile(rulePort) != 0){
			//not in file
			channel2Flag = 1;
			strcat(portList2,",");
			strcat(portList2, rulePort);
		}
		returnPri++;
	}
	printf("端口列表2：[%s]\n",portList2);

	if(channel1Flag != 0 && channel2Flag != 0){
		strcpy(channelName, ",通道2");
		strcpy(ruleName,"多规则");
		strcpy(des,"通道1非法端口：");
		strcat(des, portList1);
		strcat(des, ";通道2非法端口 :");
		strcat(des, portList2);
	}else if(channel1Flag != 0){
		strcpy(channelName, "通道1");
		strcpy(ruleName,"通道1规则");
		strcpy(des,"非法端口 :");
		strcat(des, portList1);
	}else if(channel2Flag != 0){
		strcpy(channelName, "通道2");
		strcpy(ruleName,"通道2规则");
		strcpy(des,"非法端口 :");
		strcat(des, portList2);
	}else{
		returnInt = 0;
	}

	changeStatCode(returnInt);

	printf(" 通道名称： is [%s]\n",channelName);
	printf(" 规则名称 ：is [%s]\n",ruleName);
	printf(" 描述信息： [%s]\n",des);
	return returnInt;
}

/***
 * 	校验端口信息， 并对channelName、ruleName、des进行赋值
 *
 * 	param
 * 		data
 * 			0RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|
 *
 * 	return
 * 		0:	校验成功，端口信息存在
 * 		1:	校验失败，端口信息不存在
 */
int getNodeInfo(char *data, char *channelName, char *ruleName, char *des){
	//0;RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|

	char tmp[50] = {0};
	char channelType[10]={0};
	char dealStr[20]={0};
	char liftStr[50]={0};
	char protStr[50]={0};
	char portStr[50]={0};
	char sStr[20] = {0};
	char dStr[20] = {0};

	strcpy(tmp, data);

	strcpy(channelType,strtok (tmp, ";"));//读取通道数
	strtok(NULL,";");
	strcpy(dealStr, strtok(NULL,";"));//读取操作类型 0为添加 1为修改
	strcpy(liftStr, strtok(NULL,";"));

	printf("litfStr is [%s]\n", liftStr);
	strcpy(sStr, strtok(liftStr, "|"));
	strcpy(dStr, strtok(NULL, "|"));
	strcpy(protStr, strtok(NULL,"|"));
	strcpy(portStr, strtok(NULL,"|"));

	printf("protStr is [%s]\n", protStr);
	printf("portStr is [%s]\n", portStr);
	printf("sStr is [%s]\n", sStr);
	printf("dStr is [%s]\n", dStr);

	if(checkPortInFile(portStr) == 0)
		return 0;

	strcpy(channelName, channelType);
	strcpy(ruleName, sStr);
	strcat(ruleName, ",");
	strcat(ruleName, dStr);

	strcpy(des, "策略修改为错误端口");

	return 1 ;
}

/***
 * return 	1:	not in FILE
 * 			0:	in FILE
 */
int checkPortInFile(char *portStr){
	int returnInt = 1;
	FILE *fp = fopen("/usr/dfgap/legalPorts.txt","r");
	char line[1024] = {0};
	while(NULL != fgets(line, 1024, fp)){
		if(!strncmp(portStr, line, strlen(line)-1) && strlen(portStr) == (strlen(line)-1)){
			returnInt = 0;
			break;
		}
	}
	fclose(fp);
	return returnInt;
}

