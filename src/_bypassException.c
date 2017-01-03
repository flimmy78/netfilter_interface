/***********************************
 *
 * author : wanghe
 *
 * description : by pass exceiption report
 *
 * data: 2012-10-24
 *
 *
 ****************************************/
#include "interface.h"



void changeStatCode(int stat){
	//320110000000000000000000  ------24
	//0	fail error
	//1 suc normal
	FILE *fp = fopen("/home/jiekou/src/statuscode.txt","r");
	if(fp == NULL)
	{
		return ;
	}
	char line[30]={0};
	fgets(line, 25, fp);
	fclose(fp);

	int i;

	if(stat == 0){
		line[4] = '2';
		line[6] = '1';
	}else {
		if(line[7] == '0')
			line[4] = '1';
		line[6] = '0';
	}
//	for(i = 0; i < 24; i++){
//		if(i != 6)
//			continue;
//		if(stat != 0)
//			line[i] = '0';
//		else
//			line[i] = '1';
//	}

	printf("****************line is [%s]\n", line);

	fp = fopen("/home/jiekou/src/statuscode.txt","w");
	if(fp == NULL)
	{
		return ;
	}
	fputs(line, fp);

	fclose(fp);

}

/**************************************
 *
 * description : get bypass(net connect) status .
 *
 *3 -- interface link up
 *1 -- interface eth2 link down
 *2 -- interface eth1 link down
 *0 -- interface all link down.
 *
 *
 */
int convertInterface(int bEth1Stat, int bEth2Stat){
	//eth1:1/0
	//eth2:2/0

	int aEth1Stat, aEth2Stat;
	FILE *fp = fopen("/tmp/sider_a_eth.txt","r");
	if(NULL == fp)
		return 0;
	char line[30]={0};
	while(NULL != fgets(line, 30, fp)){
		if(strncmp(line,"eth1=",strlen("eth1=")) == 0){
			strtok(line,"=");
			if(strncmp(strtok(NULL,"="),"on",strlen("on")) == 0){
				aEth1Stat = 1;
			}else{
				aEth1Stat = 0;
			}
		}else if(strncmp(line,"eth2=",strlen("eth2=")) == 0){
			strtok(line,"=");
			if(strncmp(strtok(NULL,"="),"on",strlen("on")) == 0){
				aEth2Stat = 1;
			}else{
				aEth2Stat = 0;
			}
		}
	}
	fclose(fp);

	printf("aEth1Stat is [%d],aEth2Stat is [%d],bEth1Stat is [%d],bEth1Stat is [%d]\n",aEth1Stat, aEth2Stat, bEth1Stat, bEth2Stat);
	if(aEth1Stat == 1 && aEth2Stat == 1 && bEth1Stat == 1 && bEth2Stat == 2)
		return 3;
	else if(aEth1Stat == 1 && aEth2Stat == 1 && bEth1Stat == 1 && bEth2Stat != 2)
		return 1;
	else if(aEth1Stat == 1 && aEth2Stat == 1 && bEth1Stat != 1 && bEth2Stat == 2)
		return 2;
	else if(aEth1Stat == 1 && aEth2Stat == 1 && bEth1Stat != 1 && bEth2Stat != 2)
		return 0;


	else if(aEth1Stat == 1 && aEth2Stat != 1 && bEth1Stat == 1 && bEth2Stat == 2)
		return 1;
	else if(aEth1Stat == 1 && aEth2Stat != 1 && bEth1Stat == 1 && bEth2Stat != 2)
		return 3;
	else if(aEth1Stat == 1 && aEth2Stat != 1 && bEth1Stat != 1 && bEth2Stat == 2)
		return 0;
	else if(aEth1Stat == 1 && aEth2Stat != 1 && bEth1Stat != 1 && bEth2Stat != 2)
		return 2;

	else if(aEth1Stat != 1 && aEth2Stat == 1 && bEth1Stat == 1 && bEth2Stat == 2)
		return 2;
	else if(aEth1Stat != 1 && aEth2Stat == 1 && bEth1Stat == 1 && bEth2Stat != 2)
		return 0;
	else if(aEth1Stat != 1 && aEth2Stat == 1 && bEth1Stat != 1 && bEth2Stat == 2)
		return 3;
	else if(aEth1Stat != 1 && aEth2Stat == 1 && bEth1Stat != 1 && bEth2Stat != 2)
		return 1;

	else if(aEth1Stat != 1 && aEth2Stat != 1 && bEth1Stat == 1 && bEth2Stat == 2)
		return 0;
	else if(aEth1Stat != 1 && aEth2Stat != 1 && bEth1Stat == 1 && bEth2Stat != 2)
		return 2;
	else if(aEth1Stat != 1 && aEth2Stat != 1 && bEth1Stat != 1 && bEth2Stat == 2)
		return 1;
	else if(aEth1Stat != 1 && aEth2Stat != 1 && bEth1Stat != 1 && bEth2Stat != 2)
		return 0;

	return 0;
}

