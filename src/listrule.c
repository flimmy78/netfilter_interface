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

int main(){

	init_dfgap_cfg ();
	get_dfgap_cfg (NetCfgFile);

  int i;
	int numi=0;
	int curpage = 1;
	int recordsperpage = 20;
	int startnum = 0;
	char num_tmp[3];
	char temp_mask[10];
  
 	xmlChar *value;
 
	memset(num_tmp,'\0',sizeof(num_tmp));
	memset(temp_mask,'\0',sizeof(temp_mask));

	int ruleLen; 
	int macSrcLen; 
	int macDstLen; 
	struct hostnode_struct *returnSrcPri; 
	struct hostnode_struct *returnDstPri; 
	struct rule_struct *returnPri; 
	returnPri = getRuleStruct(rule);
	macSrcLen = getStructLen(src);
	macDstLen = getStructLen(dst);
	ruleLen = getRuleStructLen(rule);

	for(i=0 ; i<ruleLen ; i++ ){ 
		returnSrcPri = getStruct(src);
		returnDstPri = getStruct(dst);
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
			if(strcmp(ALL,ruleIpS) == 0){ 
				sprintf(ruleNameS,"%s","ALL");
			}else{
				for(k = 0; k < macSrcLen; k++){ 
					char macIpS[64]={};
					ipaddr2string(htonl(returnSrcPri->ip),macIpS,sizeof(macIpS));
					if(strcmp(ruleIpS,macIpS) == 0){
						sprintf(ruleNameS,"%s",returnSrcPri->name);
						break;
					}
					returnSrcPri++;
				}
			}
			if(strcmp(ALL,ruleIpD) == 0){ 
				sprintf(ruleNameD,"%s","ALL");
			}else{
				for(j = 0; j < macDstLen; j++){ 
					char macIpD[64];
					ipaddr2string(htonl(returnDstPri->ip),macIpD,sizeof(macIpD));
					if(strcmp(ruleIpD,macIpD) == 0){
						sprintf(ruleNameD,"%s",returnDstPri->name);
						break;
					}
					returnDstPri++;
				}
			}
		 
		returnPri++;
	}
	 
	clean_dfgap_cfg ();
	return 0;

}
