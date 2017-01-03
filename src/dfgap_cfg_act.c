#include"dfgap_cfg_act.h" 


hostnode_t vs_cliaddrlist[MAX_CHANNEL_NUM][MAX_ADDRLIST_NUM]=
{
		{
		},
		{
		}
};
int vs_cliaddrlist_num[MAX_CHANNEL_NUM] = {0, 0};

hostnode_t vs_srvaddrlist[MAX_CHANNEL_NUM][MAX_ADDRLIST_NUM]=
{
		{
		},
		{
		}
};

int vs_srvaddrlist_num[MAX_CHANNEL_NUM] = {0, 0};

rule_t rule_table[MAX_CHANNEL_NUM][MAX_RULELIST_NUM];
int rule_table_num[MAX_CHANNEL_NUM] = {0, 0};

snat_t snat_table[MAX_CHANNEL_NUM][MAX_SNATLIST_NUM];
int snat_table_num[MAX_CHANNEL_NUM] = {0, 0};

#define LOGFILE_PATH123 	"/var/www/bbb.txt"

void print_value2(char * name , short value )
{
	char tmp[1256];
	sprintf(tmp, "echo \"%s = %d\" >> %s", name, value, LOGFILE_PATH123);
	system(tmp);
	return ;
}

inline int find_ip_in_cliaddrlist(u_long ip, int channel)
{
	int i = 0;

	for (i = 0; i <vs_cliaddrlist_num[channel]; i++) {
		if (ip == vs_cliaddrlist[channel][i].ip)
			return i;
	}
	
	return -1;
}

// return -1: not found, 0~n-1: index of addrlist
inline int find_ip_in_srvaddrlist(u_long ip, int channel)
{
	int i = 0;

	for (i = 0; i <vs_srvaddrlist_num[channel]; i++) {
		if (ip == vs_srvaddrlist[channel][i].ip)
			return i;
	}
	
	return -1;
}

int print_hostnode(hostnode_t *node)
{
	char mac_str[64];
	char ip_str[64];
	char vip_str[64];
	int n = 0;

	ipaddr2string(htonl(node->ip), ip_str, sizeof(ip_str));
	ipaddr2string(htonl(node->vip), vip_str, sizeof(vip_str));

	n = hex2asc(node->mac, 6, mac_str, sizeof(mac_str));
	mac_str[n] = 0;
	
	//add othermac by gpdn
	disp("%s: ip:%s mac:%s vip:%s type:%d name:%s comment:%s othermac:%s", \
		node->type==NODE_TYPE_GATEWAY?"GATE":"HOST", \
		ip_str, mac_str, vip_str, node->type, node->name, node->comment, node->othermac);
	
	return 0;
}

int print_addrlist_table(hostnode_t addrlist[][MAX_ADDRLIST_NUM], int addrlist_num[])
{
	int channel = 0;

	for (channel = 0; channel < if_channel_table_num; channel++) 
	{
		int i;
		
		disp("channel %d addrlist---------------", channel);
		for (i = 0; i < addrlist_num[channel]; i++) 
		{
			print_hostnode(&(addrlist[channel][i]));
		}
	}
	return 0;
}

int addrlist_table_init()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) 
	{
		memset(&vs_cliaddrlist[channel][0], 0, sizeof(hostnode_t)*MAX_ADDRLIST_NUM);
		vs_cliaddrlist_num[channel] = 0;
		
		memset(&vs_srvaddrlist[channel][0], 0, sizeof(hostnode_t)*MAX_ADDRLIST_NUM);
		vs_srvaddrlist_num[channel] = 0;		
	}

	return 0;
}

int addrlist_table_clean()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) 
	{
		vs_cliaddrlist_num[channel] = 0;
		vs_srvaddrlist_num[channel] = 0;		
	}

	return 0;
}

int addrlist_table_add(int side, int channel, char *buf){
	char *pc = NULL;
	char item_str[128] = "", othermac[768] = "";
	char *p = NULL, tmpstr[128] = "";
	int count = 0;
	hostnode_t hnode;
	u_long tmp_ip_nb = 0;
	if (channel < 0 || channel >= if_channel_table_num || !buf)
		goto err;
	bzero(&hnode, sizeof(hnode));
	pc = strnchcpy(buf, item_str, sizeof(item_str), '|');
	/* mac */
	if (!strcmp(item_str, "*")) 
		memset(hnode.mac, 0, 6);
	else 
		parse_mac(item_str, hnode.mac);
	/* ip */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)){
		warn("addrlist parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, buf);
		goto err;
	}
	hnode.ip = ntohl(tmp_ip_nb);
	/* ip nmask */
	p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
	if (tmpstr[0] == 0)
		count = 32;
	else
		count = atoi(tmpstr);
	hnode.nmask = (count < 0 || count > 32) ? 0 : count;
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (!item_str[0]){
		hnode.vip = -1;		// default the same
		hnode.vnmask = 0;
	}else{
		/* vip */
		p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
		if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)){
			warn("addrlist parse: invalid vip addr(%s), in buf(%s)\n", tmpstr, buf);
			goto err;
		}
		hnode.vip = ntohl(tmp_ip_nb);
		/* vip vnmask */
		p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
		if (tmpstr[0] == 0)
			count = 32;
		else
			count = atoi(tmpstr);
		hnode.vnmask = (count < 0 || count > 32) ? 0 : count;
	}
	/*  host or gw */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (strncasecmp(item_str, "g", 1) == 0)
		hnode.type = NODE_TYPE_GATEWAY;
	else if (strncasecmp(item_str, "h", 1) == 0)
		hnode.type = NODE_TYPE_HOST;
	else {
		warn("addrlist parse: invalid host type(%s), in buf(%s)\n", item_str, buf);
		goto err;
	}

	/* name */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.name, item_str, HOSTNAME_LEN);
	hnode.name[HOSTNAME_LEN-1] = 0;
	
	/* comment */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.comment, item_str, HOSTCOMMENT_LEN);
	hnode.name[HOSTCOMMENT_LEN-1] = 0;
	
	/* othermac by gpdn */
	pc = strnchcpy(pc, othermac, sizeof(othermac), '|');
	strncpy(hnode.othermac, othermac, HOSTOTHERMAC_LEN);
	hnode.name[HOSTOTHERMAC_LEN-1] = 0;
	
	//print_hostnode(&hnode);
	if (side == SIDE_INSIDE){
		memcpy(&vs_cliaddrlist[channel][vs_cliaddrlist_num[channel]], &hnode, sizeof(hostnode_t));
		vs_cliaddrlist_num[channel]++;
	} else{
		memcpy(&vs_srvaddrlist[channel][vs_srvaddrlist_num[channel]], &hnode, sizeof(hostnode_t));
		vs_srvaddrlist_num[channel]++;	
	}
	return 0;
