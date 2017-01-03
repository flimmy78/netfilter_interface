/*******************
 *
 *author : wanghe
 *date   : 2012-8-22
 *
 ***************** */
/*interface. c */
#include "interface.h"
#include "cgic.h"
#include "envfile.h"
#include "dfgap_cfg_act.h"
#include "prog/log/cfg2xml.h"

int saveToFile(char *xmlfilepath, char *xmldata) //
{
	FILE* fp;

	if ((fp = fopen(xmlfilepath, "wr")) == NULL)
	{
//		puts("fail to open file.\n");
		return -1;
	}
	fprintf(fp, "%s", xmldata);
//	printf("success to save!\n");
	return 0;

}

void println(mxml_node_t *node) // print specifying node or its child node.
{
	char buff[MAXSIZE];
	mxmlSaveString(node, buff, MAXSIZE, MXML_NO_CALLBACK);
	puts(buff);
	puts("\n");
}

void print_text(mxml_node_t *node) // print specifying node value what is text type. 
{
	if (node == NULL)
	{
		perror("node is null!\n");
		return;
	}

	puts(mxmlGetText(node, 0));

}

void print_integer(mxml_node_t *node) // print specifying node value what is integer type.  
{
	int n;

	if (node == NULL)
	{
		perror("node is null!\n");
		return;
	}

	n = atoi(mxmlGetText(node, 0));
	printf("%d\n", n);

}

// post 请求方法

size_t writer(void *buff, size_t size, size_t nmem, void *usrp)
{

	puts((char*) buff);
	puts("\n");
	return size * nmem;

}

int postUrl(const char *url, char *xmlStr)
{

	if (NULL == url)
	{
		printf("url is null!\n");
		return -1;
	}

	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, xmlStr);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
		{
			puts("res: fail to post\n");
			fprintf(stderr, "curl_easy_perform() failed :%s\n",
					curl_easy_strerror(res));
			return -1;
		}

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	return 0;
}

/*  设夓配置信息转换成xml 字符?/    */
int BaseInfo_to_XML(BaseInfo *bi, char *xmldata)

{
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *dev;

	mxml_node_t *sysint;
	mxml_node_t *smsid;
	mxml_node_t *smsaddr;
	mxml_node_t *smsport;
	mxml_node_t *smsaddrport;
	mxml_node_t *ip;
	mxml_node_t *port;
	mxml_node_t *deviceid;
	mxml_node_t *devicecode;
	mxml_node_t *localport;
	mxml_node_t *heartbeatinterval;

//	mxml_node_t *reportevent;
//	mxml_node_t *enable;
//	mxml_node_t *rawdata;

//	mxml_node_t *limit ;
//	mxml_node_t *type;
//	mxml_node_t *level;

	mxml_node_t *devicename;

	mxml_node_t *sysinfo;
	mxml_node_t *serialnum;
	mxml_node_t *hardwareversion;
	mxml_node_t *softwareversion;
	mxml_node_t *manufacturer;
	mxml_node_t *managerip;
	mxml_node_t *managermask;
	mxml_node_t *managergateway;
	mxml_node_t *managername;
	mxml_node_t *devicetype;
	mxml_node_t *validrate;
//
//	mxml_node_t *firewall;
//	mxml_node_t *rate;
//	mxml_node_t *mtu;
//	mxml_node_t *workmode;

	int i;

	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "data");

	mxmlElementSetAttr(data, "direct", "4");
	mxmlElementSetAttr(data, "cmd", "5652");
	mxmlElementSetAttr(data, "subcmd", "1");
	mxmlElementSetAttr(data, "recode", "0");

	dev = mxmlNewElement(data, "dev");
	mxmlElementSetAttr(dev, "id", bi->id);
	mxmlElementSetAttr(dev, "src_sms_id", bi->src_sms_id);

	sysint = mxmlNewElement(dev, "sysint");
	smsid = mxmlNewElement(sysint, "smsid");
	mxmlNewText(smsid, 0, bi->sysint.smsid);

	smsaddr = mxmlNewElement(sysint, "smsaddr");
	mxmlNewText(smsaddr, 0, bi->sysint.smsaddr);

	smsport = mxmlNewElement(sysint, "smsport");
	mxmlNewInteger(smsport, bi->sysint.smsport);

	for (i = 0; i < bi->sysint.smsaddrportlength; i++)
	{
		smsaddrport = mxmlNewElement(sysint, "smsaddrport");
		smsid = mxmlNewElement(smsaddrport, "smsid");
		mxmlNewText(smsid, 0, bi->sysint.smsaddrport[i].sms_id);

		ip = mxmlNewElement(smsaddrport, "ip");
		mxmlNewText(ip, 0, bi->sysint.smsaddrport[i].ip);

		port = mxmlNewElement(smsaddrport, "port");
		mxmlNewInteger(port, bi->sysint.smsaddrport[i].port);

	}
	deviceid = mxmlNewElement(sysint, "deviceid");
	mxmlNewText(deviceid, 0, bi->sysint.deviceid);

	devicecode = mxmlNewElement(sysint, "devicecode");
	mxmlNewText(devicecode, 0, bi->sysint.devicecode);

	localport = mxmlNewElement(sysint, "localport");
	mxmlNewInteger(localport, bi->sysint.localport);

	heartbeatinterval = mxmlNewElement(sysint, "heartbeatinterval");
	mxmlNewInteger(heartbeatinterval, bi->sysint.heartbeatinterval);

//	reportevent = mxmlNewElement(sysint, "reportevent");
//	enable = mxmlNewElement(reportevent, "enable");
//	mxmlNewText(enable, 0,  bi->sysint.reportevent.enable);
//
//	rawdata = mxmlNewElement(reportevent, "rawdata");
//	mxmlNewText(rawdata, 0,  bi->sysint.reportevent.rawdata);

	devicename = mxmlNewElement(sysint, "devicename");
	mxmlNewText(devicename, 0, bi->sysint.devicename);

	sysinfo = mxmlNewElement(dev, "sysinfo");

	serialnum = mxmlNewElement(sysinfo, "serialnum");
	mxmlNewText(serialnum, 0, bi->sysinfo.serialnum);

	hardwareversion = mxmlNewElement(sysinfo, "hardwareversion");
	mxmlNewText(hardwareversion, 0, bi->sysinfo.hardwareversion);

	softwareversion = mxmlNewElement(sysinfo, "softwareversion");
	mxmlNewText(softwareversion, 0, bi->sysinfo.softwareversion);

	manufacturer = mxmlNewElement(sysinfo, "manufacturer");
	mxmlNewText(manufacturer, 0, bi->sysinfo.manufacturer);

	managerip = mxmlNewElement(sysinfo, "managerip");
	mxmlNewText(managerip, 0, bi->sysinfo.managerip);

	managermask = mxmlNewElement(sysinfo, "managermask");
	mxmlNewText(managermask, 0, bi->sysinfo.managermask);

	managergateway = mxmlNewElement(sysinfo, "managergateway");
	mxmlNewText(managergateway, 0, bi->sysinfo.managergateway);

	managername = mxmlNewElement(sysinfo, "managername");
	mxmlNewText(managername, 0, bi->sysinfo.managername);

	devicetype = mxmlNewElement(sysinfo, "devicetype");
	mxmlNewText(devicetype, 0, bi->sysinfo.devicetype);

