/*
 * author : wanghe
 *
 * latest date: 2012-9-26
 *
 * */

#include"bo.h" 

/* get base information */

int get_baseinfo(char *xmlfilepath)
{

//	char xmldata[MAXSIZE];
//	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
//	{
//	//	printf("fail to load xml file. please check xml file path.\n");
//		return 11;   // fail to read safe manager config file.
//	}

	char xmldata[MAXSIZE];
	char validrate[100]={0};
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 21;
	}
	getRate(validrate);
	strcpy(bi->sysinfo.validrate, validrate);
	 BaseInfo_to_XML(bi, xmldata);
	free(bi);
	printf(xmldata);  // response to client.
	return 99;
}

/* add safe device
 * */

int add_safedev(Data ds, char *xmlfilepath)
{

	char xmldata[MAXSIZE];
	mxml_node_t *tree = NULL;
	mxml_node_t *node = NULL;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 21;
	}

	if(!strcmp(bi->id, "0") && !strcmp(bi->sysint.smsaddr, "0"))
	{
		char localip[20];
		getlocalhostip(localip);
		strcpy(bi->sysint.smsid, ds.smsid);
		strcpy(bi->sysint.smsaddr, ds.smsip);
		strcpy(bi->id, ds.id);
		strcpy(bi->sysint.deviceid, ds.id);
		strcpy(bi->sysinfo.managerip, localip ) ;
		bi->sysint.smsport = 8335;

	}
	else
	{
		return -1;
	}

	if (BaseInfo_to_XML(bi, xmldata) < 0)
	{
		free(bi);
		return 21;
	}
	if (saveToFile(xmlfilepath, xmldata) < 0)
	{
		return 21; // fail to save file .
	}

//	printf("\n");
	runPassSh();
//	system("./_bypassException 1>/dev/null 2>/dev/null &");

	runReportSh(bi->sysint.heartbeatinterval) ;
//	system("echo 'come over' > /root/comover");
	free(bi);
//	system("./_bypassException &");
	return 0;

}

/* confirm safe device. */

int confirm_safedev(Data ds, char *xmlfilepath)
{

	char xmldata[MAXSIZE];
	mxml_node_t *tree;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
	{
		free(bi);
		return 32;
	}

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 32;
	}

	if (strcmp(bi->id, "0") == 0)
	{
		return 31;   //
	}

	//printf_value("ds.dest_sms_id",ds.dest_sms_id);
	//printf_value("bi->sysint.smsid",bi->sysint.smsid);
	if (strcmp(ds.dest_sms_id, bi->sysint.smsid) == 0)
	{
		strcpy(bi->id, ds.dev_id);
		strcpy(bi->sysint.deviceid, ds.dev_id);

		if (BaseInfo_to_XML(bi, xmldata) < 0)
		{
			free(bi);
			return 32;
		}

		if (saveToFile(xmlfilepath, xmldata) < 0)
		{

			free(bi);
			return 32;
		}
	}
	else
	{
		free(bi);
		return -3;  //
	}

	free(bi);
	system("./status_report.sh > /root/data");
	return 0; // success ;

}

/* delete safe manager device. */

int del_safedev(Data ds, char *xmlfilepath)
{
	char xmldata[MAXSIZE];
	mxml_node_t *tree;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
	{
		printf("fail to load xml file. please check xml file path.\n");
		free(bi);
		return 42;
	}

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 42;
	}

	if (!strcmp(bi->id, "0"))
	{
		return 41;   // not exist safe information.
	}

	if (!strcmp(ds.dest_sms_id, bi->sysint.smsid) && !strcmp(ds.dev_id, bi->id))
	{
		strcpy(bi->id, "0");
		strcpy(bi->sysint.smsid, "0");
		strcpy(bi->sysint.deviceid, "0");
		strcpy(bi->sysint.smsaddr, "0");
		bi->sysint.smsport = 8335;   // port is null.
		//printf("bi->sysint.heartbeatinterval=%s\n", bi->sysint.heartbeatinterval);
		if (BaseInfo_to_XML(bi, xmldata) < 0)
		{
			free(bi);
			return 42;
		}

		if (saveToFile(xmlfilepath, xmldata) < 0)
		{
			free(bi);
			return 42;
		}
	}
	else
	{
		free(bi);
		return -3; // ����ƽ̨��ϢУ��ʧ��
	}

