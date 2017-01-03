/***
 *	for MAC IP List  ¡–±Ì“≥√Ê’π æ
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgic.h"
#include "envfile.h"
#include "dfgap_cfg_act.h"
#include "mxml.h" 
#include "interface.h" 

void printd(int d) {
	printf("%d", d);
}

int ListAllRule() {

	Rule *rule = NULL;
	rule = (Rule *) malloc(sizeof(Rule));

	struct hostnode_struct * returnPri;

	init_dfgap_cfg(); //≥ı ºªØdfgap
	get_dfgap_cfg(NetCfgFile); //∂¡»°dfgapŒƒº˛

	// SRC_MACT
	char mac_str[30];
	char ip_str[20];
	char mac_new[30];
	char gate[20];
	char vip_new[20];
	char vip_str[20];
	char other_mac[MAXSIZE];
	char other_new[MAXSIZE];
	int len = 0;

	len = getStruct_channel0_s_Len();
	returnPri = get_channel0_s_Struct();

	int i;
	strcpy(rule->channel, "0");

	for (i = 0; i < len; i++) {
		ipaddr2string(htonl(returnPri->ip), ip_str, sizeof(ip_str)); //◊™ªª≥…ipµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->srcList[i].ipAddr, ip_str);

		hex2asc(returnPri->mac, 6, mac_str, sizeof(mac_str));
		turn_mac(mac_str, mac_new); //◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->srcList[i].mac, mac_str);

		if (returnPri->type == NODE_TYPE_GATEWAY)
			strcpy(gate, "G");
		else
			strcpy(gate, "H");
		strcpy(rule->srcList[i].gateway, gate);
		strcpy(rule->srcList[i].hostName, returnPri->name);
		if (returnPri->vip != -1) {
			ipaddr2string(htonl(returnPri->vip), vip_str, sizeof(vip_str));	//◊™ªª≥…æ≤Ã¨ipµÿ÷∑◊÷∑˚¥Æ
			sprintf(vip_new, "%s", vip_str);
		} else {
			memset(vip_new, 0, strlen(vip_new));
		}
		hex2asc(returnPri->othermac, 6, other_mac, sizeof(other_mac));
		turn_mac(other_mac, other_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->srcList[i].macList, other_mac);
		strcpy(rule->srcList[i].natIp, vip_new);
		strcpy(rule->srcList[i].remark, "remark");
		
		returnPri ++;
		rule->srcListLength ++ ;
	}

	//DST_MACT
	len = getStruct_channel0_d_Len();
	returnPri = get_channel0_d_Struct();

	for (i = 0; i < len; i++) {
		ipaddr2string(htonl(returnPri->ip), ip_str, sizeof(ip_str));//◊™ªª≥…ipµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->dstList[i].ipAddr, ip_str);

		hex2asc(returnPri->mac, 6, mac_str, sizeof(mac_str));
		turn_mac(mac_str, mac_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->dstList[i].mac, mac_str);

		if (returnPri->type == NODE_TYPE_GATEWAY)
			strcpy(gate, "G");
		else
			strcpy(gate, "H");
		strcpy(rule->dstList[i].gateway, gate);
		strcpy(rule->dstList[i].hostName, returnPri->name);
		if (returnPri->vip != -1) {
			ipaddr2string(htonl(returnPri->vip), vip_str, sizeof(vip_str));	//◊™ªª≥…æ≤Ã¨ipµÿ÷∑◊÷∑˚¥Æ
			sprintf(vip_new, "%s", vip_str);
		} else {
			memset(vip_new, 0, strlen(vip_new));
		}
		hex2asc(returnPri->othermac, 6, other_mac, sizeof(other_mac));
		turn_mac(other_mac, other_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
		strcpy(rule->dstList[i].macList, other_mac);
		strcpy(rule->dstList[i].natIp, vip_new);
		strcpy(rule->dstList[i].remark, "remark");

		returnPri ++;
		rule->srcListLength ++ ;
	}

	//SRC_MACT
	len = getStruct_channel1_s_Len();
	returnPri = get_channel1_s_Struct();
	strcpy(rule->channel, "1");

	rule->srcListLength = 0 ;
	for (i = 0; i < len; i++) {
				ipaddr2string(htonl(returnPri->ip), ip_str, sizeof(ip_str));//◊™ªª≥…ipµÿ÷∑◊÷∑˚¥Æ
				strcpy(rule->srcList[i].ipAddr, ip_str);

				hex2asc(returnPri->mac, 6, mac_str, sizeof(mac_str));
				turn_mac(mac_str, mac_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
				strcpy(rule->srcList[i].mac, mac_str);

				if (returnPri->type == NODE_TYPE_GATEWAY)
					strcpy(gate, "G");
				else
					strcpy(gate, "H");
				strcpy(rule->srcList[i].gateway, gate);
				strcpy(rule->srcList[i].hostName, returnPri->name);
				if (returnPri->vip != -1) {
					ipaddr2string(htonl(returnPri->vip), vip_str, sizeof(vip_str));	//◊™ªª≥…æ≤Ã¨ipµÿ÷∑◊÷∑˚¥Æ
					sprintf(vip_new, "%s", vip_str);
				} else {
					memset(vip_new, 0, strlen(vip_new));
				}
				hex2asc(returnPri->othermac, 6, other_mac, sizeof(other_mac));
				turn_mac(other_mac, other_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
				strcpy(rule->srcList[i].macList, other_mac);
				strcpy(rule->srcList[i].natIp, vip_new);
				strcpy(rule->srcList[i].remark, "remark");

				returnPri ++;
				rule->srcListLength ++ ;
			}

	//SRC_MACT
	len = getStruct_channel1_d_Len();
	returnPri = get_channel1_d_Struct();
	for (i = 0; i < len; i++) {
			ipaddr2string(htonl(returnPri->ip), ip_str, sizeof(ip_str));//◊™ªª≥…ipµÿ÷∑◊÷∑˚¥Æ
			strcpy(rule->dstList[i].ipAddr, ip_str);

			hex2asc(returnPri->mac, 6, mac_str, sizeof(mac_str));
			turn_mac(mac_str, mac_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
			strcpy(rule->dstList[i].mac, mac_str);

			if (returnPri->type == NODE_TYPE_GATEWAY)
				strcpy(gate, "G");
			else
				strcpy(gate, "H");
			strcpy(rule->dstList[i].gateway, gate);
			strcpy(rule->dstList[i].hostName, returnPri->name);
			if (returnPri->vip != -1) {
				ipaddr2string(htonl(returnPri->vip), vip_str, sizeof(vip_str));	//◊™ªª≥…æ≤Ã¨ipµÿ÷∑◊÷∑˚¥Æ
				sprintf(vip_new, "%s", vip_str);
			} else {
				memset(vip_new, 0, strlen(vip_new));
			}
			hex2asc(returnPri->othermac, 6, other_mac, sizeof(other_mac));
			turn_mac(other_mac, other_new);	//◊™ªª≥…macµÿ÷∑◊÷∑˚¥Æ
			strcpy(rule->dstList[i].macList, other_mac);
			strcpy(rule->dstList[i].natIp, vip_new);
			strcpy(rule->dstList[i].remark, "remark");

			returnPri ++;
			rule->srcListLength ++ ;
		}

	char xmldata[MAXSIZE];

	Rule_to_XML(rule, xmldata);
	puts(xmldata);

	clean_dfgap_cfg();
	return 0;


}