//	firewall = mxmlNewElement(dev, "firewall");
//
//	rate = mxmlNewElement(firewall, "rate");
//	mxmlNewText(rate, 0, bi->firewall.rate);
//
//	mtu = mxmlNewElement(firewall, "mtu");
//	mxmlNewText(mtu, 0, bi->firewall.mtu);
//
//	workmode = mxmlNewElement(workmode, "firewall");
//	mxmlNewText(workmode, 0, bi->firewall.workmode);

	validrate = mxmlNewElement(sysinfo, "validrate");
	mxmlNewText(validrate, 0, bi->sysinfo.validrate);

	mxmlSaveString(xml, xmldata, MAXSIZE, MXML_NO_CALLBACK);

	mxmlDelete(xml);
	return 0;
}

int disp_baseinfo(BaseInfo *bi)
{
	if (bi == NULL)
	{
		puts("base info is null \n");
		return -1;
	}

	int i;

	puts(bi->id);
	puts("\n");
	puts(bi->src_sms_id);
	puts("\n");
	puts(bi->sysint.smsaddr);
	puts("\n");
	printf("%d\n", bi->sysint.smsport);

	puts(bi->sysint.smsid);
	puts("\n");
	puts(bi->sysint.deviceid);
	puts("\n");
	puts(bi->sysint.devicecode);
	puts("\n");
	printf("%d\n", bi->sysint.localport);
	printf("%d\n", bi->sysint.heartbeatinterval);
	puts(bi->sysint.devicename);
	puts("\n");
	printf("%d\n", bi->sysint.smsaddrportlength);

	for (i = 0; i < bi->sysint.smsaddrportlength; i++)
	{
		puts(bi->sysint.smsaddrport[i].ip);
		puts("\n");
		printf("%d\n", bi->sysint.smsaddrport[i].port);
		puts(bi->sysint.smsaddrport[i].sms_id);
		puts("\n");

	}

	puts(bi->sysinfo.serialnum);
	puts("\n");
	puts(bi->sysinfo.hardwareversion);
	puts("\n");
	puts(bi->sysinfo.softwareversion);
	puts("\n");
	puts(bi->sysinfo.manufacturer);
	puts("\n");
	puts(bi->sysinfo.managerip);
	puts("\n");
	puts(bi->sysinfo.managermask);
	puts("\n");
	puts(bi->sysinfo.managergateway);
	puts("\n");
	puts(bi->sysinfo.managername);
	puts("\n");
	puts(bi->sysinfo.devicetype);
	puts("\n");
	return 0;
}

/* 状?上报数据转换至xml 数据 */
int StatusReport_to_XML(StatusReport *sr, char *xmldata)
{
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *event_type;
	mxml_node_t *dev_id;
	mxml_node_t *ip;
	mxml_node_t *mac;
	mxml_node_t *status;
	mxml_node_t *deviceabnormalreason;
	mxml_node_t *cpuusage;
	mxml_node_t *memusage;
	mxml_node_t *diskusage;

	if (sr == NULL)
	{
		printf("report message is null!\n");
		return -1;
	}
	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "data");
	mxmlElementSetAttr(data, "direct", "5");
	mxmlElementSetAttr(data, "cmd", "5701");
	mxmlElementSetAttr(data, "subcmd", "1");

	event_type = mxmlNewElement(data, "event_type");

	mxmlNewText(event_type, 0, sr->event_type);

	dev_id = mxmlNewElement(data, "dev_id");

	mxmlNewText(dev_id, 0, sr->dev_id);

	ip = mxmlNewElement(data, "ip");

	mxmlNewText(ip, 0, sr->ip);

	mac = mxmlNewElement(data, "mac");

	mxmlNewText(mac, 0, sr->mac);

	status = mxmlNewElement(data, "status");

	mxmlNewText(status, 0, sr->status);

	deviceabnormalreason = mxmlNewElement(data, "deviceabnormalreason");

	mxmlNewText(deviceabnormalreason, 0, sr->deviceabnormalreason);

	cpuusage = mxmlNewElement(data, "cpuusage");

	mxmlNewInteger(cpuusage, sr->cpuusage);

	memusage = mxmlNewElement(data, "memusage");

	mxmlNewInteger(memusage, sr->memusage);

	diskusage = mxmlNewElement(data, "diskusage");

	mxmlNewInteger(diskusage, sr->diskusage);
	char buff[MAXSIZE];
	mxmlSaveString(xml, buff, MAXSIZE, MXML_NO_CALLBACK);
//	strcpy(xmldata, "xml=");
	strcpy(xmldata, buff);

	mxmlDelete(xml);
	return 0;

}

// 绛栫暐淇敼鍜屾棩蹇椾簨浠舵姤鏂囪浆鎹㈣嚦ml
int RuleLog_to_XML(RuleLog *rl, char *xmldata)
{

	if (rl == NULL)
	{
		printf("Rule log message is empty!\n");
		return -1;
	}

	mxml_node_t *xml;
	mxml_node_t *data;

	mxml_node_t *event_type;
	mxml_node_t *sub_type;
	mxml_node_t *dev_id;

	mxml_node_t *adminlog;
	mxml_node_t *event_id;
	mxml_node_t *time;
	mxml_node_t *user;
	mxml_node_t *log_type;
	mxml_node_t *description;
	mxml_node_t *operation;
	mxml_node_t *remark;

	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "data");
	mxmlElementSetAttr(data, "direct", "5");
	mxmlElementSetAttr(data, "cmd", "5702");
	mxmlElementSetAttr(data, "subcmd", "2");

	event_type = mxmlNewElement(data, "event_type");
	mxmlNewText(event_type, 0, rl->event_type);
	sub_type = mxmlNewElement(data, "sub_type");
	mxmlNewText(sub_type, 0, "01");

	dev_id = mxmlNewElement(data, "dev_id");
	mxmlNewText(dev_id, 0, rl->dev_id);

	int i;
	for (i = 0; i < rl->loglength; i++)
	{
		adminlog = mxmlNewElement(data, "adminlog");

		event_id = mxmlNewElement(adminlog, "event_id");
		mxmlNewText(event_id, 0, rl->adminlog[i].event_id);

		time = mxmlNewElement(adminlog, "time");
		mxmlNewText(time, 0, rl->adminlog[i].time);

		user = mxmlNewElement(adminlog, "user");
		mxmlNewText(user, 0, rl->adminlog[i].user);

		log_type = mxmlNewElement(adminlog, "log_type");
		mxmlNewText(log_type, 0, rl->adminlog[i].log_type);

		description = mxmlNewElement(adminlog, "description");
		mxmlNewText(description, 0, rl->adminlog[i].description);

		operation = mxmlNewElement(adminlog, "operation");
		mxmlNewText(operation, 0, rl->adminlog[i].operation);

		remark = mxmlNewElement(adminlog, "remark");
		mxmlNewText(remark, 0, rl->adminlog[i].remark);

	}

	char buff[MAXSIZE];
	mxmlSaveString(xml, buff, MAXSIZE, MXML_NO_CALLBACK);

	strcpy(xmldata, buff);

	mxmlDelete(xml);
	return 0;

}