err:
	return -1;
}

int print_rule(rule_t *rulep)
{
	char sip_str[64];
	char dip_str[64];
	char vsip_str[64];
	char vdip_str[64];

	ipaddr2string(htonl(rulep->sip),sip_str, sizeof(sip_str));
	ipaddr2string(htonl(rulep->dip), dip_str, sizeof(dip_str));
	ipaddr2string(htonl(rulep->vsip),vsip_str, sizeof(vsip_str));
	ipaddr2string(htonl(rulep->vdip), vdip_str, sizeof(vdip_str));


	disp("RULE:sip:%s/%d, vsip:%s, dip:%s/%d, vdip:%s, dport:%d, proto:%d, log:%d, action:%d, appcheck:%d, return1bit:%d", \
		sip_str, rulep->snmask, vsip_str, dip_str, rulep->dnmask, vdip_str, rulep->dport, \
		rulep->proto, rulep->log, rulep->action, rulep->appcheck, rulep->return1bit);
	return 0;
}

int print_rule_table(rule_t table[][MAX_RULELIST_NUM], int table_num[])
{
	int channel = 0;

	for (channel = 0; channel < if_channel_table_num; channel++) 
	{
		int i;
		
		disp("channel %d ruletable---------------", channel);
		for (i = 0; i < table_num[channel]; i++) 
		{
			print_rule(&table[channel][i]);
		}
	}
	
	return 0;
}

int rule_table_init()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) {
		memset(&rule_table[channel][0], 0, sizeof(rule_t)*MAX_RULELIST_NUM);
		rule_table_num[channel] = 0;
	}

	return 0;
}

int rule_table_clean()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) {
		rule_table_num[channel] = 0;
	}

	return 0;
}

int rule_table_add(int channel, char *str)
{
	rule_t item;
	char item_str[128] = "";
	const int itemlen = sizeof(item_str);
	char tmpstr[128] = "";
	char *pstr = str;
	char *p = NULL;
	int count = 0;
	u_long tmp_ip_nb;
	
	if (channel < 0 || channel >= if_channel_table_num || !str)
		goto err;

	bzero(&item, sizeof(rule_t));

	/* get the src-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if ((strncmp(tmpstr, "*", 1) == 0) ||
	    (strncasecmp(tmpstr, "any", 3) == 0) ||
	    (strncasecmp(tmpstr, "All", 3) == 0)){
		strcpy(tmpstr, "0.0.0.0");
		item.sip = 0 ;
		item.snmask = 32;
	}else{
		if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)){
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, str);
			goto err;
		}
		item.sip = ntohl(tmp_ip_nb);
		/* get the bits of netmask of source ip */
		p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
		count = atoi(tmpstr);
		item.snmask = (count < 0 || count > 32) ? 0 : count;
	}

	/* get the dst-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if ((strncmp(tmpstr, "*", 1) == 0) ||
	    (strncasecmp(tmpstr, "any", 3) == 0) ||
	    (strncasecmp(tmpstr, "All", 3) == 0)){
		strcpy(tmpstr, "0.0.0.0");
		item.dip = 0 ;
		item.dnmask = 32;
	}else{
		if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) {
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, str);
			goto err;
		}
		item.dip = ntohl(tmp_ip_nb);
		/* get the bits of netmask of dest ip */
		p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
		count = atoi(tmpstr);
		item.dnmask = (count < 0 || count > 32) ? 0 : count;
	}


	/* get the protocol */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "ip", 2) == 0) ||
	    (strncasecmp(item_str, "all", 3) == 0))
	{
		item.proto = PROTO_ID_IP;
	} else if (strncasecmp(item_str, "tcp", 3) == 0) {
		item.proto = PROTO_ID_TCP;
	} else if (strncasecmp(item_str, "udp", 3) == 0) {
		item.proto = PROTO_ID_UDP;
	} else {
		error("rule parse: unknown proto name(%s)\n", item_str);
		goto err;
	}
	
	/* get the dest port */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	item.dport = atoi(item_str);
	int too = 5;
//	print_value2("port-----",too);
	print_value2("port",item.dport);

	/* get the log  */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.log = 1;
	else
		item.log = 0;

	/* get the action */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncasecmp(item_str, "y", 1) == 0) ||
	    (strncasecmp(item_str, "accept", 6) == 0))
	{
		item.action = ACTION_PASS;
	} else if ((strncasecmp(item_str, "n", 1) == 0) ||
	           (strncasecmp(item_str, "drop", 4) == 0) ||
	           (strncasecmp(item_str, "deny", 4) == 0))
	{
		item.action = ACTION_DROP;
	} else {
		error("rule parse: unknown action name(%s)\n", item_str);
		goto err;
	}
	
	/* get the appcheck */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.appcheck = 1;
	else
		item.appcheck = 0;

	/* get the return1bit */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.return1bit = 1;
	else
		item.return1bit = 0;
		

	
//	print_rule(&item);
	memcpy(&rule_table[channel][rule_table_num[channel]], &item, sizeof(item));
	rule_table_num[channel]++;
	return 0;
err:
	return -1;
}

int print_snat(snat_t *snatp)
{
	char sip_str[64];
	char dip_str[64];
	
	if (!snatp)
		return -1;

	ipaddr2string(htonl(snatp->sip), sip_str, sizeof(sip_str));
	ipaddr2string(htonl(snatp->dip), dip_str, sizeof(dip_str));


	disp("SNAT:orignal ip:%s/%d, mapped ip:%s/%d", \
		sip_str, snatp->snmask, dip_str, snatp->dnmask);
	
	return 0;

}
int print_snat_table(snat_t table[][MAX_RULELIST_NUM], int table_num[])
{
	int channel = 0;

	for (channel = 0; channel < if_channel_table_num; channel++) {
		int i;
		
		disp("channel %d snat table---------------", channel);
		for (i = 0; i < table_num[channel]; i++) {
			print_snat(&table[channel][i]);
		}
	}
	return 0;
}

int snat_table_init()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) {
		memset(&snat_table[channel][0], 0, sizeof(snat_t)*MAX_SNATLIST_NUM);
		snat_table_num[channel] = 0;
	}

	return 0;


}

int snat_table_clean()
{
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) {
		snat_table_num[channel] = 0;
	}

	return 0;
}

