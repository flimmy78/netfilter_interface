#include"interface.h" 
#include"bo.h"
int checkPost_common(Data *ds, char *xmlstr) /*  */
{
//	puts(xmlstr);

	if (xmlstr == NULL) {
		strcpy(ds->direct, "0");
		strcpy(ds->cmd, "0");
		strcpy(ds->subcmd, "0000");
		return -1;
	}

	mxml_node_t *tree = NULL;
	mxml_node_t *data = NULL;
	mxml_node_t *src_sms_id = NULL;
	mxml_node_t *dev = NULL;
	mxml_node_t *dest_sms_id = NULL;
	mxml_node_t *dev_id = NULL;
	mxml_node_t *new_ccenter_addr = NULL;

	mxml_node_t *smsaddr = NULL;
	mxml_node_t *smsport = NULL;
	mxml_node_t *deviceid = NULL;
	mxml_node_t *heartbeatinterval = NULL;

	mxml_node_t *id = NULL;
	mxml_node_t *smsip = NULL;
	mxml_node_t *smsid = NULL;

	mxml_node_t *sysint = NULL;


	tree = mxmlLoadString(NULL, xmlstr, MXML_TEXT_CALLBACK);
	if (tree == NULL) {

		return -1;
	}
	data = mxmlFindElement(tree, tree, "data", NULL, NULL, MXML_DESCEND);
	if (data == NULL) {
		return -1;
	}

	char direct[2] = "0";
	char cmd[5] = "0";
	char subcmd[2] = "0";
	int n, nd, nc, ns;
	nd = strlen(mxmlElementGetAttr(data, "direct"));
	if (nd == 0) {
		return -1;
	}
	strcpy(direct, mxmlElementGetAttr(data, "direct"));

	nc = strlen(mxmlElementGetAttr(data, "cmd"));
	if (nc == 0) {
		return -1;
	}

	strcpy(cmd, mxmlElementGetAttr(data, "cmd"));

	ns = strlen(mxmlElementGetAttr(data, "subcmd"));
	if (ns == 0) {
		return -1;
	}
	strcpy(subcmd, mxmlElementGetAttr(data, "subcmd"));

	strcpy(ds->direct, direct);
	strcpy(ds->cmd, cmd);
	strcpy(ds->subcmd, subcmd);
//
	src_sms_id = mxmlFindElement(data, tree, "src_sms_id", NULL, NULL,
			MXML_DESCEND);
	if (src_sms_id != NULL) {
		strcpy(ds->src_sms_id, mxmlGetText(src_sms_id, 0));
	} else {
		strcpy(ds->src_sms_id, "0");
	}
	dest_sms_id = mxmlFindElement(data, tree, "dest_sms_id", NULL, NULL,
			MXML_DESCEND);
	if (dest_sms_id != NULL) {
		strcpy(ds->dest_sms_id, mxmlGetText(dest_sms_id, 0));
	} else {
		strcpy(ds->dest_sms_id, "0");
	}
	dev_id = mxmlFindElement(data, tree, "dev_id", NULL, NULL, MXML_DESCEND);
	if (dev_id != NULL) {
		strcpy(ds->dev_id, mxmlGetText(dev_id, 0));
	} else {
		strcpy(ds->dev_id, "0");
	}

//add  and set dev
	dev = mxmlFindElement(data, tree, "dev", NULL, NULL, MXML_DESCEND);
	if (dev != NULL) {

		// add dev
		if (NULL != mxmlElementGetAttr(dev, "id")) {
			strcpy(ds->id, mxmlElementGetAttr(dev, "id"));
		}


		if (NULL != mxmlElementGetAttr(dev, "smsid")) {
			strcpy(ds->smsid, mxmlElementGetAttr(dev, "smsid"));
		}

		if (NULL != mxmlElementGetAttr(dev, "smsip")) {
			strcpy(ds->smsip, mxmlElementGetAttr(dev, "smsip"));
		}

		// set baseinfo
			sysint = mxmlFindElement(dev, tree, "sysint", NULL, NULL, MXML_DESCEND);
			if(NULL != sysint)
			{

			smsaddr = mxmlFindElement(data, tree, "smsaddr", NULL, NULL, MXML_DESCEND);
			if (smsaddr != NULL) {
				strcpy(ds->smsaddr, mxmlGetText(smsaddr, 0));
			}

			smsport = mxmlFindElement(data, tree, "smsport", NULL, NULL, MXML_DESCEND);
			if (smsport != NULL) {
				int n;
				n = atoi(mxmlGetText(smsport, 0));
				ds->smsport = n;
			}

			deviceid = mxmlFindElement(data, tree, "deviceid", NULL, NULL,
					MXML_DESCEND);
			if (deviceid != NULL) {
				strcpy(ds->deviceid, mxmlGetText(deviceid, 0));
			}

			heartbeatinterval = mxmlFindElement(data, tree, "heartbeatinterval", NULL,
					NULL, MXML_DESCEND);
			if (heartbeatinterval != NULL) {
				int n;
				n = atoi(mxmlGetText(heartbeatinterval, 0));
				if (n > 0) {
					ds->heartbeatinterval = n;
				}
			}

			}
	}
// modify ip
	new_ccenter_addr = mxmlFindElement(data, tree, "new_ccenter_addr", NULL,
			NULL, MXML_DESCEND);

	if (new_ccenter_addr != NULL) {
		int n;
		n = strlen(mxmlElementGetAttr(new_ccenter_addr, "ip"));

		if (!n) {
			printf("new_ccenter_addr ip is null!\n");
			return -1;
		}
		strcpy(ds->new_ccenter_addrip,
				mxmlElementGetAttr(new_ccenter_addr, "ip"));
	}



	return 0;
}