//策略违事件上报xml
int RuleError_to_XML(RuleError *re, char *xmldata)
{
	if (re == NULL)
	{
		perror("rule error is null!\n");
		return -1;
	}

	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *event_type;
	mxml_node_t *dev_id;
	mxml_node_t *time;
	mxml_node_t *infoType;
	mxml_node_t *info;
	mxml_node_t *channel;
	mxml_node_t *ruleName;
	mxml_node_t *description;
	mxml_node_t *remark;
	mxml_node_t *event_id;

	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "data");
	mxmlElementSetAttr(data, "direct", "5");
	mxmlElementSetAttr(data, "cmd", "5702");
	mxmlElementSetAttr(data, "subcmd", "3");

	event_id = mxmlNewElement(data, "event_id");
	mxmlNewText(event_id, 0, re->event_id);

	event_type = mxmlNewElement(data, "event_type");
	mxmlNewText(event_type, 0, re->event_type);

	dev_id = mxmlNewElement(data, "dev_id");
	mxmlNewText(dev_id, 0, re->dev_id);

	time = mxmlNewElement(data, "time");
	mxmlNewText(time, 0, re->time);

	infoType = mxmlNewElement(data, "infoType");
	mxmlNewText(infoType, 0, re->infoType);

	info = mxmlNewElement(data, "info");

	channel = mxmlNewElement(info, "channel");
	mxmlNewText(channel, 0, re->info.channel);

	ruleName = mxmlNewElement(info, "ruleName");
	mxmlNewText(ruleName, 0, re->info.ruleName);

	description = mxmlNewElement(info, "description");
	mxmlNewText(description, 0, re->info.description);

	remark = mxmlNewElement(data, "remark");
	mxmlNewText(remark, 0, re->remark);

	char buff[MAXSIZE];
	//strcpy(buff,"xml=");
	mxmlSaveString(xml, buff, MAXSIZE, MXML_NO_CALLBACK);
	//strcpy(xmldata, "xml=");
	strcpy(xmldata, buff);

	mxmlDelete(xml);
	//	puts(xmldata); 
	return 0;
}

int Rule_to_XML(Rule *rl, char *xmldata, int rulLen, Rule *r2, int ruleLen2)
{

	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *rule;
	mxml_node_t *id;
	mxml_node_t *srcList;
	mxml_node_t *dstList;
	mxml_node_t *src;
	mxml_node_t *dst;
	mxml_node_t *hostName;
	mxml_node_t *ipAddr;
	mxml_node_t *mask;
	mxml_node_t *mac;
	mxml_node_t *natIp;
	mxml_node_t *remark;
	mxml_node_t *gateway;
	mxml_node_t *macList;
	mxml_node_t *protocol;
	mxml_node_t *flag;
	mxml_node_t *port;

	int i;

	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "data");

	mxmlElementSetAttr(data, "direct", "4");
	mxmlElementSetAttr(data, "cmd", "5653");
	mxmlElementSetAttr(data, "subcmd", "1");
	mxmlElementSetAttr(data, "recode", "0");

	BaseInfo *bi = malloc(sizeof(BaseInfo));
	if (parse_deviceinfoxml(DEVICEINFO, bi) < 0)
	{
		return -1;
	}

	mxml_node_t *dev_id;

	dev_id = mxmlNewElement(data, "dev_id");
	mxmlNewText(dev_id, 0, bi->id);

	free(bi);
	int q;
	for (q = 0; q < rulLen; q++)
	{
		rule = mxmlNewElement(data, "rule");
		mxmlElementSetAttr(rule, "channel", rl->channel);

		id = mxmlNewElement(rule, "id");
//		mxmlNewText(id, 0, rl->id);
		mxmlNewText(id, 0, "0");

		srcList = mxmlNewElement(rule, "srcList");
		dstList = mxmlNewElement(rule, "dstList");

		//printf("rl->srcListLength = %d\n", rl->srcListLength);

		for (i = 0; i < rl->srcListLength; i++)
		{
			src = mxmlNewElement(srcList, "src");

			hostName = mxmlNewElement(src, "hostName");
			mxmlNewText(hostName, 0, rl->srcList[i].hostName);

			ipAddr = mxmlNewElement(src, "ipAddr");
			mxmlNewText(ipAddr, 0, rl->srcList[i].ipAddr);

			mask = mxmlNewElement(src, "mask");
			mxmlNewText(mask, 0, rl->srcList[i].mask);

			mac = mxmlNewElement(src, "mac");
			mxmlNewText(mac, 0, rl->srcList[i].mac);

			natIp = mxmlNewElement(src, "natIp");
			mxmlNewText(natIp, 0, rl->srcList[i].natIp);

			remark = mxmlNewElement(src, "remark");
			mxmlNewText(remark, 0, rl->srcList[i].remark);

			gateway = mxmlNewElement(src, "gateway");
			mxmlNewText(gateway, 0, rl->srcList[i].gateway);

			macList = mxmlNewElement(src, "macList");
			mxmlNewText(macList, 0, rl->srcList[i].macList);

		}

		for (i = 0; i < rl->dstListLength; i++)
		{
			dst = mxmlNewElement(dstList, "dst");
			hostName = mxmlNewElement(dst, "hostName");
			mxmlNewText(hostName, 0, rl->dstList[i].hostName);

			ipAddr = mxmlNewElement(dst, "ipAddr");
			mxmlNewText(ipAddr, 0, rl->dstList[i].ipAddr);

			mask = mxmlNewElement(dst, "mask");
			mxmlNewText(mask, 0, rl->dstList[i].mask);

			mac = mxmlNewElement(dst, "mac");
			mxmlNewText(mac, 0, rl->dstList[i].mac);

			natIp = mxmlNewElement(dst, "natIp");
			mxmlNewText(natIp, 0, rl->dstList[i].natIp);

			remark = mxmlNewElement(dst, "remark");
			mxmlNewText(remark, 0, rl->dstList[i].remark);

			gateway = mxmlNewElement(dst, "gateway");
			mxmlNewText(gateway, 0, rl->dstList[i].gateway);

			macList = mxmlNewElement(dst, "macList");
			mxmlNewText(macList, 0, rl->dstList[i].macList);

		}

		protocol = mxmlNewElement(rule, "protocol");
		mxmlNewText(protocol, 0, rl->protocol);

		port = mxmlNewElement(rule, "port");
		mxmlNewInteger(port, rl->port);

		flag = mxmlNewElement(rule, "flag");
		mxmlNewText(flag, 0, rl->flag);

		rl++;
	}

/////////////////rule channel2
	for (q = 0; q < ruleLen2; q++)
	{
		rule = mxmlNewElement(data, "rule");
		mxmlElementSetAttr(rule, "channel", r2->channel);

		id = mxmlNewElement(rule, "id");
//		mxmlNewText(id, 0, r2->id);
		mxmlNewText(id, 0, "0");

		srcList = mxmlNewElement(rule, "srcList");
		dstList = mxmlNewElement(rule, "dstList");

		//printf("r2->srcListLength = %d\n", r2->srcListLength);

		for (i = 0; i < r2->srcListLength; i++)
		{
			src = mxmlNewElement(srcList, "src");

			hostName = mxmlNewElement(src, "hostName");
			mxmlNewText(hostName, 0, r2->srcList[i].hostName);

			ipAddr = mxmlNewElement(src, "ipAddr");
			mxmlNewText(ipAddr, 0, r2->srcList[i].ipAddr);

			mask = mxmlNewElement(src, "mask");
			mxmlNewText(mask, 0, r2->srcList[i].mask);

			mac = mxmlNewElement(src, "mac");
			mxmlNewText(mac, 0, r2->srcList[i].mac);

			natIp = mxmlNewElement(src, "natIp");
			mxmlNewText(natIp, 0, r2->srcList[i].natIp);

			remark = mxmlNewElement(src, "remark");
			mxmlNewText(remark, 0, r2->srcList[i].remark);

			gateway = mxmlNewElement(src, "gateway");
			mxmlNewText(gateway, 0, r2->srcList[i].gateway);

			macList = mxmlNewElement(src, "macList");
			mxmlNewText(macList, 0, r2->srcList[i].macList);

		}

		for (i = 0; i < r2->dstListLength; i++)
		{
			dst = mxmlNewElement(dstList, "dst");
			hostName = mxmlNewElement(dst, "hostName");
			mxmlNewText(hostName, 0, r2->dstList[i].hostName);

			ipAddr = mxmlNewElement(dst, "ipAddr");
			mxmlNewText(ipAddr, 0, r2->dstList[i].ipAddr);

			mask = mxmlNewElement(dst, "mask");
			mxmlNewText(mask, 0, r2->dstList[i].mask);

			mac = mxmlNewElement(dst, "mac");
			mxmlNewText(mac, 0, r2->dstList[i].mac);

			natIp = mxmlNewElement(dst, "natIp");
			mxmlNewText(natIp, 0, r2->dstList[i].natIp);

			remark = mxmlNewElement(dst, "remark");
			mxmlNewText(remark, 0, r2->dstList[i].remark);

			gateway = mxmlNewElement(dst, "gateway");
			mxmlNewText(gateway, 0, r2->dstList[i].gateway);

			macList = mxmlNewElement(dst, "macList");
			mxmlNewText(macList, 0, r2->dstList[i].macList);
		}

		protocol = mxmlNewElement(rule, "protocol");
		mxmlNewText(protocol, 0, r2->protocol);

		port = mxmlNewElement(rule, "port");
		mxmlNewInteger(port, r2->port);

		flag = mxmlNewElement(rule, "flag");
		mxmlNewText(flag, 0, r2->flag);

		r2++;
	}

	char buff[MAXSIZE_RULE];
	mxmlSaveString(xml, buff, MAXSIZE_RULE, MXML_NO_CALLBACK);
	strcpy(xmldata, buff);

	mxmlDelete(xml);
	return 0;
}