int snat_table_add(int channel, char *str)
{
	snat_t item;
	char item_str[128] = "";
	const int itemlen = sizeof(item_str);
	char tmpstr[128] = "";
	char *pstr = str;
	char *p = NULL;
	int count = 0;
	u_long tmp_ip_nb = 0;

	
	if (channel < 0 || channel >= if_channel_table_num || !str)
		goto err;

	bzero(&item, sizeof(item));
	
	/* get the src-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if ((strncmp(tmpstr, "*", 1) == 0) ||
	    (strncasecmp(tmpstr, "any", 3) == 0) ||
	    (strncasecmp(tmpstr, "all", 3) == 0))
	{
		strcpy(tmpstr, "0.0.0.0");
	}


	if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) {
		warn("snat parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, str);
		goto err;
	}
	item.sip = ntohl(tmp_ip_nb);
	
	/* get the bits of netmask of original ip */
	p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
	count = atoi(tmpstr);
	item.snmask = (count < 0 || count > 32) ? 0 : count;

	/* get the dst-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if ((strncmp(tmpstr, "*", 1) == 0) ||
	    (strncasecmp(tmpstr, "any", 3) == 0) ||
	    (strncasecmp(tmpstr, "all", 3) == 0))
	{
		strcpy(tmpstr, "0.0.0.0");
	}


	if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) {
		warn("snat parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, str);
		goto err;
	}
	item.dip = ntohl(tmp_ip_nb);

	/* get the bits of netmask of mapped ip */
	p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
	count = atoi(tmpstr);
	item.dnmask = (count < 0 || count > 32) ? 0 : count;

	memcpy(&snat_table[channel][snat_table_num[channel]], &item, sizeof(item));
	snat_table_num[channel]++;

	return 0;
err:
	return -1;
}

int link_snat_addrlist()
{
	int si = 0;
	int nfind = 0;
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) {
		for (si = 0; si < snat_table_num[channel]; si++) {
			nfind = find_ip_in_cliaddrlist(snat_table[channel][si].sip, channel);

			if (nfind >= 0) {
				vs_cliaddrlist[channel][nfind].vip = snat_table[channel][si].dip;
				continue;
			} 

			nfind = find_ip_in_srvaddrlist(snat_table[channel][si].sip, channel);

			if (nfind >= 0) {
				vs_srvaddrlist[channel][nfind].vip = snat_table[channel][si].dip;
				continue;
			} 			
		}
	}

	return 0;
}

int link_rule_addrlist()
{
	int si = 0;
	int nfind = 0;
	int channel = 0;
	
	for (channel = 0; channel < if_channel_table_num; channel++) 
	{
		for (si = 0; si < rule_table_num[channel]; si++) 
		{
			nfind = find_ip_in_cliaddrlist(rule_table[channel][si].sip, channel);

			if (nfind >= 0) 
			{
				rule_table[channel][si].vsip = vs_cliaddrlist[channel][nfind].vip;
			} 

			nfind = find_ip_in_srvaddrlist(rule_table[channel][si].dip, channel);

			if (nfind >= 0) 
			{
				rule_table[channel][si].vdip = vs_srvaddrlist[channel][nfind].vip;
			} 			
		}
	}

	return 0;
}

int get_dfgap_cfg(char *cfgpath){
	FILE *fp = NULL;
	char linebuf[1024] = "";
	if ((fp = fopen(cfgpath, "r")) == NULL){
		error("open %s error", cfgpath);
		goto err;
	}
	while (fgets(linebuf, sizeof(linebuf), fp) != NULL){
		int i;
		char *pc = linebuf;
		/* COMMENT_LINE */
		for (i = 0; i < sizeof(linebuf); i++){
			if (linebuf[i] == '#'){
				linebuf[i] = '\0'; // earse all comment 
				break;
			}
		}
		while (1){
			/* DOS_FORMAT */
			if (*pc=='\n' || *pc=='\r' || *pc==0) // end of line or string
				break;
			if (*pc!='\t' && *pc!=' ') // skip blanks & tabs
				break;
			pc++;
		}	
		
		switch (*pc){
		case '\n':
		case '\r':
		case 0:
			// empty line
			linebuf[0] = 0;
			break;
		default: {
			char block[1024];
			int channel = 0;
			char type = 0;
			char val[1024];
			pc = strnchcpy(pc, block, sizeof(block), ';');
			if (!pc){
				warn("rule parse: ignore invalid line:[%s]", linebuf);
				continue;				
			}
			channel = atoi(block);
			if (channel < 0 || channel >= if_channel_table_num){
				warn("rule parse: ignore invalid channel no[%d], line:[%s]", channel, linebuf);
				continue;
			}
			pc = strnchcpy(pc, block, sizeof(block), ';');
			if (!pc){
				warn("rule parse: ignore invalid line:[%s]", linebuf);
				continue;				
			}
			pc = strnchcpy(pc, val, sizeof(val), ';');
			if (!val[0]){
				warn("rule parse: ignore invalid line:[%s]", linebuf);
				continue;				
			}
			if (!strcasecmp(block, "SRC_MACT")){
				if (addrlist_table_add(SIDE_INSIDE, channel, val) < 0){
					warn("rule parse: ignore invalid line:[%s]", linebuf);
					continue;
				}
			}else if (!strcasecmp(block, "DST_MACT")){
				if (addrlist_table_add(SIDE_OUTSIDE, channel, val) < 0){
					warn("rule parse: ignore invalid line:[%s]", linebuf);
					continue;
				}
			}else if (!strcasecmp(block, "RULE")){
				if (rule_table_add(channel, val) < 0){
					warn("rule parse: ignore invalid line:[%s]", linebuf);
					continue;
				}
			}else if (!strcasecmp(block, "SNAT")){
				if (snat_table_add(channel, val) < 0){
					warn("rule parse: ignore invalid line:[%s]", linebuf);
					continue;
				}
			} else{
				warn("rule parse: ignore invalid type[%s], line:[%s]", block, linebuf);
				continue;
			}
			break;
		}
		}
	}
	fclose(fp);
	//link_snat_addrlist();  qjn
	//link_rule_addrlist();  qjn
	return 0;
err:
	perror("get dfgap_cfg");
	if (fp)
		fclose(fp);
	
	rule_table_clean();
	addrlist_table_clean();
	snat_table_clean();
	
	return -1;	
}

int init_dfgap_cfg()
{
	addrlist_table_init();
	rule_table_init();
	snat_table_init();
	return 0;
}
int clean_dfgap_cfg()
{
	rule_table_clean();
	addrlist_table_clean();
	snat_table_clean();
	
	return 0;	
}

