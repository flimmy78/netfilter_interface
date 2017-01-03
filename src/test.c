#include"interface.h"

int main(int argc, char **argv)
{
	//add
	char *data1 =
			"<?xml version=\"1.0\" endcoding=\"utf-8\" ?><data direct=\"4\" cmd =\"5651\" subcmd=\"1\" ><dev id=\"123\" smsip=\"192.168.3.165\" smsid=\"qqqqqqqqqqqqqqqqqqq\" /></data>";
	// get base info
	char *data2 =
			"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"4\" cmd=\"5652\" subcmd=\"1\"><src_sms_id>qqqqqqqqqqqqqqqqqqq</src_sms_id><dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id><dev_id>123</dev_id></data>";
	// confirm data
	char *data3 =
			"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"4\" cmd=\"5651\" subcmd=\"2\"><src_sms_id>E5224A02-BB6B-415A-9304-31FEFD5DB650</src_sms_id><dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id><dev_id>123</dev_id></data>";
	// delete data
	char *data4 =
			"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"4\" cmd=\"5651\" subcmd=\"3\"><src_sms_id>qqqqqqqqqqqqqqqqqqq</src_sms_id><dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id><dev_id>123</dev_id></data>";
	// get rule
	char *data5 =
			"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"4\" cmd=\"5653\" subcmd=\"1\"><src_sms_id>qqqqqqqqqqqqqqqqqqq</src_sms_id><dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id><dev_id>123</dev_id></data>";
	// modi sms ip
	char *data6 =
			"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"4\" cmd=\"5651\" subcmd=\"4\"><src_sms_id>qqqqqqqqqqqqqqqqqqq</src_sms_id><dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id><dev_id>123</dev_id><new_ccenter_addr ip=\"192.168.1.1\" /></data>";

	char xmldata[8192];
	char ip[20];
	char mask[20];
	const char *url = "https://192.168.3.166/cgi-bin/jiekou/bin/main.cgi";
	int c;
	while (c >= 0 || c <= 9)
	{
		printf("添加设备请按 1\n");
		printf("获取设备基本信息请按2\n");

		printf("确认设备请按3\n");
		printf("删除设备请按4\n");
		printf("包过滤策略请按5\n");
		printf("安管IP修改请按6\n");
		printf("旁路及状态上报请按7\n");
		printf("异常事件请按8\n");
		printf("策略修改请按9\n");

		scanf("%d", &c);
		break;
	}
	switch (c)
	{
	case 1:
		postUrl(url, data1);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 2:
		postUrl(url, data2);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 3:
		postUrl(url, data3);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 4:
		postUrl(url, data4);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 5:
		postUrl(url, data5);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 6:
		postUrl(url, data6);
		system("cat /home/jiekou/src/deviceinfo.xml");
		break;
	case 7:
		system("./sh_round.sh 1");
		break;
	case 8:
		system("./_checkException mod	'0RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|'");
		break;
	case 9:
		system("./_checkPolicy mod  '0;RULE;1;hoscc|aaaa|TCP|23|N|ACCEPT|N|N|'");
		break;
	}



}

int main1()
{

}