int parse_deviceinfoxml(char *filepath, BaseInfo *bi) // parse base information to BaseInfo structor.
{

	FILE *fp;
	mxml_node_t *tree = NULL;
	mxml_node_t *data = NULL;
	mxml_node_t *dev = NULL;
	mxml_node_t *sysint = NULL;
	mxml_node_t *smsaddr = NULL;
	mxml_node_t *smsport = NULL;

	mxml_node_t *smsaddrport = NULL;
	mxml_node_t *smsid = NULL;
	mxml_node_t *ip = NULL;
	mxml_node_t *port = NULL;

	mxml_node_t *deviceid = NULL;
	mxml_node_t *devicecode = NULL;
	mxml_node_t *localport = NULL;
	mxml_node_t *heartbeatinterval = NULL;
	mxml_node_t *devicename = NULL;

	mxml_node_t *sysinfo = NULL;
	mxml_node_t *serialnum = NULL;
	mxml_node_t *hardwareversion = NULL;
	mxml_node_t *softwareversion = NULL;
	mxml_node_t *manufacturer = NULL;
	mxml_node_t *managerip = NULL;
	mxml_node_t *managermask = NULL;
	mxml_node_t *managergateway = NULL;
	mxml_node_t *managername = NULL;
	mxml_node_t *devicetype = NULL;
	mxml_node_t *validrate = NULL;

	if (NULL == (fp = fopen(filepath, "r")))
	{
		printf("fail to open file!\n");
		return -1;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	if (tree == NULL)
	{
		printf("fail to load xml file.\n");
		return -1;
	}

	fclose(fp);

	data = mxmlFindElement(tree, tree, "data", NULL, NULL, MXML_DESCEND);
	if (data == NULL)
	{
		printf("can not find data node!, please check xml file.\n");
		return -1;
	}

	dev = mxmlFindElement(data, tree, "dev", NULL, NULL, MXML_DESCEND);
	if (dev == NULL)
	{
		printf("can not find dev node!, please check xml file.\n");
		return -1;
	}

	if (NULL == mxmlElementGetAttr(dev, "id"))
	{
		strcpy(bi->id, "0");
	}
	else
	{
		strcpy(bi->id, mxmlElementGetAttr(dev, "id"));

	}

	if (NULL == mxmlElementGetAttr(dev, "src_sms_id"))
	{
		strcpy(bi->src_sms_id, "0");
	}
	else
	{
		strcpy(bi->src_sms_id, mxmlElementGetAttr(dev, "src_sms_id"));
	}

	sysint = mxmlFindElement(dev, data, "sysint", NULL, NULL, MXML_DESCEND);
	if (sysint == NULL)
	{
		printf("can not find sysint node , please check xml file.!\n ");
		return -1;
	}

	smsid = mxmlFindElement(sysint, tree, "smsid", NULL, NULL, MXML_DESCEND);
	if (NULL == mxmlGetText(smsid, 0))
	{
		strcpy(bi->sysint.smsid, "0");
	}
	else
	{
		strcpy(bi->sysint.smsid, mxmlGetText(smsid, 0));
	}

	smsaddr = mxmlFindElement(sysint, tree, "smsaddr", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(smsaddr, 0))
	{
		strcpy(bi->sysint.smsaddr, "0");
	}
	else
	{
		strcpy(bi->sysint.smsaddr, mxmlGetText(smsaddr, 0));
	}

	smsport = mxmlFindElement(sysint, tree, "smsport", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(smsport, 0))
	{
		bi->sysint.smsport = 0;
	}
	else
	{
		bi->sysint.smsport = atoi(mxmlGetText(smsport, 0));

	}

	deviceid = mxmlFindElement(sysint, tree, "deviceid", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(deviceid, 0))
	{
		strcpy(bi->sysint.deviceid, "0");
	}
	else
	{
		strcpy(bi->sysint.deviceid, mxmlGetText(deviceid, 0));
	}
	devicecode = mxmlFindElement(sysint, tree, "devicecode", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(devicecode, 0))
	{
		strcpy(bi->sysint.devicecode, "0");
	}
	else
	{
		strcpy(bi->sysint.devicecode, mxmlGetText(devicecode, 0));
	}
	localport = mxmlFindElement(sysint, tree, "localport", NULL, NULL,
			MXML_DESCEND);
	if (localport == NULL)
	{
		bi->sysint.localport = 0;
	}
	else
	{
		bi->sysint.localport = atoi(mxmlGetText(localport, 0));
	}

	heartbeatinterval = mxmlFindElement(sysint, tree, "heartbeatinterval", NULL,
			NULL, MXML_DESCEND);
	if (heartbeatinterval == NULL)
	{
		bi->sysint.heartbeatinterval = 0;
	}
	else
	{
		bi->sysint.heartbeatinterval = atoi(mxmlGetText(heartbeatinterval, 0));
	}
	devicename = mxmlFindElement(sysint, tree, "devicename", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(devicename, 0))
	{
		strcpy(bi->sysint.devicename, "0");
	}
	else
	{
		strcpy(bi->sysint.devicename, mxmlGetText(devicename, 0));
	}
	sysinfo = mxmlFindElement(dev, data, "sysinfo", NULL, NULL, MXML_DESCEND);
	if (sysinfo == NULL)
	{
		printf("can not find sysinfo node.\n");
	}

	serialnum = mxmlFindElement(sysinfo, data, "serialnum", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(serialnum, 0))
	{
		strcpy(bi->sysinfo.serialnum, "0");
	}
	else
	{
		strcpy(bi->sysinfo.serialnum, mxmlGetText(serialnum, 0));
	}
	hardwareversion = mxmlFindElement(sysinfo, data, "hardwareversion", NULL,
			NULL, MXML_DESCEND);

	if (NULL == mxmlGetText(hardwareversion, 0))
	{
		strcpy(bi->sysinfo.hardwareversion, "0");
	}
	else
	{
		strcpy(bi->sysinfo.hardwareversion, mxmlGetText(hardwareversion, 0));
	}

	softwareversion = mxmlFindElement(sysinfo, data, "softwareversion", NULL,
			NULL, MXML_DESCEND);

	if (NULL == mxmlGetText(softwareversion, 0))
	{
		strcpy(bi->sysinfo.softwareversion, "0");
	}
	else
	{
		strcpy(bi->sysinfo.softwareversion, mxmlGetText(softwareversion, 0));
	}

	manufacturer = mxmlFindElement(sysinfo, data, "manufacturer", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(manufacturer, 0))
	{
		strcpy(bi->sysinfo.manufacturer, "0");
	}
	else
	{
		strcpy(bi->sysinfo.manufacturer, mxmlGetText(manufacturer, 0));
	}
	managerip = mxmlFindElement(sysinfo, data, "managerip", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(managerip, 0))
	{
		strcpy(bi->sysinfo.managerip, "0");
	}
	else
	{
		strcpy(bi->sysinfo.managerip, mxmlGetText(managerip, 0));
	}
	managermask = mxmlFindElement(sysinfo, data, "managermask", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(managermask, 0))
	{
		strcpy(bi->sysinfo.managermask, "0");
	}
	else
	{
		strcpy(bi->sysinfo.managermask, mxmlGetText(managermask, 0));
	}
	managergateway = mxmlFindElement(sysinfo, data, "managergateway", NULL,
			NULL, MXML_DESCEND);

	if (NULL == mxmlGetText(managergateway, 0))
	{
		strcpy(bi->sysinfo.managergateway, "0");
	}
	else
	{
		strcpy(bi->sysinfo.managergateway, mxmlGetText(managergateway, 0));
	}
	managername = mxmlFindElement(sysinfo, data, "managername", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(managername, 0))
	{
		strcpy(bi->sysinfo.managername, "0");
	}
	else
	{
		strcpy(bi->sysinfo.managername, mxmlGetText(managername, 0));
	}
	devicetype = mxmlFindElement(sysinfo, data, "devicetype", NULL, NULL,
			MXML_DESCEND);
	if (NULL == mxmlGetText(devicetype, 0))
	{
		strcpy(bi->sysinfo.devicetype, "0");
	}
	else
	{
		strcpy(bi->sysinfo.devicetype, mxmlGetText(devicetype, 0));
	}
	validrate = mxmlFindElement(sysinfo, data, "validrate", NULL, NULL,
			MXML_DESCEND);

	if (NULL == mxmlGetText(validrate, 0))
	{
		strcpy(bi->sysinfo.validrate, "0");
	}
	else
	{
		strcpy(bi->sysinfo.validrate, mxmlGetText(validrate, 0));
	}
	puts("\n");

	mxmlDelete(tree);
	return 0;

}

int LoadXMLFile(char *filepath, char *xmldata)
{
	FILE *fp;
	mxml_node_t *tree;
	if (NULL == (fp = fopen(filepath, "r")))
	{
		fprintf(stderr, "faile to open %s\n", filepath);
		return -1;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	mxmlSaveString(tree, xmldata, MAXSIZE, MXML_NO_CALLBACK);

	mxmlDelete(tree);
	fclose(fp);
	return 0;
}

/***
 * 缁勮闇?灞曠ず鐨刋ML瀵硅?
 */
int ruleshow(char* xmlStr, char *src, char *dst, char *rule, char *src2,
		char *dst2, char *rule2)
{
	int i;
	int numi = 0;
	//int curpage = 1;
	int macSrcLen = 0;
	int macSrcLen2 = 0;
	int macDstLen = 0;
	int macDstLen2 = 0;
	char num_tmp[3];
	char macIpS[20];
	char macIpD[20];
	//rl = (Rule *) malloc(sizeof(Rule)) ; 
	int ruleLen; //
	int ruleLen2;

	struct hostnode_struct *returnSrcPri;
	struct hostnode_struct *returnDstPri; //鐩殑Mac缁撴瀯浣撴暟缁勫ご鎸囬拡
	struct rule_struct *returnPri; //瑙勫垯缁撴?浣撴暟缁勫ご鎸囬?
	returnPri = getRuleStruct(rule);
	macSrcLen = getStructLen(src);
	macSrcLen2 = getStructLen(src2);
	macDstLen = getStructLen(dst);
	macDstLen2 = getStructLen(dst2);
	ruleLen = getRuleStructLen(rule);
	ruleLen2 = getRuleStructLen(rule2);

	Rule *rl;
	rl = (Rule *) malloc(sizeof(Rule) * ruleLen);
	Rule *r2;
	r2 = (Rule *) malloc(sizeof(Rule) * ruleLen2);

	for (i = 0; i < ruleLen; i++)
	{ //寰幆瑙勫垯琛ㄧ粨鏋勪綋鏁扮?

		rl[i].srcListLength = 0;
		rl[i].dstListLength = 0;

		memset(num_tmp, '\0', sizeof(num_tmp));

		if (strcmp(rule, CHANNEL0_RULE) == 0)
		{
			strcpy(rl[i].channel, "0");
		}

		if (strcmp(rule, CHANNEL1_RULE) == 0)
		{
			strcpy(rl[i].channel, "1");
		}

		returnSrcPri = getStruct(src);
		returnDstPri = getStruct(dst);
		sprintf(num_tmp, "%d", ++numi);
		char ruleIpS[LINEMAX] =
		{ };
		char ruleIpD[64] =
		{ };
		char ruleNameS[128] =
		{ };
		char ruleNameD[128] =
		{ };
		char ruleProt[64] =
		{ };
		char rulePort[64] =
		{ };
		char appCheck[64] =
		{ };

		int k = 0;
		int j = 0;
		ipaddr2string(htonl(returnPri->sip), ruleIpS, sizeof(ruleIpS));

		ipaddr2string(htonl(returnPri->dip), ruleIpD, sizeof(ruleIpD));

		sprintf(ruleProt, "%d", returnPri->proto);
		if (strcmp(ruleProt, "1") == 0)
		{
			strcpy(ruleProt, "TCP");
		}
		else if (strcmp(ruleProt, "2") == 0)
		{
			strcpy(ruleProt, "UDP");
		}
		else if (strcmp(ruleProt, "5") == 0)
		{
			strcpy(ruleProt, "ALL");
		}
		else
		{
			strcpy(ruleProt, "ERROR");
		}
		strcpy(rl[i].protocol, ruleProt);

		sprintf(rulePort, "%d", returnPri->dport);
		rl[i].port = returnPri->dport;

		sprintf(appCheck, "%d", returnPri->appcheck);
		if (strcmp(appCheck, "1") == 0)
		{
			strcpy(appCheck, "Y");
		}
		else if (strcmp(appCheck, "0") == 0)
		{
			strcpy(appCheck, "N");
		}
		else
		{
			strcpy(appCheck, "ERROR");
		}
		strcpy(rl[i].flag, appCheck);

		char temp_mask[20];
		char *mask_str;
		memset(num_tmp, '\0', sizeof(num_tmp));
		memset(temp_mask, '\0', sizeof(temp_mask));

		if (strcmp(ALL, ruleIpS) == 0)
		{ //
			sprintf(ruleNameS, "%s", "ALL");
			for (k = 0; k < macSrcLen; k++)
			{
				char macIpS[64] =
				{ };
				ipaddr2string(htonl(returnSrcPri->ip), macIpS, sizeof(macIpS));
				strcpy(rl[i].srcList[k].ipAddr, macIpS);
				//	puts(rl[i].srcList[k].ipAddr) ;
				sprintf(ruleNameS, "%s", returnSrcPri->name);
				strcpy(rl[i].srcList[k].hostName, ruleNameS);
				// 	puts(rl[i].srcList[k].hostName) ;
				sprintf(temp_mask, "%d", returnSrcPri->nmask);
				mask_str = mask_turn(temp_mask);
				strcpy(rl[i].srcList[k].mask, mask_str);
				//   puts(rl[i].srcList[k].mask) ;
				int n;
				char mac_str[64] =
				{ };
				char mac_new[64] =
				{ };
				n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));

				mac_str[n] = 0;
				turn_mac(mac_str, mac_new); //
				strcpy(rl[i].srcList[k].mac, mac_new);
				//	 puts(rl[i].srcList[k].mac) ;

				char vip_str[64] =
				{ };
				char vip_new[64] =
				{ };
				if (returnSrcPri->vip != -1)
				{
					ipaddr2string(htonl(returnSrcPri->vip), vip_str,
							sizeof(vip_str)); //
					sprintf(vip_new, "%s", vip_str);

				}
				else
					memset(vip_new, 0, strlen(vip_new));

				strcpy(rl[i].srcList[k].natIp, vip_new);
				//	  puts(rl[i].srcList[k].natIp);

				strcpy(rl[i].srcList[k].remark, returnSrcPri->comment);
				//		 puts(rl[i].srcList[k].remark);

				char gate[10] =
				{ };
				if (returnSrcPri->type == NODE_TYPE_GATEWAY)
					strcpy(gate, "G");
				else
					strcpy(gate, "H");
				strcpy(rl[i].srcList[k].gateway, gate);

				//	 puts(rl[i].srcList[k].gateway);

				strcpy(rl[i].srcList[k].macList, returnSrcPri->othermac);

				//puts(rl[i].srcList[k].macList);

				returnSrcPri++;
				rl[i].srcListLength++;
			}
		}
		else
		{
			for (k = 0; k < macSrcLen; k++)
			{ //
				char macIpS[64] =
				{ };
				ipaddr2string(htonl(returnSrcPri->ip), macIpS, sizeof(macIpS));
				if (strcmp(ruleIpS, macIpS) == 0)
				{
					sprintf(ruleNameS, "%s", returnSrcPri->name);
					strcpy(rl[i].srcList[0].ipAddr, macIpS);

					//puts(rl[i].srcList[k].ipAddr);

					strcpy(rl[i].srcList[0].hostName, ruleNameS);
					// puts(rl[i].srcList[k].hostName);

					sprintf(temp_mask, "%d", returnSrcPri->nmask);

					mask_str = mask_turn(temp_mask);
					strcpy(rl[i].srcList[0].mask, mask_str);
					// puts(rl[i].srcList[k].mask);

					char mac_str[64] =
					{ };
					char mac_new[64] =
					{ };

					int n;
					n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new); //
					strcpy(rl[i].srcList[0].mac, mac_new);
					//	puts(rl[i].srcList[k].mac);

					char vip_str[64] =
					{ };
					char vip_new[64] =
					{ };
					if (returnSrcPri->vip != -1)
					{
						ipaddr2string(htonl(returnSrcPri->vip), vip_str,
								sizeof(vip_str)); //
						sprintf(vip_new, "%s", vip_str);

					}
					else
						memset(vip_new, 0, strlen(vip_new));

					strcpy(rl[i].srcList[0].natIp, vip_new);

					strcpy(rl[i].srcList[0].remark, returnSrcPri->comment);

					char gate[10] =
					{ };
					if (returnSrcPri->type == NODE_TYPE_GATEWAY)
						strcpy(gate, "G");
					else
						strcpy(gate, "H");
					strcpy(rl[i].srcList[0].gateway, gate);

					strcpy(rl[i].srcList[0].macList, returnSrcPri->othermac);
					rl[i].srcListLength++;
					break;
				}
				returnSrcPri++;

			}
		}

		if (strcmp(ALL, ruleIpD) == 0)
		{
			sprintf(ruleNameD, "%s", "ALL");
			for (k = 0; k < macDstLen; k++)
			{
				char macIpD[64] =
				{ };
				ipaddr2string(htonl(returnDstPri->ip), macIpD, sizeof(macIpD));
				strcpy(rl[i].dstList[k].ipAddr, macIpD);

				sprintf(ruleNameD, "%s", returnDstPri->name);
				strcpy(rl[i].dstList[k].hostName, ruleNameD);

				sprintf(temp_mask, "%d", returnDstPri->nmask);
				mask_str = mask_turn(temp_mask);
				strcpy(rl[i].dstList[k].mask, mask_str);

				int n;
				char mac_str[64] =
				{ };
				char mac_new[64] =
				{ };
				n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
				mac_str[n] = 0;
				turn_mac(mac_str, mac_new); //
				strcpy(rl[i].dstList[k].mac, mac_new);

				char vip_str[64] =
				{ };
				char vip_new[64] =
				{ };
				if (returnDstPri->vip != -1)
				{
					ipaddr2string(htonl(returnDstPri->vip), vip_str,
							sizeof(vip_str)); //
					sprintf(vip_new, "%s", vip_str);
				}
				else
					strcpy(rl[i].dstList[k].natIp, vip_new);

				strcpy(rl[i].dstList[k].remark, returnDstPri->comment);
				char gate[10] =
				{ };
				if (returnDstPri->type == NODE_TYPE_GATEWAY)
					strcpy(gate, "G");
				else
					strcpy(gate, "H");
				strcpy(rl[i].dstList[k].gateway, gate);
				strcpy(rl[i].dstList[k].macList, returnDstPri->othermac);

				returnDstPri++;
				rl[i].dstListLength++;
			}

		}
		else
		{
			for (j = 0; j < macDstLen; j++)
			{ //
				char macIpD[64];
				ipaddr2string(htonl(returnDstPri->ip), macIpD, sizeof(macIpD));
				if (strcmp(ruleIpD, macIpD) == 0)
				{
					sprintf(ruleNameD, "%s", returnDstPri->name);
					strcpy(rl[i].dstList[0].ipAddr, macIpD);
					strcpy(rl[i].dstList[0].hostName, ruleNameD);
					sprintf(temp_mask, "%d", returnDstPri->nmask);
					mask_str = mask_turn(temp_mask);
					strcpy(rl[i].dstList[0].mask, temp_mask);

					char mac_str[64] =
					{ };
					char mac_new[64] =
					{ };
					int n;
					n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new); //
					strcpy(rl[i].dstList[j].mac, mac_new);

					char vip_str[64] =
					{ };
					char vip_new[64] =
					{ };
					if (returnDstPri->vip != -1)
					{
						ipaddr2string(htonl(returnDstPri->vip), vip_str,
								sizeof(vip_str)); //
						sprintf(vip_new, "%s", vip_str);
					}
					else
						memset(vip_new, 0, strlen(vip_new));

					strcpy(rl[i].dstList[0].natIp, vip_new);

					strcpy(rl[i].dstList[0].remark, returnDstPri->comment);
					char gate[10] =
					{ };
					if (returnDstPri->type == NODE_TYPE_GATEWAY)
						strcpy(gate, "G");
					else
						strcpy(gate, "H");
					strcpy(rl[i].dstList[0].gateway, gate);
					strcpy(rl[i].dstList[0].macList, returnDstPri->othermac);
					rl[i].dstListLength++;

					break;
				}
				returnDstPri++;

			}
		}

		returnPri++;

	}

	/////////////////////rule channel 2

	returnPri = getRuleStruct(rule2);
	for (i = 0; i < ruleLen2; i++)
	{ //寰幆瑙勫垯琛ㄧ粨鏋勪綋鏁扮?

		r2[i].srcListLength = 0;
		r2[i].dstListLength = 0;

		memset(num_tmp, '\0', sizeof(num_tmp));

		if (strcmp(rule2, CHANNEL0_RULE) == 0)
		{
			strcpy(r2[i].channel, "0");
		}

		if (strcmp(rule2, CHANNEL1_RULE) == 0)
		{
			strcpy(r2[i].channel, "1");
		}

		returnSrcPri = getStruct(src2);
		returnDstPri = getStruct(dst2);
		sprintf(num_tmp, "%d", ++numi);
		char ruleIpS[LINEMAX] =
		{ };
		char ruleIpD[64] =
		{ };
		char ruleNameS[128] =
		{ };
		char ruleNameD[128] =
		{ };
		char ruleProt[64] =
		{ };
		char rulePort[64] =
		{ };
		char appCheck[64] =
		{ };

		int k = 0;
		int j = 0;
		ipaddr2string(htonl(returnPri->sip), ruleIpS, sizeof(ruleIpS));

		ipaddr2string(htonl(returnPri->dip), ruleIpD, sizeof(ruleIpD));

		sprintf(ruleProt, "%d", returnPri->proto);
		if (strcmp(ruleProt, "1") == 0)
		{
			strcpy(ruleProt, "TCP");
		}
		else if (strcmp(ruleProt, "2") == 0)
		{
			strcpy(ruleProt, "UDP");
		}
		else if (strcmp(ruleProt, "5") == 0)
		{
			strcpy(ruleProt, "ALL");
		}
		else
		{
			strcpy(ruleProt, "ERROR");
		}
		strcpy(r2[i].protocol, ruleProt);

		sprintf(rulePort, "%d", returnPri->dport);
		r2[i].port = returnPri->dport;

		sprintf(appCheck, "%d", returnPri->appcheck);
		if (strcmp(appCheck, "1") == 0)
		{
			strcpy(appCheck, "Y");
		}
		else if (strcmp(appCheck, "0") == 0)
		{
			strcpy(appCheck, "N");
		}
		else
		{
			strcpy(appCheck, "ERROR");
		}
		strcpy(r2[i].flag, appCheck);

		char temp_mask[20];
		char *mask_str;
		memset(num_tmp, '\0', sizeof(num_tmp));
		memset(temp_mask, '\0', sizeof(temp_mask));

		if (strcmp(ALL, ruleIpS) == 0)
		{ //
			sprintf(ruleNameS, "%s", "ALL");
			for (k = 0; k < macSrcLen2; k++)
			{
				char macIpS[64] =
				{ };
				ipaddr2string(htonl(returnSrcPri->ip), macIpS, sizeof(macIpS));
				strcpy(r2[i].srcList[k].ipAddr, macIpS);
				//	puts(r2[i].srcList[k].ipAddr) ;
				sprintf(ruleNameS, "%s", returnSrcPri->name);
				strcpy(r2[i].srcList[k].hostName, ruleNameS);
				// 	puts(r2[i].srcList[k].hostName) ;
				sprintf(temp_mask, "%d", returnSrcPri->nmask);
				mask_str = mask_turn(temp_mask);
				strcpy(r2[i].srcList[k].mask, mask_str);
				//   puts(r2[i].srcList[k].mask) ;
				int n;
				char mac_str[64] =
				{ };
				char mac_new[64] =
				{ };
				n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));

				mac_str[n] = 0;
				turn_mac(mac_str, mac_new); //
				strcpy(r2[i].srcList[k].mac, mac_new);
				//	 puts(rl[i].srcList[k].mac) ;

				char vip_str[64] =
				{ };
				char vip_new[64] =
				{ };
				if (returnSrcPri->vip != -1)
				{
					ipaddr2string(htonl(returnSrcPri->vip), vip_str,
							sizeof(vip_str)); //
					sprintf(vip_new, "%s", vip_str);

				}
				else
					memset(vip_new, 0, strlen(vip_new));

				strcpy(r2[i].srcList[k].natIp, vip_new);
				//	  puts(r2[i].srcList[k].natIp);

				strcpy(r2[i].srcList[k].remark, returnSrcPri->comment);
				//		 puts(r2[i].srcList[k].remark);

				char gate[10] =
				{ };
				if (returnSrcPri->type == NODE_TYPE_GATEWAY)
					strcpy(gate, "G");
				else
					strcpy(gate, "H");
				strcpy(r2[i].srcList[k].gateway, gate);

				//	 puts(r2[i].srcList[k].gateway);

				strcpy(r2[i].srcList[k].macList, returnSrcPri->othermac);

				//puts(r2[i].srcList[k].macList);

				returnSrcPri++;
				r2[i].srcListLength++;
			}
		}
		else
		{
			for (k = 0; k < macSrcLen2; k++)
			{ //
				char macIpS[64] =
				{ };
				ipaddr2string(htonl(returnSrcPri->ip), macIpS, sizeof(macIpS));
				if (strcmp(ruleIpS, macIpS) == 0)
				{
					sprintf(ruleNameS, "%s", returnSrcPri->name);
					strcpy(r2[i].srcList[0].ipAddr, macIpS);

					//puts(r2[i].srcList[k].ipAddr);

					strcpy(r2[i].srcList[0].hostName, ruleNameS);
					// puts(r2[i].srcList[k].hostName);

					sprintf(temp_mask, "%d", returnSrcPri->nmask);

					mask_str = mask_turn(temp_mask);
					strcpy(r2[i].srcList[0].mask, mask_str);
					// puts(r2[i].srcList[k].mask);

					char mac_str[64] =
					{ };
					char mac_new[64] =
					{ };

					int n;
					n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new); //
					strcpy(r2[i].srcList[0].mac, mac_new);
					//	puts(r2[i].srcList[k].mac);

					char vip_str[64] =
					{ };
					char vip_new[64] =
					{ };
					if (returnSrcPri->vip != -1)
					{
						ipaddr2string(htonl(returnSrcPri->vip), vip_str,
								sizeof(vip_str)); //
						sprintf(vip_new, "%s", vip_str);

					}
					else
						memset(vip_new, 0, strlen(vip_new));

					strcpy(r2[i].srcList[0].natIp, vip_new);

					strcpy(r2[i].srcList[0].remark, returnSrcPri->comment);

					char gate[10] =
					{ };
					if (returnSrcPri->type == NODE_TYPE_GATEWAY)
						strcpy(gate, "G");
					else
						strcpy(gate, "H");
					strcpy(r2[i].srcList[0].gateway, gate);

					strcpy(r2[i].srcList[0].macList, returnSrcPri->othermac);
					r2[i].srcListLength++;
					break;
				}
				returnSrcPri++;

			}
		}

		if (strcmp(ALL, ruleIpD) == 0)
		{
			sprintf(ruleNameD, "%s", "ALL");
			for (k = 0; k < macDstLen2; k++)
			{
				char macIpD[64] =
				{ };
				ipaddr2string(htonl(returnDstPri->ip), macIpD, sizeof(macIpD));
				strcpy(r2[i].dstList[k].ipAddr, macIpD);

				sprintf(ruleNameD, "%s", returnDstPri->name);
				strcpy(r2[i].dstList[k].hostName, ruleNameD);

				sprintf(temp_mask, "%d", returnDstPri->nmask);
				mask_str = mask_turn(temp_mask);
				strcpy(r2[i].dstList[k].mask, mask_str);

				int n;
				char mac_str[64] =
				{ };
				char mac_new[64] =
				{ };
				n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
				mac_str[n] = 0;
				turn_mac(mac_str, mac_new); //
				strcpy(r2[i].dstList[k].mac, mac_new);

				char vip_str[64] =
				{ };
				char vip_new[64] =
				{ };
				if (returnDstPri->vip != -1)
				{
					ipaddr2string(htonl(returnDstPri->vip), vip_str,
							sizeof(vip_str)); //
					sprintf(vip_new, "%s", vip_str);
				}
				else
					strcpy(r2[i].dstList[k].natIp, vip_new);

				strcpy(r2[i].dstList[k].remark, returnDstPri->comment);
				char gate[10] =
				{ };
				if (returnDstPri->type == NODE_TYPE_GATEWAY)
					strcpy(gate, "G");
				else
					strcpy(gate, "H");
				strcpy(r2[i].dstList[k].gateway, gate);
				strcpy(r2[i].dstList[k].macList, returnDstPri->othermac);

				returnDstPri++;
				r2[i].dstListLength++;
			}

		}
		else
		{
			for (j = 0; j < macDstLen2; j++)
			{ //
				char macIpD[64];
				ipaddr2string(htonl(returnDstPri->ip), macIpD, sizeof(macIpD));
				if (strcmp(ruleIpD, macIpD) == 0)
				{
					sprintf(ruleNameD, "%s", returnDstPri->name);
					strcpy(r2[i].dstList[0].ipAddr, macIpD);
					strcpy(r2[i].dstList[0].hostName, ruleNameD);
					sprintf(temp_mask, "%d", returnDstPri->nmask);
					mask_str = mask_turn(temp_mask);
					strcpy(r2[i].dstList[0].mask, temp_mask);

					char mac_str[64] =
					{ };
					char mac_new[64] =
					{ };
					int n;
					n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new); //
					strcpy(r2[i].dstList[j].mac, mac_new);

					char vip_str[64] =
					{ };
					char vip_new[64] =
					{ };
					if (returnDstPri->vip != -1)
					{
						ipaddr2string(htonl(returnDstPri->vip), vip_str,
								sizeof(vip_str)); //
						sprintf(vip_new, "%s", vip_str);
					}
					else
						memset(vip_new, 0, strlen(vip_new));

					strcpy(r2[i].dstList[0].natIp, vip_new);

					strcpy(r2[i].dstList[0].remark, returnDstPri->comment);
					char gate[10] =
					{ };
					if (returnDstPri->type == NODE_TYPE_GATEWAY)
						strcpy(gate, "G");
					else
						strcpy(gate, "H");
					strcpy(r2[i].dstList[0].gateway, gate);
					strcpy(r2[i].dstList[0].macList, returnDstPri->othermac);
					r2[i].dstListLength++;

					break;
				}
				returnDstPri++;

			}
		}

		returnPri++;

	}

	if (Rule_to_XML(rl, xmlStr, ruleLen, r2, ruleLen2) < 0)
	{
		perror("parse error!");
		return -1;
	}

