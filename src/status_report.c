/************************************************************

 锟斤拷锟斤拷锟斤拷锟斤拷模锟斤拷锟斤拷锟阶刺拷媳锟斤拷锟斤拷锟�
 锟斤拷锟竭ｏ拷锟斤拷锟斤拷

 锟斤拷锟节ｏ拷 2012-9-8

 *************************************************************/

#include "interface.h"

struct occupy
{
	char name[20];
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
};

#define IFCONFIG_DAT	"/tmp/ifconfig.dat"
#define PROC_MEMINFO	"/proc/meminfo"
#define DFINFO_DAT	"/tmp/dfinfo.dat"
#define PROC_STAT	"/proc/stat"

void print_value(char *name, char * value)
{
	int debug_flag = 0;
	char tmp[1256];
	if (debug_flag == 0)
	{
		sprintf(tmp, "echo \"%s = %s\" >> %s", name, value, "/var/www/aaa.txt");
		system(tmp);
	}

	return;
}


/*****************************************************
 *
 *取锟矫憋拷锟斤拷mac 锟斤拷址
 *ethname: 锟斤拷锟斤拷疲锟�锟界， eth0
 *mac_str: 锟斤拷mac锟斤拷址锟斤拷锟芥到mac_str
 *
 *****************************************************/
int get_mac(char *ethname, char *mac_str)
{

	struct ifreq ifreq;
	int sock;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		return -1;
	}
	strcpy(ifreq.ifr_name, ethname);
	if (ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
		perror("ioctl");
		return -1;
	}

	sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x\n",
			(unsigned char) ifreq.ifr_hwaddr.sa_data[0],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[1],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[2],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[3],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[4],
			(unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
	close(sock);
	return 0;
}
/****************************************************************************
 Usage: int get_occupy (struct occupy *o);

 Do: get CPU msg

 Parameters:
 struct occupy *o: CPU Struct

 Return: 0 锟斤拷锟斤拷 suc; -1 锟斤拷锟斤拷 fail
 *****************************************************************************/
int get_occupy(struct occupy *o)
{
	FILE *fd;
	int n;
	char buff[1024];

	fd = fopen(PROC_STAT, "r");
	if (fd == NULL)
		goto err;
	fgets(buff, sizeof(buff), fd);
	sscanf(buff, "%s %u %u %u %u", o->name, &o->user, &o->nice, &o->system,
			&o->idle);
	fclose(fd);

	return 0;
	err: return -1;
}

/****************************************************************************
 Usage: unsigned int cal_occupy (struct occupy *o, struct occupy *n);

 Do: count CPU %

 Parameters:
 struct occupy *o: CPU struct
 struct occupy *n: CPU struct

 Return: CPU% 锟斤拷锟斤拷 suc; 0 锟斤拷锟斤拷 fail
 *****************************************************************************/
unsigned int cal_occupy(struct occupy *o, struct occupy *n)
{
	unsigned int od, nd;
	unsigned int id, sd;

	if (o == NULL || n == NULL)
		goto err;
	od = o->user + o->nice + o->system + o->idle;

	nd = n->user + n->nice + n->system + n->idle;
	id = n->user - o->user;
	sd = n->system - o->system;
	return ((sd + id) * 100) / (nd - od);
	err: return 0;
}

/****************************************************************************
 Usage: int get_cpu_from_proc(unsigned int *cpu_percent);

 Do: 锟斤拷取CPU使锟斤拷锟斤拷

 Parameters:
 unsigned int *cpu_percent: CPU使锟斤拷锟绞憋拷锟斤拷指锟斤拷

 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 *****************************************************************************/
int get_cpu_from_proc(unsigned int *cpu_percent)
{
	struct occupy old, new;

	if (!cpu_percent)
		goto err;

	/* get cpu percent */
	//sleep(1);
	if (get_occupy(&old) < 0)
		goto err;
	usleep(100000);
	if (get_occupy(&new) < 0)
		goto err;
	*cpu_percent = cal_occupy(&old, &new);

	return 0;
	err: return -1;
}