int checkdata(Data ds) {

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5652") // get baseinfo
			&& !strcmp(ds.subcmd, "1")) {
		BaseInfo *bi;
		bi = (BaseInfo *) malloc(sizeof(BaseInfo));
		if (parse_deviceinfoxml(DEVICEINFO, bi) < 0) {
			free(bi);
			return -1;
		}
		if (!strcmp(bi->id, ds.dev_id)
				&& !strcmp(bi->sysint.smsid, ds.dest_sms_id)) {
			free(bi);
			return 0;
		}
		return -1;

	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5651") // add
			&& !strcmp(ds.subcmd, "1")) {

		return 1;
	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5651") //confirm
			&& !strcmp(ds.subcmd, "2")) {
		return 2;
	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5651") // delete
			&& !strcmp(ds.subcmd, "3")) {
		return 3;
	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5651") //  modify
			&& !strcmp(ds.subcmd, "4")) {
		return 4;
	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5652") // set
			&& !strcmp(ds.subcmd, "2")) {
		return 5;
	}

	if (!strcmp(ds.direct, "4") && !strcmp(ds.cmd, "5653") //get rule
			&& !strcmp(ds.subcmd, "1")) {
		return 6;
	}

	return -1;

}

int main() {

	// test by hand

	// add dev
//	char *data =
//			"<?xml version=\"1.0\" endcoding=\"utf-8\" ?> <data direct=\"4\" cmd =\"5651\" subcmd=\"1\" > \
//			<dev id=\"123\" smsip=\"32.45.66.33\" smsid=\"qqqqqqqqqqqqqqqqqqq\" /> \
//			</data>";
//	 get baseinfo
//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//<data direct=\"4\" cmd=\"5652\" subcmd=\"1\">  \
//<src_sms_id>123</src_sms_id>   \
//<dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id>    \
//<dev_id>123</dev_id>   \
//</data>" ;

	// confirm dev
//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//			<data direct=\"4\" cmd=\"5651\" subcmd=\"2\">  \
//			<src_sms_id>2dfs3f-fdsaf-fdsausddf</src_sms_id>   \
//			<dest_sms_id>2dfs3f-fdsaf-fdsausddf</dest_sms_id>    \
//			<dev_id>123456</dev_id>   \
//			</data>" ;

	//delete dev
// char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//<data direct=\"4\" cmd=\"5651\" subcmd=\"3\">  \
//<src_sms_id>2dfs3f-fdsaf-fdsausddf</src_sms_id>   \
//<dest_sms_id>2dfs3f-fdsaf-fdsausddf</dest_sms_id>    \
//<dev_id>123</dev_id>   \
//</data>" ;

	// set baseinfo
//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//<data direct=\"4\" cmd=\"5652\" subcmd=\"2\">  \
//<src_sms_id>qqqqqqqqqqqqqqqqqqq</src_sms_id>   \
//<dest_sms_id>qqqqqqqqqqqqqqqqqqq</dest_sms_id>    \
//<dev_id>123</dev_id>   \
//<dev id=\"123\" src_sms_id=\"qqqqqqqqqqqqqqqqqqq\"> \
//<sysint>  \
//<heartbeatinterval>4</heartbeatinterval>  \
//<smsaddr>11.11.11.11</smsaddr> \
//<smsport>443</smsport> \
//<deviceid>singleGap</deviceid> \
//</sysint> \
//</dev>  \
//</data>";

//	get rule
//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//<data direct=\"4\" cmd=\"5653\" subcmd=\"1\">  \
//<src_sms_id>2dfs3f-fdsaf-fdsausddf</src_sms_id>   \
//<dest_sms_id>2dfs3f-fdsaf-fdsausddf</dest_sms_id>    \
//<dev_id>123456</dev_id>   \
//</data>" ;

//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//		<data direct=\"4\" cmd=\"5653\" subcmd=\"1\">  \
//		<src_sms_id>80F1D6DE-D435-4C56-AADE-8A8A861A9D07</src_sms_id>   \
//		<dest_sms_id>80F1D6DE-D435-4C56-AADE-8A8A861A9D07</dest_sms_id>    \
//		<dev_id>C0EEAC32-32E7-4196-ADA7-35FD12A3DAD7</dev_id>   \
//		</data>" ;

	// modi sms ip
//	char *data = "<?xml version=\"1.0\" encoding=\"utf-8\"?>  \
//<data direct=\"4\" cmd=\"5651\" subcmd=\"4\">  \
//<src_sms_id>2dfs3f-fdsaf-fdsausddf</src_sms_id>   \
//<dest_sms_id>2dfs3f-fdsaf-fdsausddf</dest_sms_id>    \
//<dev_id>123456</dev_id>   \
//<new_ccenter_addr ip=\"192.168.1.1\" /> \
//</data>" ;

	Data ds;
	char *p;

	char *data = NULL;
	char *temp = NULL;
	int content_length = 0;
	p = getenv("CONTENT_LENGTH");
	if (NULL != p)
		content_length = atoi(p);

	data = malloc(content_length);
	temp = malloc(content_length);
	memset(data, '\0', content_length);
	memset(data, '\0', content_length);
	while (NULL != fgets(temp, content_length, stdin)) {
		strncat(data, temp, strlen(temp));
	}
	strcat(data, "\0");
	char cmdtest[1024] = { 0 };
	sprintf(cmdtest, "echo '%s' > /root/data", data);
	system(cmdtest);

	int recode = -1;
	int c;
	char *devxmlpath = DEVICEINFO;
	if (checkPost_common(&ds, data) < 0) {
		c = -1;
	}

	free(temp);
	free(data);

	printf("Content-Type: text/xml;charset=utf-8");
	c = checkdata(ds);
	switch (c) {
	case -3:
		recode = -3; //
		break;
	case -2:
		recode = -2; //
		break;
	case -1:
		recode = -1; //
		break;
	case 0:
		recode = get_baseinfo(devxmlpath);
		break;
	case 1:
		recode = add_safedev(ds, devxmlpath); // add
		break;
	case 2:
		recode = confirm_safedev(ds, devxmlpath);
//		printf_value("come","in");
		break;
	case 3:
		recode = del_safedev(ds, devxmlpath);
		break;
	case 4:
		recode = modi_safedevip(ds, devxmlpath);
		break;
	case 5:
		recode = set_baseinfo(ds, devxmlpath);
		break;
	case 6:
		recode = get_packfilter_rule(ds, devxmlpath);
		break;

	}

	switch (recode) {
	case 11:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\" %s \" cmd=\" %s \" subcmd=\" %s \" recode=\"%d\">  \
<errordesc> fail to read config-file.</errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 12:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 13:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;

	case 21:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 22:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 31:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 32:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>error </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 41:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> can not find something about safe manager. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 42:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> fail to read config-file. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 51:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>can not find something about safe manager. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 52:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> fail to read config-file. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 61:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>fail to read config file. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 62:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>fail to set config infomation  </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case 0:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\"?><data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\"></data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;

	case -1:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> fail to parse request message. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case -2:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc> fail to check safe manager info. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case -3:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>  fail to check device info. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;
	case -4:
		printf(
				"<?xml version=\"1.0\" encoding=\"utf-8\" ?>   \
<data direct=\"%s\" cmd=\"%s\" subcmd=\"%s\" recode=\"%d\">  \
<errordesc>unknown error. </errordesc> \
</data>",
				ds.direct, ds.cmd, ds.subcmd, recode);
		break;

	}

	return 0;
}