int add_addrlist_table(int channel, int type, char *buf)
{
	char *pc = NULL;
	char item_str[128] = "", othermac[768] = "";
	char *p = NULL, tmpstr[128] = "";
	int ret = 0;
	int tem_i = 0;
	int count = 0;
	hostnode_t hnode;
	u_long tmp_ip_nb = 0;
	if (channel < 0 || channel >= 2 || !buf)
		goto err;
	if ((type != ADRLIST_SRC) && (type != ADRLIST_DST))
		goto err;
	bzero(&hnode, sizeof(hnode));
	pc = strnchcpy(buf, item_str, sizeof(item_str), '|');
	/* mac */
	if (!strcmp(item_str, "*")) {
		memset(hnode.mac, 0, 6);
	}else{
		parse_mac(item_str, hnode.mac);
	}
	 /* check mac */
  
  if (type == ADRLIST_SRC)
  {
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (!memcmp(hnode.mac, vs_cliaddrlist[channel][tem_i].mac, 6))
  		{
  			ret = 2;
  			goto other;
  		}			
  	}
  }
  else
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (!memcmp(hnode.mac, vs_srvaddrlist[channel][tem_i].mac, 6))
  		{
  			ret = 2;
  			goto other;
  		}	
  	}
  }

	/* ip */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) 
	{
		warn("addrlist parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, buf);
		goto err;
	}
	hnode.ip = ntohl(tmp_ip_nb);

	  /* check ip */
  if (type == ADRLIST_SRC){
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (hnode.ip == vs_cliaddrlist[channel][tem_i].ip)
  		{
  			ret = 1;
  			goto other;
  		} 			
  	}
  }
  else 
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (hnode.ip == vs_srvaddrlist[channel][tem_i].ip)
  		{
  			ret = 1;
  			goto other;
  		}			
  	}
  }



	/* ip nmask */
	p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
	if (tmpstr[0] == 0)
		count = 32;
	else
		count = atoi(tmpstr);
	hnode.nmask = (count < 0 || count > 32) ? 0 : count;

	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (!item_str[0]) 
	{
		// default value
	//	hnode.vip = hnode.ip;		// default the same
	//	hnode.vnmask = hnode.nmask;
		hnode.vip = -1;		// default the same
		hnode.vnmask = 0;
	} 
	else 
	{
		/* vip */
		p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
		if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) 
		{
			warn("addrlist parse: invalid vip addr(%s), in buf(%s)\n", tmpstr, buf);
			goto err;
		}

		hnode.vip = ntohl(tmp_ip_nb);

		/* vip vnmask */
		p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
		if (tmpstr[0] == 0)
			count = 32;
		else
			count = atoi(tmpstr);
		hnode.vnmask = (count < 0 || count > 32) ? 0 : count;
	}

	/*  host or gw */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (strncasecmp(item_str, "g", 1) == 0)
		hnode.type = NODE_TYPE_GATEWAY;
	else if (strncasecmp(item_str, "h", 1) == 0)
		hnode.type = NODE_TYPE_HOST;
	else {
		warn("addrlist parse: invalid host type(%s), in buf(%s)\n", item_str, buf);
		goto err;
	}

	/* name */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.name, item_str, HOSTNAME_LEN);
	hnode.name[HOSTNAME_LEN-1] = 0;

	/* check name */
	if (type == ADRLIST_SRC)
  {
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (!strcmp(hnode.name, vs_cliaddrlist[channel][tem_i].name))
  		{
  			ret = 3;
  			goto other;
  		}
   	}
  }
  else
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (!strcmp(hnode.name, vs_srvaddrlist[channel][tem_i].name))
  		{
  			ret = 3;
  			goto other;
  		}			
  	}
  }
	
	/* comment */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.comment, item_str, HOSTCOMMENT_LEN);
	hnode.name[HOSTCOMMENT_LEN-1] = 0;
	
	/* othermac by gpdn */
	pc = strnchcpy(pc, othermac, sizeof(othermac), '|');
	strncpy(hnode.othermac, othermac, HOSTOTHERMAC_LEN);
	hnode.name[HOSTOTHERMAC_LEN-1] = 0;
	
	//print_hostnode(&hnode);
	if (type == ADRLIST_SRC) 
	{
		memcpy(&vs_cliaddrlist[channel][vs_cliaddrlist_num[channel]], &hnode, sizeof(hostnode_t));
		vs_cliaddrlist_num[channel]++;
	} 
	else 
	{
		memcpy(&vs_srvaddrlist[channel][vs_srvaddrlist_num[channel]], &hnode, sizeof(hostnode_t));
		vs_srvaddrlist_num[channel]++;	
	}
	return 0;
err:
	return -1;
other:
	return ret;
}


int turn_mac(const char* mac_org, char* new_mac)
{
	char *p = new_mac;
	int n = 1;
	
	if(!mac_org) goto err;
	
	while (mac_org[n - 1])
	{
		if (n > 2 && (n%2 == 1))
		{
			*p = ':';
			*(++p) = toupper(mac_org[n - 1]);
		}
		else
			*p = toupper(mac_org[n - 1]);
		n++;
		p++;	
	}
	*p = 0 ;
	
	return 0;	
	
	err:
		perror("error:in turn_mac, mac_org is null\n");
		return -1;
}



int fprint_hostnode( hostnode_t* node, FILE* f_cfg, int type, int channel)
{
	char ip_str[64] = {0};
	char vip_str[64] = {0}; // static_ip
	char vip_new[64] = {0}; // static_ip
	char mac_str[64] = {0};
	char mac_new[64] = {0};
	int n = 0;
	int ret = 0;

	ipaddr2string(htonl(node->ip), ip_str, sizeof(ip_str));
	if(node->vip != -1)
		ipaddr2string(htonl(node->vip), vip_str, sizeof(vip_str));

	n = hex2asc(node->mac, 6, mac_str, sizeof(mac_str));
	mac_str[n] = 0;

//adjust something
	turn_mac(mac_str, mac_new);
	if(vip_str)
		sprintf(vip_new, "%s", vip_str);
	else
		strcpy(vip_new, vip_str);
		

	if (type == ADRLIST_SRC)  // 1:SRC_MACT
	{
		ret = fprintf(f_cfg, "%d;SRC_MACT;%s|%s/%d|%s|%s|%s|%s|%s\n", channel, 
			mac_new, ip_str, node->nmask, vip_new, node->type==NODE_TYPE_GATEWAY?"G":"H",
			node->name, node->comment, node->othermac);
		if (ret < 0) goto err;
	}
	else if(type == ADRLIST_DST) // 2: DST_MACT
	{
		ret = fprintf(f_cfg, "%d;DST_MACT;%s|%s/%d|%s|%s|%s|%s|%s\n", channel, 
			mac_new, ip_str, node->nmask, vip_new, node->type==NODE_TYPE_GATEWAY?"G":"H",
			node->name, node->comment, node->othermac);
		if (ret < 0) goto err;
	}
	
	return 0; 

	err:
	  return -1;
}

int fprint_dfgcfg_adrlist(hostnode_t addrlist[][MAX_ADDRLIST_NUM], int addrlist_num[], FILE* f_cfg, int type, int channel)
{
	int i = 0;
	int ret = 0;
	
	for(i = 0; i < addrlist_num[channel]; i++)
	{
		ret = fprint_hostnode( &addrlist[channel][i], f_cfg, type, channel);
		if(ret < 0) goto err;
	}	


	return 0;
	
	err:
	  return -1;
}