/****************************************************************************
 Usage: int get_mem_from_proc(unsigned int *mem_percent);

 Do: 锟斤拷取锟节达拷使锟斤拷锟斤拷

 Parameters:
 unsigned int *mem_percent: 锟节达拷使锟斤拷锟绞憋拷锟斤拷指锟斤拷

 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 *****************************************************************************/
int get_mem_from_proc(unsigned int *mem_percent)
{
	unsigned int mem_total = 0, mem_free = 0;
	FILE *fp = NULL;
	char linebuf[1024];
	char dummy[128];

	if (!mem_percent)
		goto err;

	fp = fopen(PROC_MEMINFO, "r");
	if (fp == NULL)
		goto err;
	while (fgets(linebuf, sizeof(linebuf), fp))
	{
		if (strstr(linebuf, "MemTotal"))
		{
			sscanf(linebuf, "%s %u %s", dummy, &mem_total, dummy);
		}
		else if (strstr(linebuf, "MemFree"))
		{
			sscanf(linebuf, "%s %u %s", dummy, &mem_free, dummy);
		}
	}

	*mem_percent = (100 * (mem_total - mem_free)) / mem_total;
	fclose(fp);

	return 0;
	err: return -1;
}

/****************************************************************************
 Usage: int brg_show(void);

 Do: 锟斤拷锟絰ml锟斤拷

 Parameters:
 锟斤拷

 Return: 0锟斤拷锟缴癸拷;1锟斤拷失锟斤拷
 *****************************************************************************/
//int brg_show(void);
/****************************************************************************
 Usage: int get_rxtx_from_ifconfig(char *ifname, long long *rx_bytes, long long *tx_bytes);

 Do: 锟斤拷取锟斤拷锟斤拷辗锟斤拷锟斤拷锟斤拷锟斤拷息

 Parameters:
 char *ifname: 锟斤拷锟斤拷锟� long long *rx_bytes: 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟� long long *tx_bytes: 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟�
 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 *****************************************************************************/
int get_rxtx_from_ifconfig(char *ifname, long long *rx_bytes,
		long long *tx_bytes)
{
	char cmdbuf[1024];
	char linebuf[1024];
	FILE *fp = NULL;
	char dummy[64], rx_bytes_str[64], tx_bytes_str[64];
	char *p = NULL;

	if (ifname == NULL || !rx_bytes || !tx_bytes)
		goto err;

	sprintf(cmdbuf, "/sbin/ifconfig %s> %s 2>/dev/null", ifname, IFCONFIG_DAT);

	system(cmdbuf);

	fp = fopen(IFCONFIG_DAT, "r");
	if (fp == NULL)
		goto err;

	*rx_bytes = *tx_bytes = 0;
	while (fgets(linebuf, sizeof(linebuf), fp))
	{
		if (strstr(linebuf, "RX bytes") && strstr(linebuf, "TX bytes"))
		{
			sscanf(linebuf, "%s %s %s %s %s %s\n", dummy, rx_bytes_str, dummy,
					dummy, dummy, tx_bytes_str);
			p = strchr(rx_bytes_str, ':');

			if (p)
			{
				*rx_bytes = strtoll(p + 1, NULL, 10);
			}
			p = strchr(tx_bytes_str, ':');
			if (p)
			{
				*tx_bytes = strtoll(p + 1, NULL, 10);
			}
			//printf("%s rx:%Ld, tx:%Ld\n", ifname, *rx_bytes, *tx_bytes);
		}
	}

	fclose(fp);

	return 0;
	err: if (fp)
		fclose(fp);
	return -1;
}

/****************************************************************************
 Usage: int get_rxtx(unsigned int *rx_1_rate, unsigned int *tx_1_rate , unsigned int *rx_2_rate, unsigned int *tx_2_rate);

 Do: 锟斤拷取锟斤拷锟斤拷辗锟斤拷锟斤拷锟斤拷锟斤拷息

 Parameters:
 unsigned int *rx_1_rate: 锟斤拷锟�锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟� unsigned int *tx_1_rate: 锟斤拷锟�锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟� unsigned int *rx_2_rate: 锟斤拷锟�锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟� unsigned int *tx_2_rate: 锟斤拷锟�锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟�
 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 *****************************************************************************/