//
//	printf("Content-Type: text/xml;charset=utf-8");
//	puts(xmlStr);
	free(rl);
	free(r2);
	return 0;

}

/*********************
 *
 * get local ip for eth0
 *
 *
 *
 **********************/
int getlocalhostip(char *ip)
{

	char info[200];
	char tmp2[100];
	FILE *fp = popen("ifconfig eth0|grep 'inet addr'", "r");
	if(fp == NULL)
	{
		return -1;
	}
	fgets(info, 200, fp);
	pclose(fp);
	strtok(info, ":");
	strcpy(tmp2, strtok(NULL, ":"));
	strtok(NULL, ":");
	strtok(NULL, ":");
	strcpy(ip, strtok(tmp2, "  "));
	return 0;

}

/****************
 *
 * get Mask
 *
 ****************/

int getlocalmask(char *mask)
{

	char info[200];
	char tmp2[100];
	FILE *fp = popen("ifconfig eth0|grep 'Mask'", "r");
	if(fp == NULL)
	{
		return -1;
	}
	fgets(info, 200, fp);
	pclose(fp);
	strtok(info, ":");
	strtok(NULL, ":");
	strtok(NULL, ":");
	strcpy(tmp2, strtok(NULL, ":"));
	strcpy(mask, tmp2);
	return 0;

}
/**
 *
 * 获取得当前系统时间?yyyy-MM-dd HH:mm:SS
 */