int fprint_rule(rule_t *rulep, FILE* f_cfg, int channel)
{
	char sip_str[64] = {0};
	char dip_str[64] = {0};
	char vsip_str[64] = {0};
	char vdip_str[64] = {0};
	char proto_str[64] = {0};
	int ret = 0;

	if(rulep->proto == 1) strcpy(proto_str, "TCP");
	else if(rulep->proto == 2) strcpy(proto_str, "UDP"); 
	else if(rulep->proto == 5) strcpy(proto_str, "ALL");
	else proto_str[0] = 0;

  if (rulep->sip)
		ipaddr2string(htonl(rulep->sip),sip_str, sizeof(sip_str));
	else
	{
		strcpy(sip_str, "All");
		rulep->snmask = 32;
	}
		
	if (rulep->dip)
		ipaddr2string(htonl(rulep->dip), dip_str, sizeof(dip_str));
	else
	{
		strcpy(dip_str, "All");
		rulep->dnmask = 32;
	}
	//ipaddr2string(htonl(rulep->vsip),vsip_str, sizeof(vsip_str));
	//ipaddr2string(htonl(rulep->vdip), vdip_str, sizeof(vdip_str));
	
	ret = fprintf(f_cfg, "%d;RULE;%s/%d|%s/%d|%s|%d|N|ACCEPT|%s|N\n",
			channel, sip_str, rulep->snmask, dip_str, rulep->dnmask, 
			proto_str, rulep->dport, rulep->appcheck == 1?"Y":"N");
	if(ret < 0) goto err;
	
	return 0;
	err:
		perror("error in fprint_rule!\n");
		return -1;
}

int fprint_dfgcfg_rule(rule_t table[][MAX_RULELIST_NUM], int table_num[], FILE* f_cfg,  int channel)
{
	int i = 0;
	int ret = 0;
	for (i = 0; i < table_num[channel]; i++) 
	{
		ret = fprint_rule(&table[channel][i], f_cfg, channel);
		if(ret < 0) goto err;
	}

	
	return 0;

	err:
		return -1;
}

int put_dfgap_cfg( char* dfg_cfg )
{
	FILE* f_dfg_cfg = NULL;
	char line_temp[256] = {0};
	int ret = 0;
	
	
	f_dfg_cfg = fopen( dfg_cfg, "w");
	if(!f_dfg_cfg) goto err;

	fprintf(f_dfg_cfg, "#config file for NetHawk.\n");
	fprintf(f_dfg_cfg, "#SRC_MACT table for channel0.\n");
	ret = fprint_dfgcfg_adrlist(vs_cliaddrlist, vs_cliaddrlist_num, f_dfg_cfg, ADRLIST_SRC, 0);	
	if(ret < 0 ) goto err;

	fprintf(f_dfg_cfg, "#DST_MACT table for channel0.\n");
	ret = fprint_dfgcfg_adrlist(vs_srvaddrlist, vs_srvaddrlist_num, f_dfg_cfg, ADRLIST_DST, 0);	
	if(ret < 0 ) goto err;

	fprintf(f_dfg_cfg, "#RULE table for channel0.\n");
	ret = fprint_dfgcfg_rule(rule_table, rule_table_num, f_dfg_cfg , 0);	
	if(ret < 0 ) goto err;

	fprintf(f_dfg_cfg, "#SRC_MACT table for channel1.\n");
	ret = fprint_dfgcfg_adrlist(vs_cliaddrlist, vs_cliaddrlist_num, f_dfg_cfg, ADRLIST_SRC, 1);	
	if(ret < 0 ) goto err;

	fprintf(f_dfg_cfg, "#DST_MACT table for channel1.\n");
	ret = fprint_dfgcfg_adrlist(vs_srvaddrlist, vs_srvaddrlist_num, f_dfg_cfg, ADRLIST_DST, 1);	
	if(ret < 0 ) goto err;

	fprintf(f_dfg_cfg, "#RULE table for channel1.\n");
	ret = fprint_dfgcfg_rule(rule_table, rule_table_num, f_dfg_cfg , 1);	
	if(ret < 0 ) goto err;

	fclose(f_dfg_cfg);
	sync();
	return 0;

	err:
	  perror("error in dfgcfg write !!\n");
	
	  if(f_dfg_cfg) fclose(f_dfg_cfg);
	  return -1;
}


int add_rule_table(int channel, char *str)
{
	rule_t item;
	char item_str[128] = "";
	const int itemlen = sizeof(item_str);
	char tmpstr[128] = "";
	char *pstr = str;
	char *p = NULL;
	int count = 0;
	int tem_i = 0;
	u_long tmp_ip_nb;
	
	if (channel < 0 || channel >= 2 || !str)
		goto err;

	bzero(&item, sizeof(rule_t));

	/* get the src-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "All", 3) == 0))
	{
		strcpy(item_str, "0.0.0.0");
		
	  if (!inet_aton(item_str, (struct in_addr *)&tmp_ip_nb)) 
	  {
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", item_str, str);
			goto err;
	  }
	  item.sip = ntohl(tmp_ip_nb);
		item.snmask = 32 ;
	}
	else
	{
		for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
		{
			if (!strcmp(item_str, vs_cliaddrlist[channel][tem_i].name))
			{
				item.sip = vs_cliaddrlist[channel][tem_i].ip;
				item.snmask = vs_cliaddrlist[channel][tem_i].nmask;
				break;
			}
		}
	}

	/* get the dst-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "All", 3) == 0))
	{
		strcpy(item_str, "0.0.0.0");
		if (!inet_aton(item_str, (struct in_addr *)&tmp_ip_nb)) 
		{
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", item_str, str);
			goto err;
	  }
	  item.dip = ntohl(tmp_ip_nb);
	  item.dnmask = 32;
	}
	else
	{
		for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
		{
			if (!strcmp(item_str, vs_srvaddrlist[channel][tem_i].name))
			{
				item.dip = vs_srvaddrlist[channel][tem_i].ip;
				item.dnmask = vs_srvaddrlist[channel][tem_i].nmask;
				break;
			}
		}
	}


	


	/* get the protocol */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "ip", 2) == 0) ||
	    (strncasecmp(item_str, "all", 3) == 0))
	{
		item.proto = PROTO_ID_IP;
	} else if (strncasecmp(item_str, "tcp", 3) == 0) {
		item.proto = PROTO_ID_TCP;
	} else if (strncasecmp(item_str, "udp", 3) == 0) {
		item.proto = PROTO_ID_UDP;
	} else {
		error("rule parse: unknown proto name(%s)\n", item_str);
		goto err;
	}
	
	/* get the dest port */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	item.dport = atoi(item_str);

	/* get the log  */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.log = 1;
	else
		item.log = 0;

	/* get the action */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncasecmp(item_str, "y", 1) == 0) ||
	    (strncasecmp(item_str, "accept", 6) == 0))
	{
		item.action = ACTION_PASS;
	} else if ((strncasecmp(item_str, "n", 1) == 0) ||
	           (strncasecmp(item_str, "drop", 4) == 0) ||
	           (strncasecmp(item_str, "deny", 4) == 0))
	{
		item.action = ACTION_DROP;
	} else {
		error("rule parse: unknown action name(%s)\n", item_str);
		goto err;
	}
	
	/* get the appcheck */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.appcheck = 1;
	else
		item.appcheck = 0;

	/* get the return1bit */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.return1bit = 1;
	else
		item.return1bit = 0;
		
			/*check rule*/
	for(tem_i = 0; tem_i < rule_table_num[channel]; tem_i++)
	{
		if(!memcmp(&item, &rule_table[channel][tem_i], sizeof(item)))
		{
			return 1;  // RULE_LAP
		}	
	}
	