int get_rxtx(unsigned int *rx_1_rate, unsigned int *tx_1_rate,
		unsigned int *rx_2_rate, unsigned int *tx_2_rate)
{
	int res = 0;
	long long o_rx_1_bytes = 0;
	long long o_tx_1_bytes = 0;
	long long o_rx_2_bytes = 0;
	long long o_tx_2_bytes = 0;

	long long n_rx_1_bytes = 0;
	long long n_tx_1_bytes = 0;
	long long n_rx_2_bytes = 0;
	long long n_tx_2_bytes = 0;

	res = get_rxtx_from_ifconfig("eth1", &o_rx_1_bytes, &o_tx_1_bytes);
	if (res < 0)
		goto err;
	res = get_rxtx_from_ifconfig("eth2", &o_rx_2_bytes, &o_tx_2_bytes);
	if (res < 0)
		goto err;

	usleep(100000);

	res = get_rxtx_from_ifconfig("eth1", &n_rx_1_bytes, &n_tx_1_bytes);
	if (res < 0)
		goto err;
	res = get_rxtx_from_ifconfig("eth2", &n_rx_2_bytes, &n_tx_2_bytes);
	if (res < 0)
		goto err;

	*rx_1_rate = (n_rx_1_bytes - o_rx_1_bytes) / 8;
	*tx_1_rate = (n_tx_1_bytes - o_tx_1_bytes) / 8;
	*rx_2_rate = (n_rx_2_bytes - o_rx_2_bytes) / 8;
	*tx_2_rate = (n_tx_2_bytes - o_tx_2_bytes) / 8;

	return 0;

	err: return -1;
}

/****************************************************************************
 Usage: int get_disk_rate(unsigned int *rate);

 Do: 锟斤拷取硬锟斤拷使锟斤拷锟斤拷锟斤拷息

 Parameters:
 unsigned int *rate: 硬锟斤拷使锟斤拷锟绞憋拷锟斤拷指锟斤拷

 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 *****************************************************************************/
int get_disk_rate(unsigned int *rate)
{
	char cmdbuf[1024];
	char linebuf[1024];
	FILE *fp = NULL;
	char dummy[64], rate_str[64];

	if (rate == NULL)
		goto err;

	sprintf(cmdbuf, "df -Tk> %s 2>/dev/null", DFINFO_DAT);
	system(cmdbuf);

	fp = fopen(DFINFO_DAT, "r");
	if (fp == NULL)
		goto err;

	while (fgets(linebuf, sizeof(linebuf), fp))
	{
		if (strstr(linebuf, "/dev"))
		{
			sscanf(linebuf, "%s %s %s %s %s %s\n", dummy, dummy, dummy, dummy,
					dummy, rate_str);
			*rate = (unsigned int) atoi(rate_str);
			//printf("rate_str:%s \n", rate_str);
			//printf("rate:%d%% \n", *rate);
			break;
		}
	}

	fclose(fp);

	return 0;

	err:

	if (fp)
		fclose(fp);

	return -1;
}

/*锟斤拷印锟截达拷头*/
void print_header(int flag)
{
	if (flag == 0)
	{
		printf("Content-Type: text/xml;charset=utf-8\n\n");
	}
	else
	{
		printf("Content-Type: text/html;charset=utf-8\n\n");
	}

	return;
}

/****************************************************************************
 Usage: int get_status(char *xmldata)

 Do: 锟斤拷取状态锟斤拷锟斤拷拼装锟斤拷xml锟街凤拷

 Parameters:
 char xmldata: 锟斤拷xml锟街凤拷锟斤拷诖舜锟� int heartbeatinterval  : 锟斤拷锟斤拷锟斤拷
 Return: 0 锟斤拷锟斤拷 锟缴癸拷; -1 锟斤拷锟斤拷 失锟斤拷
 ******************************************************************************/

