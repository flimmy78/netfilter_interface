/*******************************
 *
 * author : wanghe
 * date   : 2012-10-25
 * des    : auto check bypass exception while power on
 *
 **************************/

#include "interface.h"

/**************************************
 *
 * description : get bypass(net connect) status .
 *
 *
 *1 -- interface link up
 *0 -- interface link down.
 *
 *
 */

int get_bypass_status(char *netAdapter)
{
	char res[200] =
	{ 0 };
	char cmd[200] =
	{ 0 };
	sprintf(cmd, "ifconfig %s |grep 'RUNNING'", netAdapter);
	FILE *fp = popen(cmd, "r");
	fgets(res, 200, fp);
	pclose(fp);
	if (strlen(res))
	{
		return 1;
	}

	return 0;
}

/********************
 *
 * main enterance
 *
 ********************/
int main()
{
	int status = -1;
	BaseInfo *bi ;
	bi = malloc(sizeof(BaseInfo));
	if (parse_deviceinfoxml(DEVICEINFO, bi) < 0)
	{
		printf("error 1\n");
		free(bi);
		return 0;
	}

	if(!strcmp(bi->id, "0"))
	{
		printf("bi.id is [%s]\n", bi->id);
		printf("error 2\n");
		free(bi);
		return 0;
	}

	runPassSh();
//	system("./_bypassException 1>/dev/null 2>/dev/null &");

	runReportSh(bi->sysint.heartbeatinterval) ;

	free(bi);
	return 0;
}