//	print_rule(&item);
	memcpy(&rule_table[channel][rule_table_num[channel]], &item, sizeof(item));
	rule_table_num[channel]++;
	
	return 0;
err:
	return -1;
}

int modify_addrlist_table(int channel, int type, char *buf, char* hostname)
{
	char *pc = NULL;
	char item_str[128] = "", othermac[768] = "";
	char *p = NULL, tmpstr[128] = "";
	
	int id = 0;
	int ret = 0;
	int tem_i = 0;
	u_long origin_ip = 0;
	u_char origin_mask = 0;
	
	int count = 0;
	hostnode_t hnode;
	u_long tmp_ip_nb = 0;
	
	if (channel < 0 || channel >= 2 || !buf)
		goto err;
	if ((type != ADRLIST_SRC) && (type != ADRLIST_DST))
		goto err;
	
	id = find_name_in_addrlist(channel, type, hostname);
	if(id < 0) goto err;

	bzero(&hnode, sizeof(hnode));
	
	pc = strnchcpy(buf, item_str, sizeof(item_str), '|');
	/* mac */
	if (!strcmp(item_str, "*")) 
	{
		memset(hnode.mac, 0, 6);
	}
	else
	{ 
		parse_mac(item_str, hnode.mac);
	}
	
	 /* check mac */
  
  if (type == ADRLIST_SRC)
  {
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (!memcmp(hnode.mac, vs_cliaddrlist[channel][tem_i].mac, 6))
  		{
  			ret = 2; 
  			goto other;
  		}			
  	}
  }
  else
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (!memcmp(hnode.mac, vs_srvaddrlist[channel][tem_i].mac, 6))
  		{
  			ret = 2;
  			goto other;
  		}	
  	}
  }

	/* ip */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
	if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) 
	{
		warn("addrlist parse: invalid ip addr(%s), in buf(%s)\n", tmpstr, buf);
		goto err;
	}
	hnode.ip = ntohl(tmp_ip_nb);

	  /* check ip */
  if (type == ADRLIST_SRC)
  {
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (hnode.ip == vs_cliaddrlist[channel][tem_i].ip)
  		{
  			ret = 1;
  			goto other;
  		} 			
  	}
  }
  else 
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (hnode.ip == vs_srvaddrlist[channel][tem_i].ip)
  		{
  			ret = 1;
  			goto other;
  		}			
  	}
  }



	/* ip nmask */
	p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
	if (tmpstr[0] == 0)
		count = 32;
	else
		count = atoi(tmpstr);
	hnode.nmask = (count < 0 || count > 32) ? 0 : count;

	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (!item_str[0]) 
	{
		// default value
	//	hnode.vip = hnode.ip;		// default the same
	//	hnode.vnmask = hnode.nmask;
		hnode.vip = -1;		// default the same
		hnode.vnmask = 0;
	} 
	else 
	{
		/* vip */
		p = strnchcpy(item_str, tmpstr, sizeof(tmpstr), '/');
		if (!inet_aton(tmpstr, (struct in_addr *)&tmp_ip_nb)) 
		{
			warn("addrlist parse: invalid vip addr(%s), in buf(%s)\n", tmpstr, buf);
			goto err;
		}

		hnode.vip = ntohl(tmp_ip_nb);

		/* vip vnmask */
		p = strnchcpy(p, tmpstr, sizeof(tmpstr), '/');
		if (tmpstr[0] == 0)
			count = 32;
		else
			count = atoi(tmpstr);
		hnode.vnmask = (count < 0 || count > 32) ? 0 : count;
	}

	/*  host or gw */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	if (strncasecmp(item_str, "g", 1) == 0)
		hnode.type = NODE_TYPE_GATEWAY;
	else if (strncasecmp(item_str, "h", 1) == 0)
		hnode.type = NODE_TYPE_HOST;
	else {
		warn("addrlist parse: invalid host type(%s), in buf(%s)\n", item_str, buf);
		goto err;
	}

	/* name */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.name, item_str, HOSTNAME_LEN);
	hnode.name[HOSTNAME_LEN-1] = 0;

	/* check name */
	if (type == ADRLIST_SRC)
  {
  	for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (!strcmp(hnode.name, vs_cliaddrlist[channel][tem_i].name))
  		{
  			ret = 3;
  			goto other;
  		}
   	}
  }
  else
  {
  	for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
  	{
  		if (id == tem_i) continue;
  		if (!strcmp(hnode.name, vs_srvaddrlist[channel][tem_i].name))
  		{
  			ret = 3;
  			goto other;
  		}			
  	}
  }
	
	/* comment */
	pc = strnchcpy(pc, item_str, sizeof(item_str), '|');
	strncpy(hnode.comment, item_str, HOSTCOMMENT_LEN);
	hnode.name[HOSTCOMMENT_LEN-1] = 0;
	
	/* othermac by gpdn */
	pc = strnchcpy(pc, othermac, sizeof(othermac), '|');
	strncpy(hnode.othermac, othermac, HOSTOTHERMAC_LEN);
	hnode.name[HOSTOTHERMAC_LEN-1] = 0;
	
	/*change relate rule_table*/
	if (type == ADRLIST_SRC)
	{
		origin_ip = vs_cliaddrlist[channel][id].ip;
		origin_mask = vs_cliaddrlist[channel][id].nmask;
		for (tem_i = 0; tem_i < rule_table_num[channel]; tem_i++)
		{
			if(origin_ip == rule_table[channel][tem_i].sip && origin_mask == rule_table[channel][tem_i].snmask)
			{
				rule_table[channel][tem_i].sip = hnode.ip;
				rule_table[channel][tem_i].snmask = hnode.nmask;
			}
		}
	} 
	else
	{
		origin_ip = vs_srvaddrlist[channel][id].ip;
		origin_mask = vs_srvaddrlist[channel][id].nmask;
		for (tem_i = 0; tem_i < rule_table_num[channel]; tem_i++)
		{
			if(origin_ip == rule_table[channel][tem_i].dip && origin_mask == rule_table[channel][tem_i].dnmask)
			{
				rule_table[channel][tem_i].dip = hnode.ip;
				rule_table[channel][tem_i].dnmask = hnode.nmask;
			}
		}
	}
	
  

	if (type == ADRLIST_SRC) 
	{
		memcpy(&vs_cliaddrlist[channel][id], &hnode, sizeof(hostnode_t));
	} 
	else 
	{
		memcpy(&vs_srvaddrlist[channel][id], &hnode, sizeof(hostnode_t));
	}
	
	return 0;