/**************************************
 *
 * description : get bypass(net connect) status .
 *
 *3 -- interface link up
 *1 -- interface eth2 link down
 *2 -- interface eth1 link down
 *0 -- interface all link down.
 *
 *
 */

int get_bypass_status()
{
	int returnInt = 1;
	int returnEth1 = 0;
	int returnEth2 = 0;
	char res[200] =
	{ 0 };
	char cmd[200] =
	{ 0 };

	sprintf(cmd, "ifconfig eth1 |grep 'RUNNING'");
	FILE *fp = popen(cmd, "r");
	fgets(res, 200, fp);
	pclose(fp);
	if (strlen(res) != 0)
	{
		returnEth1 = 1;
	}

	strcpy(res,"");
	sprintf(cmd, "ifconfig eth2 |grep 'RUNNING'");
	fp = popen(cmd, "r");
	fgets(res, 200, fp);
	pclose(fp);
	if (strlen(res) != 0)
	{
		returnEth2 = 2;
	}

	//translate A&B interface status
	int tmp = convertInterface(returnEth1, returnEth2);

	printf("tmp is [%d]\n",tmp);
	if(tmp == 3)
		returnInt = 1;
	else
		returnInt = 0;

	changeStatCode(returnInt);

//	return returnEth1+returnEth2;
	return tmp;
}

/*********************
 *
 *
 *author      :  wanghe
 * description:
 * check bypass and do it.
 * gerneral a string of xml.
 *
 *netcard : net adapter card .
 *
 *xmldata : save string xml to array of xmldata.
 *date : 2012-10-24
 *
 *
 **************************/
int doByPassException(BaseInfo *bi, char *netcard, char *xmldata)
{
	int status;
	char *channelName = malloc(100);
	char *ruleName = malloc(30);
	char des[200];
	char infoType[3];
	char channel[20];

	RuleError *re;
	re = (RuleError *) malloc(sizeof(RuleError));

	status = get_bypass_status();
	if (status != 3)
	{
		if (status == 2)
		{
			strcpy(channel, "通道1");
			strcpy(des, "通道1 旁路");
		}else if (status == 1)
		{
			strcpy(channel, "通道2");
			strcpy(des, "通道2 旁路");
		}else if(status == 0){
			strcpy(channel, "通道1、通道2");
			strcpy(des, "通道1、通道2 旁路");
		}

		if (parse_deviceinfoxml(DEVICEINFO, bi) < 0)
		{
			free(bi);
			free(re);
			return -1;
		}

		strcpy(infoType, "04");
		strcpy(re->infoType, infoType);
		strcpy(re->dev_id, bi->id);

		//取得事件类型, bypass
		strcpy(re->event_type, "3203");

		// 异常事件id
		char uuid[50];
		getUUID(uuid);
		strcpy(re->event_id, uuid);

		// 获取异常事件发生的本地时间
		char timestr[20];
		getSysTime(timestr);
		strcpy(re->time, timestr);

		// 获取异常事件的通道号
		strcpy(re->info.channel, channel);
		// 获取异常事件的规则名称
		sprintf(ruleName, " 连接错误");
		strcpy(re->info.ruleName, ruleName);

		// 获取异常事件的详细描述信息 e
		strcpy(re->info.description, des);

		// 备注
		strcpy(re->remark, "");

		if (RuleError_to_XML(re, xmldata) < 0)
		{
			free(bi);
			free(re);
			return -1;
		}

		return 1; // it is bypass .
	}
	return 0;
}

int main()
{
	int status;
	RuleError *re;
	char xmldata[MAXSIZE];
	char url[50];
	int flag1 = 0, flag2 = 0; // flag is 0 if it is not  bypass.

	re = (RuleError *) malloc(sizeof(RuleError));
	BaseInfo *bi;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (parse_deviceinfoxml(DEVICEINFO, bi) < 0)
	{
		free(bi);
		free(re);
		return -1;
	}

	if (getuid() != 0)
	{
		fprintf(stderr, "Netlink Status Check Need Root Power.\n");
		free(re);
		free(bi);
		return 1;
	}
	sprintf(url, "https://%s:%d/%s", bi->sysint.smsaddr, bi->sysint.smsport, CENTER_URL);
	puts(url);
//	while (1)
//	{
		//
		status = doByPassException(bi, "eth1", xmldata);
		if (status)
		{
			postUrl(url, xmldata);
			flag1 = 1;
		}
		else
		{
			flag1 = 0;
		}

		puts(xmldata);
//		sleep(3);
//		status = doByPassException(bi, "eth2", xmldata);
//		if (status)
//		{
//			postUrl(url, xmldata);
//			flag2 = 1;
//		}
//		else
//		{
//			flag2 = 0;
//		}
//		puts(xmldata);
//		sleep(300);
//	}
	free(bi);
	free(re);

	return 0;

}
