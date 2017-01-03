#ifndef _DFGAP_CFG_ACT_H
#define _DFGAP_CFG_ACT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "misc.h"

#define MAX_ADDRLIST_NUM	256
#define MAX_RULE_NUM			512
#define MAX_SNAT_NUM			512
#define MAX_RULELIST_NUM	512
#define MAX_SNATLIST_NUM	512

#define NODE_TYPE_HOST		0
#define NODE_TYPE_GATEWAY	1

#define HOSTNAME_LEN			128
#define HOSTCOMMENT_LEN		128
#define HOSTOTHERMAC_LEN	768	//gpdn add for other mac

#define ACTION_PASS		1
#define ACTION_DROP		2
#define ACTION_NONE		0

#define CHANL_0  0 
#define CHANL_1  1 

#define ADRLIST_SRC   1
#define ADRLIST_DST   2

#define SUCCESS 0
#define IP_LAP 1
#define MAC_LAP 2
#define HOST_LAP 3

#define if_channel_table_num  2

#define CHANNEL0_SRC "0SRC_MACT"
#define CHANNEL0_DST "0DST_MACT"
#define CHANNEL1_SRC "1SRC_MACT"
#define CHANNEL1_DST "1DST_MACT"
#define CHANNEL0_RULE "0RULE"
#define CHANNEL1_RULE "1RULE"

typedef struct hostnode_struct 
{
	u_long ip;
	u_char nmask;
	u_char mac[6];
	u_long vip;  // natip 
	u_char vnmask;
	u_char type;
	u_char name[HOSTNAME_LEN];
	u_char comment[HOSTCOMMENT_LEN];
	u_char othermac[HOSTOTHERMAC_LEN];	//gpdn add for other mac
} hostnode_t;

typedef struct rule_struct 
{
	u_long sip;
	u_long dip;
	u_char snmask;
	u_char dnmask;	
	u_long vsip;
	u_long vdip;
	u_char proto;
	u_short dport;
	u_char log;
	u_char action;
	u_char return1bit;
	u_char appcheck;
} rule_t;

typedef struct 
{
	u_long sip;	// addr mapped from
	u_long dip;	// addr mapped to
	u_char snmask;
	u_char dnmask;
} snat_t;

extern hostnode_t vs_cliaddrlist[MAX_CHANNEL_NUM][MAX_ADDRLIST_NUM]; // 源地址表
extern int vs_cliaddrlist_num[MAX_CHANNEL_NUM];                       // 地址表数目
extern hostnode_t vs_srvaddrlist[MAX_CHANNEL_NUM][MAX_ADDRLIST_NUM];  //目的地址表
extern int vs_srvaddrlist_num[MAX_CHANNEL_NUM];                      //目的地址表数目
extern rule_t rule_table[MAX_CHANNEL_NUM][MAX_RULE_NUM];           // 规则表
extern int rule_table_num[MAX_CHANNEL_NUM];                        // 规则表数目



extern snat_t snat_table[MAX_CHANNEL_NUM][MAX_SNAT_NUM];
extern int snat_table_num[MAX_CHANNEL_NUM];

 



int add_rule_table(int channel, char *str);                  // 添加策略列表
int delete_rule_table (int channel, int ruleid);              // 删除 策略列表
int modify_rule_table(int channel, char *str, int ruleid);     // 


int add_addrlist_table(int channel, int type, char *buf);
int delete_addrlist_table (int channel, int type, char* hostname);
int find_name_in_addrlist(int channle, int type, char* hostname);       // 根据hostname 查找 相应 表项，返回 数组号
int modify_addrlist_table(int channel, int type, char *buf, char* hostname);

int put_dfgap_cfg( char* dfg_cfg );  // 写配置文件


int turn_mac(const char* mac_org, char* new_mac); 

int fprint_hostnode( hostnode_t* node, FILE* f_cfg, int type, int channel); 
int fprint_dfgcfg_adrlist(hostnode_t addrlist[][MAX_ADDRLIST_NUM], int addrlist_num[], FILE* f_cfg, int type, int channel);
int fprint_rule(rule_t *rulep, FILE* f_cfg, int channel);
int fprint_dfgcfg_rule(rule_t table[][MAX_RULELIST_NUM], int table_num[], FILE* f_cfg,  int channel);




int print_hostnode(hostnode_t *node);
int print_addrlist_table(hostnode_t addrlist[][MAX_ADDRLIST_NUM], int addrlist_num[]);
int addrlist_table_init();
int addrlist_table_clean();
int addrlist_table_add(int side, int channel, char *buf);
int print_rule(rule_t *rulep);
int print_rule_table(rule_t table[][MAX_RULELIST_NUM], int table_num[]);
int rule_table_init();
int rule_table_clean();
int rule_table_add(int channel, char *str);
int print_snat(snat_t *snatp);
int print_snat_table(snat_t table[][MAX_RULELIST_NUM], int table_num[]);
int snat_table_init();
int snat_table_clean();
int snat_table_add(int channel, char *str);
int link_snat_addrlist();
int link_rule_addrlist();
int get_dfgap_cfg(char *cfgpath);
int clean_dfgap_cfg();
int init_dfgap_cfg();
//	hostnode_struct
int getStructLen(char *channelType);
struct hostnode_struct *getStruct(char *channelType);

/***
 * 获取指定通道规则表长度
 */
int getRuleStructLen(char *ruleChannel);
/***
 *获取指定通道规则表数组头指针
 */
struct rule_struct *getRuleStruct(char *ruleChannel);
int getStruct_channel0_s_Len();
int getStruct_channel1_s_Len();
int getStruct_channel0_d_Len();
int getStruct_channel1_d_Len();
struct hostnode_struct * get_channel0_s_Struct();
struct hostnode_struct * get_channel1_s_Struct();
struct hostnode_struct * get_channel0_d_Struct();
struct hostnode_struct * get_channel1_d_Struct();
#endif