err:
	return -1;
other:
	return ret;
}

int modify_rule_table(int channel, char *str, int ruleid)
{
	rule_t item;
	char item_str[128] = "";
	const int itemlen = sizeof(item_str);
	char tmpstr[128] = "";
	char *pstr = str;
	char *p = NULL;
	int count = 0;
	int tem_i = 0;
	u_long tmp_ip_nb;
	
	if (channel < 0 || channel >= 2 || !str)
		goto err;
	if (ruleid <= 0 || ruleid > rule_table_num[channel]) 
		goto err;

	bzero(&item, sizeof(rule_t));

	/* get the src-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "All", 3) == 0))
	{
		strcpy(item_str, "0.0.0.0");
		
	  if (!inet_aton(item_str, (struct in_addr *)&tmp_ip_nb)) 
	  {
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", item_str, str);
			goto err;
	  }
	  item.sip = ntohl(tmp_ip_nb);
		item.snmask = 32 ;
	}
	else
	{
		for (tem_i = 0; tem_i < vs_cliaddrlist_num[channel]; tem_i++)
		{
			if (!strcmp(item_str, vs_cliaddrlist[channel][tem_i].name))
			{
				item.sip = vs_cliaddrlist[channel][tem_i].ip;
				item.snmask = vs_cliaddrlist[channel][tem_i].nmask;
				break;
			}
		}
	}

	/* get the dst-ip address */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "All", 3) == 0))
	{
		strcpy(item_str, "0.0.0.0");
		if (!inet_aton(item_str, (struct in_addr *)&tmp_ip_nb)) 
		{
			warn("rule parse: invalid ip addr(%s), in buf(%s)\n", item_str, str);
			goto err;
	  }
	  item.dip = ntohl(tmp_ip_nb);
	  item.dnmask = 32;
	}
	else
	{
		for (tem_i = 0; tem_i < vs_srvaddrlist_num[channel]; tem_i++)
		{
			if (!strcmp(item_str, vs_srvaddrlist[channel][tem_i].name))
			{
				item.dip = vs_srvaddrlist[channel][tem_i].ip;
				item.dnmask = vs_srvaddrlist[channel][tem_i].nmask;
				break;
			}
		}
	}

	/* get the protocol */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncmp(item_str, "*", 1) == 0) ||
	    (strncasecmp(item_str, "any", 3) == 0) ||
	    (strncasecmp(item_str, "ip", 2) == 0) ||
	    (strncasecmp(item_str, "all", 3) == 0))
	{
		item.proto = PROTO_ID_IP;
	} else if (strncasecmp(item_str, "tcp", 3) == 0) {
		item.proto = PROTO_ID_TCP;
	} else if (strncasecmp(item_str, "udp", 3) == 0) {
		item.proto = PROTO_ID_UDP;
	} else {
		error("rule parse: unknown proto name(%s)\n", item_str);
		goto err;
	}
	
	/* get the dest port */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	item.dport = atoi(item_str);

	/* get the log  */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.log = 1;
	else
		item.log = 0;

	/* get the action */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if ((strncasecmp(item_str, "y", 1) == 0) ||
	    (strncasecmp(item_str, "accept", 6) == 0))
	{
		item.action = ACTION_PASS;
	} else if ((strncasecmp(item_str, "n", 1) == 0) ||
	           (strncasecmp(item_str, "drop", 4) == 0) ||
	           (strncasecmp(item_str, "deny", 4) == 0))
	{
		item.action = ACTION_DROP;
	} else {
		error("rule parse: unknown action name(%s)\n", item_str);
		goto err;
	}
	
	/* get the appcheck */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.appcheck = 1;
	else
		item.appcheck = 0;

	/* get the return1bit */
	pstr = strnchcpy(pstr, item_str, itemlen, '|');
	if (strncasecmp(item_str, "y", 1) == 0)
		item.return1bit = 1;
	else
		item.return1bit = 0;
		
		/*check rule*/
	for(tem_i = 0; tem_i < rule_table_num[channel]; tem_i++)
	{
		if(tem_i == ruleid -1) continue;
		if(!memcmp(&item, &rule_table[channel][tem_i], sizeof(item)))
		{
			return 1;  // RULE_LAP
		}	
	}
	
//	print_rule(&item);
	memcpy(&rule_table[channel][ruleid -1], &item, sizeof(item));

	return 0;
err:
	return -1;
}

int delete_rule_table (int channel, int ruleid)
{
	int gap = 0;

	if (ruleid <= 0 || ruleid > rule_table_num[channel]) goto err;
	gap = rule_table_num[channel] - ruleid;

	if(gap)
		memcpy (&rule_table[channel][ruleid-1], &rule_table[channel][ruleid], gap * sizeof(rule_t));
	
	memset (&rule_table[channel][rule_table_num[channel] - 1], 0, sizeof(rule_t));
	
	rule_table_num[channel]--;



	return 0;
  err:
  	return -1;
	
}

int delete_addrlist_table (int channel, int type, char* hostname)
{
	int gap = 0;
	int id = 0;
	int res = 0;

  	if ((type != ADRLIST_SRC) && (type != ADRLIST_DST))
		goto err;
		
	id = find_name_in_addrlist(channel, type, hostname);
	id++;
	
//	if( -1 == check_host_in_rule(channel, type, hostname))
		//	goto err;

	if ((type == ADRLIST_SRC) && (id <= 0 || id > vs_cliaddrlist_num[channel])) goto err;
	if ((type == ADRLIST_DST) && (id <= 0 || id > vs_srvaddrlist_num[channel])) goto err;
		
	if (type == ADRLIST_SRC)
		res = check_ip_in_rule(channel , type, vs_cliaddrlist[channel][id - 1].ip);
  else
  	res = check_ip_in_rule(channel , type, vs_srvaddrlist[channel][id - 1].ip);
  	
  if(res == 1) goto others;

	
	if (type == ADRLIST_SRC)
	{
		gap = vs_cliaddrlist_num[channel] - id;
	
		if(gap)
			memcpy (&vs_cliaddrlist[channel][id-1], &vs_cliaddrlist[channel][id], gap * sizeof(hostnode_t));
	
		memset (&vs_cliaddrlist[channel][vs_cliaddrlist_num[channel] - 1], 0, sizeof(hostnode_t));
	
		vs_cliaddrlist_num[channel]--;
  }
  else
  {
  	gap = vs_srvaddrlist_num[channel] - id;
	
		if(gap)
			memcpy (&vs_srvaddrlist[channel][id-1], &vs_srvaddrlist[channel][id], gap * sizeof(hostnode_t));
	
		memset (&vs_srvaddrlist[channel][vs_srvaddrlist_num[channel] - 1], 0, sizeof(hostnode_t));
	
		vs_srvaddrlist_num[channel]--;
  }

	return 0;
	others:
	  return res;
  err:
  	return -1;
	
}