//	system("killall status_report");
	stopReportSh();
	stopPassSh();
	system("killall sleep 1>/dev/null 2>/dev/null");

//	system("killall _bypassException 1>/dev/null 2>/dev/null");
	return 0;  // success.

}

/* modify IP pointed by safe manager device. */
int modi_safedevip(Data ds, char *xmlfilepath)
{
	char xmldata[MAXSIZE];
	mxml_node_t *tree;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
	{
		free(bi);
		return 52;
	}

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 52;
	}

	if (!strcmp(ds.dest_sms_id, bi->sysint.smsid) && !strcmp(ds.dev_id, bi->id))
	{
		strcpy(bi->sysint.smsaddr, ds.new_ccenter_addrip);
		if (BaseInfo_to_XML(bi, xmldata) < 0)
		{
			free(bi);
			return 52;
		}

		if (saveToFile(xmlfilepath, xmldata) < 0)
		{

			free(bi);
			return 52;
		}
	}
	else
	{
		free(bi);
		return 51; // not exist safe information.
	}

	//system("./status_report.sh");
	return 0;  // success.

}

/* set base information */

int set_baseinfo(Data ds, char *xmlfilepath)
{
	char xmldata[MAXSIZE];
	mxml_node_t *tree;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
	{
	//	printf("fail to load xml file. please check xml file path.\n");
		free(bi);
		return 52;
	}

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 52;
	}

	if (!strncmp(ds.dest_sms_id, bi->sysint.smsid,
					strlen(bi->sysint.smsid)))
	{

		strcpy(bi->src_sms_id, ds.dest_sms_id);
		strcpy(bi->sysint.deviceid, ds.dev_id);
		strcpy(bi->id, ds.dev_id);
		bi->sysint.heartbeatinterval = ds.heartbeatinterval;
		strcpy(bi->sysint.smsaddr, ds.smsaddr);
		bi->sysint.smsport = ds.smsport;

		if (BaseInfo_to_XML(bi, xmldata) < 0)
		{
			free(bi);
			return 52;
		}

		if (saveToFile(xmlfilepath, xmldata) < 0)
		{
		//	printf("Fail to save the file.\n");
			free(bi);
			return 52;
		}
	}
	else
	{
		free(bi);
		return 51; // not exist safe information.
	}

	return 0;  // success.
	//system("./status_report.sh");
}

//
int get_packfilter_rule(Data ds, char *xmlfilepath)
{
	char xmldata[MAXSIZE_RULE];
	mxml_node_t *tree;
	BaseInfo *bi = NULL;
	bi = (BaseInfo *) malloc(sizeof(BaseInfo));

	if (LoadXMLFile(xmlfilepath, xmldata) < 0)
	{
		//printf("fail to load xml file. please check xml file path.\n");
		free(bi);
		return 42;
	}

	if (parse_deviceinfoxml(xmlfilepath, bi) < 0)
	{
		free(bi);
		return 42;
	}

	if (!strcmp(bi->id, "0"))
	{
		return 41;   // not exist safe information.
	}
if (strncmp(ds.dev_id, bi->id, strlen(bi->id)) &&
		strncmp(ds.dest_sms_id, bi->sysint.smsid, strlen(bi->sysint.smsid)))
		{
			return -1;
		}

		char xmlStr[MAXSIZE_RULE];
		char buff[MAXSIZE_RULE];

		char channelType[10];

		strcpy( channelType, CHANNEL0_RULE);
		init_dfgap_cfg();
		get_dfgap_cfg( NetCfgFile);

		ruleshow(xmlStr, CHANNEL0_SRC, CHANNEL0_DST, CHANNEL0_RULE, CHANNEL1_SRC, CHANNEL1_DST, CHANNEL1_RULE);
     	puts(xmlStr);
     	char cmd[50];
//     	sprintf(cmd, "echo '%s' > /root/log", xmlStr);
//     	system(cmd);
		clean_dfgap_cfg();
		return 99;

	}

