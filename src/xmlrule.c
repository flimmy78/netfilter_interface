/***
 * 	for Rule List 列表页面展示
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgic.h"
#include "envfile.h"
#include "dfgap_cfg_act.h"
#include "prog/log/cfg2xml.h"


int cgiMain(){

	char xmlStr[LINEMAX];
	char channelType[10];

	init_dfgap_cfg ();
	get_dfgap_cfg (NetCfgFile);


	printf("Content-Type: text/xml;charset=utf-8\n\n");
	xmlSaveFormatFileEnc("-",pdoc,"UTF-8",0);
	clean_dfgap_cfg ();
	return 0;
err:
  clean_dfgap_cfg ();
  return -1;
}

/***
 * 组装需要展示的XML对象
 */
xmlDocPtr ruleshow(char* xmlStr, char *src, char *dst, char *rule){
	int i;
	int numi=0;
	int curpage = 1;
	int recordsperpage = 20;
	int startnum = 0;
	char num_tmp[3];
	char temp_mask[10];



	//read xml from request


	qproot_node = xmlDocGetRootElement(qpdoc);
	qpnode = qproot_node->xmlChildrenNode;
	qpnode = xml_getnodebyname(qpnode,"QUERYCONDITION");
	value = xmlGetProp(qpnode, BAD_CAST "currentpagenum");
	curpage = atoi(value);
	value = xmlGetProp(qpnode, BAD_CAST "recordsperpage");
	recordsperpage = atoi(value);
	startnum = recordsperpage*(curpage - 1)+1;
	//create a new xml document
	pdoc = xmlNewDoc (BAD_CAST "1.0");
	proot_node = xmlNewNode (NULL, BAD_CAST "EFSFRAME");
	xmlDocSetRootElement (pdoc, proot_node);
	pnode = xmlNewNode (NULL, BAD_CAST "QUERYINFO");
	xmlAddChild (proot_node, pnode);
	memset(num_tmp,'\0',sizeof(num_tmp));
	memset(temp_mask,'\0',sizeof(temp_mask));

	int ruleLen;//规则结构体数组长度
	int macSrcLen;//源Mac结构体数组长度
	int macDstLen;//目的Mac结构体数组长度
	struct hostnode_struct *returnSrcPri;//源Mac结构体数组头指针
	struct hostnode_struct *returnDstPri;//目的Mac结构体数组头指针
	struct rule_struct *returnPri;//规则结构体数组头指针
	returnPri = getRuleStruct(rule);
	macSrcLen = getStructLen(src);
	macDstLen = getStructLen(dst);
	ruleLen = getRuleStructLen(rule);

	for(i=0 ; i<ruleLen ; i++ ){//循环规则表结构体数组
		returnSrcPri = getStruct(src);
		returnDstPri = getStruct(dst);
		sprintf(num_tmp,"%d",++numi);
		if(numi >= startnum && numi < (startnum + recordsperpage)){//是否符合传入页码
			char ruleIpS[LINEMAX] ={};
			char ruleIpD[64] ={};
			char ruleNameS[128] = {};
			char ruleNameD[128] = {};
			char ruleProt[64] = {};
			char rulePort[64] = {};
			char appCheck[64] = {};
			int k = 0;
			int j = 0;
			ipaddr2string(htonl(returnPri->sip),ruleIpS,sizeof(ruleIpS));
			ipaddr2string(htonl(returnPri->dip),ruleIpD,sizeof(ruleIpD));
			sprintf(ruleProt,"%d",returnPri->proto);
			if(strcmp(ruleProt,"1") == 0){
				strcpy(ruleProt,"TCP");
			}else if(strcmp(ruleProt,"2") == 0){
				strcpy(ruleProt,"UDP");
			}else if(strcmp(ruleProt,"5") == 0){
				strcpy(ruleProt,"ALL");
			}else {
				strcpy(ruleProt,"ERROR");
			}
			sprintf(rulePort,"%d",returnPri->dport);
			sprintf(appCheck,"%d",returnPri->appcheck);
			if(strcmp(appCheck,"1") == 0){
				strcpy(appCheck,"Y");
			}else if(strcmp(appCheck,"0") == 0){
				strcpy(appCheck,"N");
			}else {
				strcpy(appCheck,"ERROR");
			}



			if(strcmp(ALL,ruleIpS) == 0){//规则表源地址是否是全匹配
				sprintf(ruleNameS,"%s","ALL");
			}else{
				for(k = 0; k < macSrcLen; k++){//找出源MAC 中匹配的Ip并返回其对应的名称
					char macIpS[64]={};
					ipaddr2string(htonl(returnSrcPri->ip),macIpS,sizeof(macIpS));
					if(strcmp(ruleIpS,macIpS) == 0){
						sprintf(ruleNameS,"%s",returnSrcPri->name);
						break;
					}
					returnSrcPri++;
				}
			}
			if(strcmp(ALL,ruleIpD) == 0){//规则表目的地址是否是全匹配
				sprintf(ruleNameD,"%s","ALL");
			}else{
				for(j = 0; j < macDstLen; j++){//找出目的MAC 中匹配的Ip并返回其对应的名称
					char macIpD[64];
					ipaddr2string(htonl(returnDstPri->ip),macIpD,sizeof(macIpD));
					if(strcmp(ruleIpD,macIpD) == 0){
						sprintf(ruleNameD,"%s",returnDstPri->name);
						break;
					}
					returnDstPri++;
				}
			}
			pnode1 = xmlNewNode (NULL, BAD_CAST "ROW");
			xmlAddChild (pnode, pnode1);//添加子节点
			xmlNewChild(pnode1,NULL, BAD_CAST "ROWNUM", num_tmp);
			xmlNewChild(pnode1,NULL, BAD_CAST "SIP", ruleNameS);
			xmlNewChild(pnode1,NULL, BAD_CAST "DIP", ruleNameD);
			xmlNewChild(pnode1,NULL, BAD_CAST "PROT", ruleProt);
			xmlNewChild(pnode1,NULL, BAD_CAST "PORT", rulePort);
			xmlNewChild(pnode1,NULL, BAD_CAST "APPCHECK", appCheck);
		}
		returnPri++;
	}
	xmlNewProp(pnode, BAD_CAST "records", num_tmp);
	if(i%recordsperpage == 0){
		sprintf(num_tmp,"%d",i/recordsperpage);
	}else{
		sprintf(num_tmp,"%d",i/recordsperpage+1);
	}
	xmlNewProp(pnode, BAD_CAST "totalpages",BAD_CAST num_tmp);
	xml_cleanup(qpdoc);
	return pdoc;
}