void getSysTime(char *tm)
{

	struct tm * tm_ptr;
	time_t the_time;

	(void) time(&the_time); //
	tm_ptr = localtime(&the_time); //

	sprintf(tm, "%02d-%02d-%02d %02d:%02d:%02d", tm_ptr->tm_year + 1900,
			tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour,
			tm_ptr->tm_min, tm_ptr->tm_sec);

}

/**
 *
 * 获取得当前系统时间?yyyy-MM-dd HH:mm:SS
 */
void getSysTime1(char *tm)
{

	struct tm * tm_ptr;
	time_t the_time;

	(void) time(&the_time); //
	tm_ptr = localtime(&the_time); //

	sprintf(tm, "%02d%02d%02d%02d%02d%02d", tm_ptr->tm_year + 1900,
			tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour,
			tm_ptr->tm_min, tm_ptr->tm_sec);

}

/******************
 *
 * get uuid
 *
 *
 ****************/

int getUUID(char *uuid)
{

	char uuidstr[64];
        uuid_t uu;
	memset(uuidstr, 0, 64);
        uuid_generate(uu);
        uuid_unparse(uu,uuidstr);
        sprintf(uuid, "%s", uuidstr);;

//	char tm[30] =
//	{ 0 };
//	getSysTime1(tm);
//
//	sprintf(uuid, "%s%d", tm, getRandom());
	return 0;
}

