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
#include "interface.h" 
int ruleshow(char* xmlStr, char *src, char *dst, char *rule) ;


int get_packfilter_rule(){
	 
	char xmlStr[MAXSIZE];
	char channelType[10];

	strcpy(channelType, CHANNEL0_RULE); 
	init_dfgap_cfg ();
	get_dfgap_cfg (NetCfgFile);

	if(strcmp(CHANNEL0_RULE, channelType) == 0){
	 ruleshow(xmlStr,CHANNEL0_SRC,CHANNEL0_DST,CHANNEL0_RULE);
	 perror("!!!!!!");
	 strcpy(channelType, "1"); 
	 
	}else if(strcmp(CHANNEL1_RULE, channelType) == 0){
		ruleshow(xmlStr,CHANNEL1_SRC,CHANNEL1_DST,CHANNEL1_RULE);
	}
	
	clean_dfgap_cfg ();
	return 0;
 
}

/***
 * 组装需要展示的XML对象
 */
int ruleshow(char* xmlStr, char *src, char *dst, char *rule){
	int i;
	int numi=0;
	//int curpage = 1;
	 
	char num_tmp[3];
	Rule *rl ; 
	rl = ( Rule *) malloc(sizeof(Rule)) ; 
	rl->srcListLength = 0 ; 
	rl->dstListLength = 0 ; 

	memset(num_tmp,'\0',sizeof(num_tmp));
	
	if( strcmp(rule, CHANNEL0_RULE) == 0) 
		{
			strcpy(rl->channel, "0") ; 
		} 
	
	if( strcmp(rule, CHANNEL1_RULE) == 0) 
		{
			strcpy(rl->channel, "1") ; 
		} 
	 
	
		
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
					
			char temp_mask[20]; 
			char *mask_str; 
			memset(num_tmp,'\0',sizeof(num_tmp));
			memset(temp_mask,'\0',sizeof(temp_mask));
			
			if(strcmp(ALL,ruleIpS) == 0){//规则表源地址是否是全匹配
				sprintf(ruleNameS,"%s","ALL");
				for(k=0; k<macSrcLen ; k++ ) 
				{
					char macIpS[64]={};
					ipaddr2string(htonl(returnSrcPri->ip),macIpS,sizeof(macIpS));
					strcpy(rl->srcList[k].ipAddr, macIpS); 
				//	puts(rl->srcList[k].ipAddr) ;
					sprintf(ruleNameS,"%s",returnSrcPri->name);
					strcpy(rl->srcList[k].hostName, ruleNameS); 
    			// 	puts(rl->srcList[k].hostName) ;
					sprintf(temp_mask,"%d",returnSrcPri->nmask);
					mask_str=mask_turn(temp_mask);
					strcpy(rl->srcList[k].mask, mask_str) ;
				 //   puts(rl->srcList[k].mask) ;
					int n ; 
					char mac_str[64] = {};
					char mac_new[64] = {} ; 
					n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));
				  
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new);//转换成mac地址字符串
					strcpy(rl->srcList[k].mac, mac_new) ;			
				//	 puts(rl->srcList[k].mac) ;
					 
					char vip_str[64] = {};
					char vip_new[64] = {};
					if(returnSrcPri->vip != -1)
					{
						ipaddr2string(htonl(returnSrcPri->vip), vip_str, sizeof(vip_str));	//转换成静态ip地址字符串
						sprintf(vip_new, "%s", vip_str);
					  
					}
					else
						memset(vip_new , 0 , strlen(vip_new));
						
						strcpy(rl->srcList[k].natIp, vip_new) ;	
				//	  puts(rl->srcList[k].natIp); 
					
						strcpy(rl->srcList[k].remark, returnSrcPri->comment) ;		
					//		 puts(rl->srcList[k].remark); 			
							 		
						char gate[10]={}; 
						if(returnSrcPri->type==NODE_TYPE_GATEWAY)
							strcpy(gate, "G");
						else
							strcpy(gate, "H");
						strcpy(rl->srcList[k].gateway, gate) ;
						
					//	 puts(rl->srcList[k].gateway); 	
						 
						strcpy(rl->srcList[k].macList, returnSrcPri->othermac) ;
						
						 //puts(rl->srcList[k].macList); 	
					
					returnSrcPri ++ ;
					rl->srcListLength ++ ;
				}
				
				
									
				
			}else{
				for(k = 0; k < macSrcLen; k++){//找出源MAC 中匹配的Ip并返回其对应的名称
					char macIpS[64]={};
					ipaddr2string(htonl(returnSrcPri->ip),macIpS,sizeof(macIpS));
					if(strcmp(ruleIpS,macIpS) == 0){
						sprintf(ruleNameS,"%s",returnSrcPri->name);
						strcpy(rl->srcList[k].ipAddr, macIpS) ; 
					
						//puts(rl->srcList[k].ipAddr); 	
						
						strcpy(rl->srcList[k].hostName, ruleNameS) ;
						// puts(rl->srcList[k].hostName); 
						 
						sprintf(temp_mask,"%d",returnSrcPri->nmask);
						
						mask_str = mask_turn(temp_mask);
						strcpy(rl->srcList[k].mask, mask_str) ;
						// puts(rl->srcList[k].mask); 
						 
						
						char mac_str[64] = {};
						char mac_new[64] = {}; 
					
						
						int n ; 
						n = hex2asc(returnSrcPri->mac, 6, mac_str, sizeof(mac_str));			
						mac_str[n] = 0;
						turn_mac(mac_str, mac_new);//转换成mac地址字符串
						strcpy(rl->srcList[i].mac, mac_new) ;
					//	puts(rl->srcList[k].mac); 
						
						char vip_str[64] = {};
						char vip_new[64] = {};
						if(returnSrcPri->vip != -1)
						{
							ipaddr2string(htonl(returnSrcPri->vip), vip_str, sizeof(vip_str));	//转换成静态ip地址字符串
							sprintf(vip_new, "%s", vip_str);
								
						}
						else
							memset(vip_new , 0 , strlen(vip_new));
							
						strcpy(rl->srcList[i].natIp, vip_new) ;
					//	puts(rl->srcList[k].natIp); 
						
						strcpy(rl->srcList[k].remark, returnSrcPri->comment) ;
						//puts(rl->srcList[k].remark); 
												
						char gate[10] = {}; 
						if(returnSrcPri->type==NODE_TYPE_GATEWAY)
							strcpy(gate, "G");
						else
							strcpy(gate, "H");
						strcpy(rl->srcList[k].gateway, gate) ;
						
					//	puts(rl->srcList[k].gateway);
						
						strcpy(rl->srcList[k].macList, returnSrcPri->othermac) ;
						//puts(rl->srcList[k].macList);
						break;
					}
					returnSrcPri++;
					rl->srcListLength ++ ;
				}
			}
			
			if(strcmp(ALL,ruleIpD) == 0){//规则表目的地址是否是全匹配
				sprintf(ruleNameD,"%s","ALL");
				for(k=0; k<macDstLen; k++ ) 
				{
					char macIpS[64]={};
					ipaddr2string(htonl(returnDstPri->ip),macIpS,sizeof(macIpS));
					strcpy(rl->srcList[k].ipAddr, macIpS); 
				//	puts(rl->srcList[k].ipAddr);
					
					sprintf(ruleNameS,"%s",returnDstPri->name);
					strcpy(rl->srcList[k].hostName, ruleNameS); 
				//	puts(rl->srcList[k].hostName);
					
					sprintf(temp_mask,"%d",returnDstPri->nmask);
					mask_str = mask_turn(temp_mask);
					strcpy(rl->dstList[k].mask, mask_str) ;
				//		puts(rl->srcList[k].mask);
				 
					int n ; 
					char mac_str[64] = {};
					char mac_new[64] = {} ;
					n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
					mac_str[n] = 0;
					turn_mac(mac_str, mac_new);//转换成mac地址字符串
					strcpy(rl->dstList[k].mac, mac_new) ;			
					
					char vip_str[64] = {};
					char vip_new[64] = {};
					if(returnDstPri->vip != -1)
					{
						ipaddr2string(htonl(returnDstPri->vip), vip_str, sizeof(vip_str));	//转换成静态ip地址字符串
						sprintf(vip_new, "%s", vip_str);
					}
					else
						memset(vip_new , 0 , strlen(vip_new));
					strcpy(rl->dstList[k].natIp, vip_new) ;
					
					strcpy(rl->dstList[k].remark, returnDstPri->comment) ;		
					char gate[10] = {}; 						
					if(returnDstPri->type==NODE_TYPE_GATEWAY)
						strcpy(gate, "G");
					else
						strcpy(gate, "H");
					strcpy(rl->dstList[k].gateway, gate) ;
					strcpy(rl->dstList[k].macList, returnDstPri->othermac) ;
					
					returnDstPri ++ ;
					rl->dstListLength ++ ;
				}
				
			}else{
				for(j = 0; j < macDstLen; j++){//找出目的MAC 中匹配的Ip并返回其对应的名称
					char macIpD[64];
					ipaddr2string(htonl(returnDstPri->ip),macIpD,sizeof(macIpD));
					if(strcmp(ruleIpD,macIpD) == 0){
						sprintf(ruleNameD,"%s",returnDstPri->name);
							sprintf(ruleNameS,"%s",returnDstPri->name);
						strcpy(rl->dstList[j].ipAddr, macIpD) ; 
					//	puts(rl->dstList[j].ipAddr );
						
						strcpy(rl->dstList[j].hostName, ruleNameD) ;
						sprintf(temp_mask,"%d",returnDstPri->nmask);
						mask_str = mask_turn(temp_mask);
						strcpy(rl->dstList[j].mask, temp_mask) ;
						
						
						
						char mac_str[64] = {};
						char mac_new[64] = {}; 
						int n ; 
						n = hex2asc(returnDstPri->mac, 6, mac_str, sizeof(mac_str));
						mac_str[n] = 0;
						turn_mac(mac_str, mac_new);//转换成mac地址字符串
						strcpy(rl->dstList[j].mac, mac_new) ;
						
						char vip_str[64] = {};
						char vip_new[64] = {};
						if(returnDstPri->vip != -1)
						{
							ipaddr2string(htonl(returnDstPri->vip), vip_str, sizeof(vip_str));	//转换成静态ip地址字符串
							sprintf(vip_new, "%s", vip_str);
						}
						else
							memset(vip_new , 0 , strlen(vip_new));
							
						strcpy(rl->dstList[j].natIp, vip_new) ;
						
						strcpy(rl->dstList[j].remark, returnDstPri->comment) ;
						
						char gate[10] = {}; 
						if(returnDstPri->type==NODE_TYPE_GATEWAY)
							strcpy(gate, "G");
						else
							strcpy(gate, "H");
						strcpy(rl->srcList[i].gateway, gate) ;
						strcpy(rl->srcList[i].macList, returnDstPri->othermac) ;
						
						break;
					}
					returnDstPri++;
					rl->dstListLength ++ ;

				}
			}
		
		
		returnPri++;
	}

	disp_rule(rl) ; 
	
	
	if(Rule_to_XML(rl, xmlStr) <0 )
		{
			perror("parse error!");
		}
	puts(xmlStr) ;
	free(rl) ;
	return 0;
}