int get_status(char *ip, int *port, char *xmldata) // 锟斤拷取状态锟斤拷 拼锟斤拷xml 锟街凤拷
{
	char status[30] =
	{ 0 };
//	strcpy(status, "32011000000000000000000") ;

	char tmp[64] =
	{ 0 };

	unsigned int cpu_percent = 0;
	unsigned int mem_percent = 0;
	unsigned int disk_rate = 0;
	char dev_id[50];
	char managerip[20];
	int heartbeatinterval_num = 0;
//	int validrate = 0;
	getStatusCode(status);

	char *deviceabnormalreason = "abnormalreason"; // 锟借备锟斤拷锟斤拷锟皆拷锟�//	char ip[20];
	char mac[20];
	char event_type[50] = "FFFF";
	int res = 0;

	res = get_cpu_from_proc(&cpu_percent);
	if (res < 0)
	{
		cpu_percent = 0;
	}
	bzero(tmp, sizeof(tmp));
	sprintf(tmp, "%d", cpu_percent);

	res = get_mem_from_proc(&mem_percent);
	if (res < 0)
	{
		mem_percent = 0;
	}
	bzero(tmp, sizeof(tmp));
	sprintf(tmp, "%d", mem_percent);
	printf("mem_percent=%d\n", mem_percent);

	res = get_disk_rate(&disk_rate);
	if (res < 0)
	{
		disk_rate = 0;
	}
	bzero(tmp, sizeof(tmp));
	sprintf(tmp, "%d", disk_rate);

	res = get_mac("eth0", mac);
	if (res < 0)
	{
		strcpy(mac, "0");
	}

	/*
	 * 姝ゅ鍙畬鎴愯幏鍙栨湁鏁堟暟鎹祦閲忕殑涓氬姟閫昏緫
	 *
	 * */

	/*
	 * get validrate from net adapter.
	 */

	char *validrate=malloc(100);

	getRate(validrate);

	FILE *fp;
	mxml_node_t *xml;
	mxml_node_t *tree;
	mxml_node_t *data;
	mxml_node_t *dev;
	mxml_node_t *sysint;
	mxml_node_t *smsaddr;
	mxml_node_t *heartbeatinterval;
	mxml_node_t *sysinfo;

	mxml_node_t *dev_id_t;
	mxml_node_t *event_type_t;
	mxml_node_t *ip_t;
	mxml_node_t *port_t;

	mxml_node_t *mac_t;
	mxml_node_t *status_t;
	mxml_node_t *deviceabnormalreason_t;
	mxml_node_t *cpuusage_t;
	mxml_node_t *memusage_t;
	mxml_node_t *diskusage_t;
	mxml_node_t *validrate_t;
	mxml_node_t *managerip_t;

	if (NULL == (fp = fopen(DEVICEINFO, "r")))
	{
		perror("fail to open file!\n");
		return -1;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	if (tree == NULL)
	{
		perror("fail to load xml file.\n");
		return -1;
	}

	fclose(fp);

	data = mxmlFindElement(tree, tree, "data", NULL, NULL, MXML_DESCEND);
	if (data == NULL)
	{
		perror("can not find data node!, please check xml file.\n");
		return -1;
	}

	dev = mxmlFindElement(data, tree, "dev", NULL, NULL, MXML_DESCEND);
	if (dev == NULL)
	{
		perror("can not find dev node!, please check xml file.\n");
		return -1;
	}

	if (NULL == mxmlElementGetAttr(dev, "id"))
	{
		strcpy(dev_id, "1");
	}
	else
	{
		strcpy(dev_id, mxmlElementGetAttr(dev, "id"));
	}

	sysint = mxmlFindElement(dev, data, "sysint", NULL, NULL, MXML_DESCEND);
	if (sysint == NULL)
	{
		printf("can not find sysint node , please check xml file.!\n ");
		return -1;
	}

	smsaddr = mxmlFindElement(sysint, tree, "smsaddr", NULL, NULL,
			MXML_DESCEND);
	if (smsaddr == NULL)
	{
		memcpy(ip, '\0', 20);
	}
	else
	{
		strcpy(ip, mxmlGetText(smsaddr, 0));
	}

	port_t = mxmlFindElement(sysint, tree, "smsport", NULL, NULL, MXML_DESCEND);
	if (NULL == port_t)
	{
		*port = 0;
	}
	else
	{
		*port = atoi(mxmlGetText(port_t, 0));
	}

	heartbeatinterval = mxmlFindElement(sysint, tree, "heartbeatinterval", NULL,
			NULL, MXML_DESCEND);
	if (heartbeatinterval == NULL)
	{
		heartbeatinterval_num = 0;
	}
	else
	{
		heartbeatinterval_num = atoi(mxmlGetText(heartbeatinterval, 0));
	}
	sysinfo = mxmlFindElement(dev, data, "sysinfo", NULL, NULL, MXML_DESCEND);
	if (sysinfo == NULL)
	{
		printf("can not find sysinfo node , please check xml file.!\n ");
		return -1;
	}
//	validrate_t = mxmlFindElement(sysinfo, tree, "validrate", NULL, NULL,
//			MXML_DESCEND);
//	if (validrate_t == NULL)
//	{
//		validrate = 0;
//	}
//	else
//	{
//		validrate = atoi(mxmlGetText(validrate_t, 0));
//	}
	managerip_t = mxmlFindElement(sysinfo, tree, "managerip", NULL, NULL,
			MXML_DESCEND);
	if (managerip_t == NULL)
	{
		strcpy(managerip, "0");
	}
	else
	{
		strcpy(managerip, mxmlGetText(managerip_t, 0));
	}

	//

	xml = mxmlNewXML("1.0");

	data = mxmlNewElement(xml, "data");
	mxmlElementSetAttr(data, "direct", "5");
	mxmlElementSetAttr(data, "cmd", "5701");
	mxmlElementSetAttr(data, "subcmd", "1");

	event_type_t = mxmlNewElement(data, "event_type");

	mxmlNewText(event_type_t, 0, event_type);

	dev_id_t = mxmlNewElement(data, "id");

	mxmlNewText(dev_id_t, 0, dev_id);

	ip_t = mxmlNewElement(data, "ip");

	mxmlNewText(ip_t, 0, managerip);

	mac_t = mxmlNewElement(data, "mac");

	mxmlNewText(mac_t, 0, mac);

	status_t = mxmlNewElement(data, "status");

	mxmlNewText(status_t, 0, status);

	deviceabnormalreason_t = mxmlNewElement(data, "deviceabnormalreason");

	mxmlNewText(deviceabnormalreason_t, 0, deviceabnormalreason);

	cpuusage_t = mxmlNewElement(data, "cpuusage");

	mxmlNewInteger(cpuusage_t, cpu_percent);

	memusage_t = mxmlNewElement(data, "memusage");

	mxmlNewInteger(memusage_t, mem_percent);

	diskusage_t = mxmlNewElement(data, "diskusage");

	mxmlNewInteger(diskusage_t, disk_rate);

	validrate_t = mxmlNewElement(data, "validrate");
	mxmlNewText(validrate_t, 0, validrate);

	char buff[MAXSIZE];
	mxmlSaveString(xml, buff, MAXSIZE, MXML_NO_CALLBACK);
	strcpy(xmldata, buff);
	mxmlDelete(xml);

	return heartbeatinterval_num;
}

int getStatusCode(char *code)
{
	FILE *fp = NULL;
	fp = fopen("/home/jiekou/src/statuscode.txt", "r");
	if (fp == NULL)
	{
		printf("fail to open file\n");
		return;
	}
	fgets(code, 25, fp);
	printf("code is [%s]\n", code);
	if (!strlen(code))
	{
		printf("fail to get status code\n");
		return;
	}
	fclose(fp);
	return 1; // success to get status code.

}
int main()
{
	int heartbeatinterval;
	char *ip = malloc(30);
	int ret = 0;
	char *url = malloc(50);
	;
//	const char path[50] = CENTER_URL;
	char *xmldata = malloc(2048);
	int port = 0;

	int i;
	if (heartbeatinterval == 0)
	{
		return 0;
	}
	int tmpi = 0;
//	while (1)
//	{
//		printf("========================================[%d]\n",++tmpi);
	heartbeatinterval = get_status(ip, &port, xmldata);
	sprintf(url, "https://%s:%d/%s", ip, port, CENTER_URL);
	puts(url);
	ret = postUrl(url, xmldata);
	if (ret < 0)
	{
		printf("fail to post !\n");
	}
	puts(xmldata);

	free(xmldata);
	free(url);
	free(ip);
	//sleep(heartbeatinterval);
//		flush();
//		sync();
//	}
	return ret;

}