int find_name_in_addrlist(int channel, int type, char* hostname)
{
	int i = 0;
	

	if ((type != ADRLIST_SRC) && (type != ADRLIST_DST))
		goto err;
		
	if (type == ADRLIST_SRC)
	{
	   for( i = 0; i< vs_cliaddrlist_num[channel]; i++)
	   {
	   		if(!strcmp(hostname, vs_cliaddrlist[channel][i].name))
	   			return i;
	   }
  }
  else
  {
  	 for( i = 0; i< vs_srvaddrlist_num[channel]; i++)
	   {
	   		if(!strcmp(hostname, vs_srvaddrlist[channel][i].name))
	   			return i;
	   }
  }
 
  err:
  return -1;
}

int check_host_in_rule(int channel, int type, char* hostname)
{
	int i = 0;
	int res = 0;
	u_long temp_ip = 0 ;

	if ((type != ADRLIST_SRC) && (type != ADRLIST_DST))
		goto err;

	if (type == ADRLIST_SRC)
	{
	   for( i = 0; i< vs_cliaddrlist_num[channel]; i++)
	   {
	   		if(!strcmp(hostname, vs_cliaddrlist[channel][i].name))
	   			temp_ip = vs_cliaddrlist[channel][i].ip;
	   }
  }
  else
  {
  	 for( i = 0; i< vs_srvaddrlist_num[channel]; i++)
	   {
	   		if(!strcmp(hostname, vs_srvaddrlist[channel][i].name))
	   			temp_ip = vs_srvaddrlist[channel][i].ip;
	   }
  }

 	res = check_ip_in_rule(channel , type, temp_ip);

 	if(res == 1)
 		goto err;

 	return 0;

  err:
  return -1;
}

int check_ip_in_rule(int channel, int type, u_long ip)
{
	int i = 0;
  if (type == ADRLIST_SRC)
  {
  	for(i = 0; i < rule_table_num[channel]; i++)
	  {
		  if(rule_table[channel][i].sip == ip)
			  return 1;
	  }
  }
  else
  {
  	for(i = 0; i < rule_table_num[channel]; i++)
	  {
		  if(rule_table[channel][i].dip == ip)
			  return 1;
	  }
  }
	return 0;
}

int getStructLen(char *channelType){
	int returnVal;
	if(strcmp(CHANNEL0_SRC,channelType) == 0){
		returnVal = vs_cliaddrlist_num[0];
	}else if(strcmp(CHANNEL0_DST,channelType) == 0){
		returnVal = vs_srvaddrlist_num[0];
	}else if(strcmp(CHANNEL1_SRC,channelType) == 0){
		returnVal = vs_cliaddrlist_num[1];
	}else if(strcmp(CHANNEL1_DST,channelType) == 0){
		returnVal = vs_srvaddrlist_num[1];
	}else{
		returnVal = 0;
	}
	return returnVal;
}



struct hostnode_struct *getStruct(char *channelType){
	struct hostnode_struct *returnPri;
	if(strcmp(CHANNEL0_SRC,channelType) == 0){
		if(vs_cliaddrlist_num[0] > 0) returnPri = &vs_cliaddrlist[0][0];
	}else if(strcmp(CHANNEL0_DST,channelType) == 0){
		if(vs_srvaddrlist_num[0] > 0) returnPri = &vs_srvaddrlist[0][0];
	}else if(strcmp(CHANNEL1_SRC,channelType) == 0){
		if(vs_cliaddrlist_num[1] > 0) returnPri = &vs_cliaddrlist[1][0];
	}else if(strcmp(CHANNEL1_DST,channelType) == 0){
		if(vs_srvaddrlist_num[1] > 0) returnPri = &vs_srvaddrlist[1][0];
	}
	return returnPri;
}

int getRuleStructLen(char *ruleChannel){
	int returnVal;
	if(strcmp(CHANNEL0_RULE,ruleChannel) == 0){
		returnVal = rule_table_num[0];
	}else if(strcmp(CHANNEL1_RULE,ruleChannel) == 0){
		returnVal = rule_table_num[1];
	}else{
		returnVal = 0;
	}
	return returnVal;
}

struct rule_struct *getRuleStruct(char *ruleChannel){
	struct rule_struct *returnPri;
	if(strcmp(CHANNEL0_RULE,ruleChannel) == 0){
		if(rule_table_num[0] > 0) {
			returnPri = &rule_table[0][0];
		}
	}else if(strcmp(CHANNEL1_RULE,ruleChannel) == 0){
		if(rule_table_num[1] > 0) returnPri = &rule_table[1][0];
	}
	return returnPri;
}

int getStruct_channel0_s_Len()
{
	int returnVal;
	 returnVal = vs_cliaddrlist_num[0];
	 return returnVal;
}

int getStruct_channel1_s_Len()
{
	int returnVal;
	 returnVal = vs_cliaddrlist_num[1];
	 return returnVal;
}

int getStruct_channel0_d_Len()
{
	int returnVal;
	 returnVal = vs_srvaddrlist_num[0];
	 return returnVal;
}

int getStruct_channel1_d_Len()
{
	int returnVal;
	 returnVal = vs_srvaddrlist_num[1];
	 return returnVal;
}

struct hostnode_struct * get_channel0_s_Struct()
{
	struct hostnode_struct *returnPri;
	returnPri = &vs_cliaddrlist[0][0];
	return returnPri;
}

struct hostnode_struct * get_channel1_s_Struct()
{
	struct hostnode_struct *returnPri;
	returnPri = &vs_cliaddrlist[1][0];
	return returnPri;
}

struct hostnode_struct * get_channel0_d_Struct()
{
	struct hostnode_struct *returnPri;
	returnPri = &vs_srvaddrlist[0][0];
	return returnPri;
}


struct hostnode_struct * get_channel1_d_Struct()
{
	struct hostnode_struct *returnPri;
	returnPri = &vs_srvaddrlist[1][0];
	return returnPri;
}