/*****************
 *
 * get validrate from net adapter
 *
 ******************/
void getBytes(char *res, char *bytes){
	char tmp[200] = {0};
	strcpy(tmp, res);

	char txBytes[100]={0};
	strtok(tmp,":");
	strtok(NULL,":");
	strcpy(txBytes,strtok(NULL,":"));
	strcpy(bytes, strtok(txBytes,"("));
}
/***********
 *
 * get validrate
 *
 ************/

void getRate(char *validrate){
	//[          RX bytes:84754 (82.7 Kb)  TX bytes:11169 (10.9 Kb)]

	char res1[200] = {0};
	char res2[200] = {0};
	FILE *fp = popen("ifconfig eth1|grep 'TX bytes'","r");
	fgets(res1,200,fp);
	pclose(fp);
	fp = popen("ifconfig eth2|grep 'TX bytes'","r");
	fgets(res2,200,fp);
	pclose(fp);

	char *byte1=malloc(100);
	char *byte2=malloc(100);
	getBytes(res1,byte1);
	getBytes(res2,byte2);

	float b1 = atof(byte1);
	float b2 = atof(byte2);
//	printf("bytes1 is [%d]\n", b1+b2);
//	printf("bytes2 is [%d]\n", b2);
	char tmp[100]={0};
	sprintf(tmp, "%.2f", b1+b2);
	//printf("=============tmp is [%.2f]\n",b1+b2);
	strncpy(validrate, tmp, strlen(tmp)-3);
//	sprintf(validrate, "%.2f", b1+b2);
	//printf("validrate is [%s]\n", validrate);
	free(byte1);
	free(byte2);

}

void stopPassSh(){
	char cmd[50]={0};
	sprintf(cmd, "killall  sh -c 1>/dev/null 2>/dev/null");
	system(cmd);
}

void runPassSh(){
	system("./sh_bypass.sh 1>/dev/null 2>/dev/null &");
}

void stopReportSh(){
	char cmd[50]={0};
	sprintf(cmd, "killall  sh -c 1>/dev/null 2>/dev/null");
	system(cmd);
}

void runReportSh(int timeSec){
	char cmd[50]={0};
	sprintf(cmd, "./sh_round.sh %d 1>/dev/null 2>/dev/null &", timeSec);
//	char tmp[100]={0};
//	sprintf(tmp, "echo '%s' > comin", cmd);
	system(cmd);
//	system(tmp);
}

int getRandom(){
	int j,i;
	srand((int)time(0));
//	for(i = 0 ;i< 10 ;i++){
	j = 1+(int)(10000.0*rand()/(RAND_MAX+1.0));

	printf("rand is [%d]\n", j);
	return j;
//	}
}
