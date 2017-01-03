#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "envfile.h"
#include <string.h>
#include <unistd.h>

/************************************

Function: analysis

Description: Mark the config file on every speical position

Called By: 
*************************************/
int analysis(long int* src1,long int* dst1,long int* rule1,long int* src2,long int* dst2,long int* rule2)
{	
	FILE* fp;
	char* buf;
	char line[LINEMAX]={0};
	int len=0;

	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open fille failed\n");	
		return 0;
	}

	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		len=strlen(line);
		buf=strtok(line," ");
		if(buf==NULL) continue;

		if(!strcmp(buf,"#SRC_MACT"))
		{
			if(*src1==0) *src1=ftell(fp)-len;
			else *src2=ftell(fp)-len;
		}	
		else if(!strcmp(buf,"#DST_MACT"))
		{	
			if(*dst1==0) *dst1=ftell(fp)-len;
			else *dst2=ftell(fp)-len;
		}	
		else if(!strcmp(buf,"#RULE"))
		{	
			if(*rule1==0) *rule1=ftell(fp)-len;
			else *rule2=ftell(fp)-len;
		}	
	}
	fclose(fp);
	return 1;
}

void AddNetcfg(char* buf,long int offset)
{	
	FILE* fp;
	char line[LINEMAX]={0};
	int num_sum=0;
	int num_buf=0;
	int num_all=0;
	long int temp_offset=0;


	if((fp=fopen(NetCfgFile,"rb"))==NULL) printf("open netconfig failed;\n");
	fseek(fp,0,SEEK_END);
	num_sum=ftell(fp);
	num_buf=strlen(buf)+5;		
	num_all=num_sum+num_buf;
	char* sum;
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);
	fseek(fp,0,SEEK_SET);
	while(temp_offset<offset)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
//		printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	strcat(sum,buf);
	strcat(sum,"\n");
	while((fgets(line,LINEMAX,fp))!=NULL)
	{
		strcat(sum,line);
//		printf("%s\n",sum);
	}	
	fclose(fp);
	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);
	free(sum);
	sync();
						
	
}
int DeleteCfg_Check(char* hostname,long int offset1,long int offset2,long int offset3,long int offset4)
{

	FILE* fp;
	//char* sum;
	int temp_offset=0;
	int num_all=0; 
	char line[LINEMAX]={0};
	char* line_temp;	

	char* IP_MASK;
	char MAC[30]={0};
	char STATIC[25]={0};
	char GATE[2]={0};
	char HOST[10]={0};
	char REMARK[100]={0};
	char MAC_LIST[1024]={0};
	
	char* temp_src;
	char* temp_dest;
	//char* rest;
	int res=1;	

	IP_MASK=malloc(sizeof(char)*50);
	line_temp=malloc(sizeof(char)*LINEMAX);


	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 

	fseek(fp,0,SEEK_END);
	num_all = ftell(fp);
	if(offset4 == 0)
		offset4 = num_all;

	/*********** if rule is null then return  **************/
	fseek(fp,offset3,SEEK_SET);
	fgets(line,LINEMAX,fp);
	temp_offset = ftell(fp);
	if(temp_offset == offset4)
	{
		return 1;
	}	
	
	fseek(fp,0,SEEK_SET);
	temp_offset = 0;
	
	/******* check host exits or not ********/
	fseek(fp,offset1,SEEK_SET);	
	fgets(line,LINEMAX,fp);

	while(temp_offset < offset2)
	{	
		fgets(line,LINEMAX,fp);
		strcpy(line_temp,line);
		_separate(line_temp,IP_MASK,MAC,STATIC,GATE,HOST,REMARK, MAC_LIST);
		if(!strcmp(HOST,hostname)) break;
		temp_offset=ftell(fp);
	//	printf("%s\n",sum);
	}
	fseek(fp,offset3,SEEK_SET);
	fgets(line,LINEMAX,fp);

	if(offset4 == 0)
		offset4 = num_all;

	while(temp_offset<offset4)
	{
		fgets(line,LINEMAX,fp);
		strcpy(line_temp,line);
		
		temp_src=strtok(line_temp+7,"|");
		temp_dest=strtok(NULL,"|");
	
		if(!strcmp(temp_src,IP_MASK))
		{
			res=0;	
			break;
		}else if(!strcmp(temp_dest,IP_MASK))
		{
			res=0;
			break;
		}
		temp_offset=ftell(fp);
	}	


	fclose(fp);

	free(IP_MASK);
	free(line_temp);
	return res;
}

void DeleteCfg(char* hostname,long int offset1,long int offset2)
{
	FILE* fp;
	char* sum;
	int temp_offset=0;
	int num_all=0; 
	char line[LINEMAX]={0};
	char* line_temp;	

	char* IP;
	char* MASK;
	char MAC[30]={0};
	char STATIC[25]={0};
	char GATE[2]={0};
	char HOST[10]={0};
	char REMARK[100]={0};
	char MAC_LIST[1024]={0};
	
	IP=malloc(sizeof(char)*32);
	//MASK=malloc(sizeof(int));
	line_temp=malloc(sizeof(char)*LINEMAX);


	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 
	fseek(fp,0,SEEK_END);
	num_all=ftell(fp);
	fseek(fp,0,SEEK_SET);
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);

	while(temp_offset < offset1)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	fgets(line,LINEMAX,fp);
	strcat(sum,line);

	while(temp_offset < offset2)
	{	
		
		fgets(line,LINEMAX,fp);
		strcpy(line_temp,line);
		separate(line_temp,&IP,&MASK,MAC,STATIC,GATE,HOST,REMARK, MAC_LIST);
		if(!strcmp(HOST,hostname)) break;
		strcat(sum,line);
		temp_offset=ftell(fp);
	//	printf("%s\n",sum);
		free(MASK);
		
	}

	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		strcat(sum,line);
	}
	fclose(fp);


	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);


	free(sum);
	free(IP);
	free(MASK);
	free(line_temp);
	sync();
}

void ReplaceCfg_ruleByModify(char* Hostname_origin,char* IPMASK_new,long int offset1,long int offset2,long int offset3,long int offset4,int flag_channel,int flag_channel_true)
{
	FILE* fp;
	char* sum;
	int temp_offset=0;
	int num_all=0; 
	char line[LINEMAX]={0};	
	char* line_temp;	
	char* Cfgdata;	
	char* str_temp;
	
	char* IP_MASK;
	char MAC[30]={0};
	char STATIC[25]={0};
	char GATE[2]={0};
	char HOST[10]={0};
	char REMARK[100]={0};
	char MAC_LIST[1024]={0};

	char* IP_SRC;
	char* IP_DEST;
	char* rest;
	//int num_test=0;
	//char show[10]={0};
	
	str_temp = malloc(sizeof(char)*50);
	IP_MASK = malloc(sizeof(char)*50);
	line_temp=malloc(sizeof(char)*LINEMAX);
	Cfgdata = malloc(sizeof(char)*LINEMAX);

	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 
	fseek(fp,0,SEEK_END);
	num_all=ftell(fp)+100 ;
	fseek(fp,0,SEEK_SET);
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);

	while(temp_offset < offset1)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	fgets(line,LINEMAX,fp);
	strcat(sum,line);
	
	while(temp_offset < offset2)
	{	
		fgets(line,LINEMAX,fp);
		strcpy(line_temp,line);
		_separate(line_temp,IP_MASK,MAC,STATIC,GATE,HOST,REMARK, MAC_LIST);
		if(!strcmp(HOST,Hostname_origin))
		{	
			strcpy(str_temp,IP_MASK);
		}
		strcat(sum,line);
		temp_offset=ftell(fp);
	}
			/*strcat(sum,"[");
			strcat(sum,str_temp);
			strcat(sum,"]");*/

	fgets(line,LINEMAX,fp);
	strcat(sum,line);
	//temp_offset = ftell(fp);
	if(offset4 == 0)
	{
		offset4 = num_all;
	}
	while(temp_offset < offset3)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	if(flag_channel == 0||flag_channel == 3)
	{
		if(flag_channel == 3)
		{
			temp_offset=ftell(fp);
		}else{

			if(fgets(line,LINEMAX,fp)!=NULL)
			{
				temp_offset = ftell(fp);
				strcat(sum,line);
			}
		}
	}else
	{
		fseek(fp,offset3,SEEK_SET);
		fgets(line,LINEMAX,fp);
	
	}	
	/*fgets(line,LINEMAX,fp);
	temp_offset = ftell(fp);
	if(temp_offset>(offset3+27)&& flag_channel==1)
	{
		fseek(fp,offset3,SEEK_SET);
		fgets(line,LINEMAX,fp);
	}else{
		strcat(sum,line);
	}*/
	
	while(temp_offset < offset4)
	{	
		
		if(fgets(line,LINEMAX,fp)==NULL)
		{
			break;
		}
		strcpy(line_temp,line);
		
		strtok(line_temp,";");
		strtok(NULL,";");
		IP_SRC = strtok(NULL,"|");
		IP_DEST = strtok(NULL,"|");
		rest = strtok(NULL,"\n");
		if(!strcmp(IP_SRC,str_temp)) 
		{
			memset(Cfgdata,0,100);
			if(flag_channel_true == 0)
			{
				strcat(Cfgdata,"0;RULE;");	
			}else if(flag_channel_true == 1)
			{
				strcat(Cfgdata,"1;RULE;");	
			}
			strcat(Cfgdata,IPMASK_new);	
			strcat(Cfgdata,"|");	
			strcat(Cfgdata,IP_DEST);	
			strcat(Cfgdata,"|");	
			strcat(Cfgdata,rest);	

			strcat(sum,Cfgdata);
			strcat(sum,"\n");
		}
		else if(!strcmp(IP_DEST,str_temp))
		{
			memset(Cfgdata,0,100);
			if(flag_channel_true == 0)
			{
				strcat(Cfgdata,"0;RULE;");	
			}else if(flag_channel_true == 1)
			{
				strcat(Cfgdata,"1;RULE;");	
			}
			strcat(Cfgdata,IP_SRC);	
			strcat(Cfgdata,"|");	
			strcat(Cfgdata,IPMASK_new);	
			strcat(Cfgdata,"|");	
			strcat(Cfgdata,rest);	

			strcat(sum,Cfgdata);
			strcat(sum,"\n");
		}else 
		{
			strcat(sum,line);
		}
		temp_offset=ftell(fp);
	//	printf("%s\n",sum);
		
	}
	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		strcat(sum,line);
	}
	fclose(fp);

	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);

	free(sum);
	/*free(IP_SRC);
	free(IP_DEST);
	free(PROTOCOL);
	free(PORT);
 	free(SIGN);*/
	free(str_temp);
	free(IP_MASK);
	free(Cfgdata);
	free(line_temp);
	sync();

}
void ReplaceCfg(char* hostname,char* buf,long int offset1,long int offset2)
{
	FILE* fp;
	char* sum;
	int temp_offset=0;
	int num_all=0; 
	char line[LINEMAX]={0};	
	char* line_temp;	

	char* IP;
	char* MASK;
	char MAC[30]={0};
	char STATIC[25]={0};
	char GATE[2]={0};
	char HOST[10]={0};
	char REMARK[100]={0};
	char MAC_LIST[1024]={0};
	
	IP=malloc(sizeof(char)*32);
	line_temp=malloc(sizeof(char)*2048);

	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 
	fseek(fp,0,SEEK_END);
	num_all=ftell(fp)+strlen(buf)+5;
	fseek(fp,0,SEEK_SET);
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);

	while(temp_offset < offset1)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	fgets(line,LINEMAX,fp);
	strcat(sum,line);

	while(temp_offset < offset2)
	{	
		
		fgets(line,LINEMAX,fp);
		strcpy(line_temp,line);
		separate(line_temp,&IP,&MASK,MAC,STATIC,GATE,HOST,REMARK, MAC_LIST);
		if(!strcmp(HOST,hostname)) 
		{
			strcat(sum,buf);
			strcat(sum,"\n");
			break;
			
		}
		strcat(sum,line);
		temp_offset=ftell(fp);
	//	printf("%s\n",sum);
		free(MASK);
		
	}

	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		strcat(sum,line);
	}
	fclose(fp);

	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);

	free(sum);
	free(IP);
	free(MASK);
	free(line_temp);
	sync();
}

char* mask_turn(char* mask)
{
char* Mask;
Mask=malloc(sizeof(char)*20);
if(!strcmp("0",mask))
{ 
strcpy(Mask,"0.0.0.0");
return Mask;
}
if(!strcmp("1",mask))
{ 
strcpy(Mask,"128.0.0.0");
return Mask;
}
else if(!strcmp("2",mask))
{ 
strcpy(Mask,"192.0.0.0");
return Mask;
}
else if(!strcmp("3",mask))
{ 
strcpy(Mask,"224.0.0.0");
return Mask;
}
else if(!strcmp("4",mask))
{ 
strcpy(Mask,"240.0.0.0");
return Mask;
}
else if(!strcmp("5",mask))
{ 
strcpy(Mask,"248.0.0.0");
return Mask;
}
else if(!strcmp("6",mask))
{ 
strcpy(Mask,"252.0.0.0");
return Mask;
}
else if(!strcmp("7",mask))
{ 
strcpy(Mask,"254.0.0.0");
return Mask;
}
else if(!strcmp("8",mask))
{ 
strcpy(Mask,"255.0.0.0");
return Mask;
}
else if(!strcmp("9",mask))
{ 
strcpy(Mask,"255.128.0.0");
return Mask;
}
else if(!strcmp("10",mask))
{ 
strcpy(Mask,"255.192.0.0");
return Mask;
}
else if(!strcmp("11",mask))
{ 
strcpy(Mask,"255.224.0.0");
return Mask;
}
else if(!strcmp("12",mask))
{ 
strcpy(Mask,"255.240.0.0");
return Mask;
}
else if(!strcmp("13",mask))
{ 
strcpy(Mask,"255.248.0.0");
return Mask;
}
else if(!strcmp("14",mask))
{ 
strcpy(Mask,"255.252.0.0");
return Mask;
}
else if(!strcmp("15",mask))
{ 
strcpy(Mask,"255.254.0.0");
return Mask;
}
else if(!strcmp("16",mask))
{ 
strcpy(Mask,"255.255.0.0");
return Mask;
}
else if(!strcmp("17",mask))
{
strcpy(Mask,"255.255.128.0");
return Mask;
}
else if(!strcmp("18",mask))
{ 
strcpy(Mask,"255.255.192.0");
return Mask;
}
else if(!strcmp("19",mask))
{ 
strcpy(Mask,"255.255.224.0");
return Mask;
}
else if(!strcmp("20",mask))
{ 
strcpy(Mask,"255.255.240.0");
return Mask;
}
else if(!strcmp("21",mask))
{ 
strcpy(Mask,"255.255.248.0");
return Mask;
}
else if(!strcmp("22",mask))
{ 
strcpy(Mask,"255.255.252.0");
return Mask;
}
else if(!strcmp("23",mask))
{ 
strcpy(Mask,"255.255.254.0");
return Mask;
}
else if(!strcmp("24",mask))
{ 
strcpy(Mask,"255.255.255.0");
return Mask;
}
else if(!strcmp("25",mask))
{ 
strcpy(Mask,"255.255.255.128");
return Mask;
}
else if(!strcmp("26",mask))
{ 
strcpy(Mask,"255.255.255.192");
return Mask;
}
else if(!strcmp("27",mask))
{ 
strcpy(Mask,"255.255.255.224");
return Mask;
}
else if(!strcmp("28",mask))
{ 
strcpy(Mask,"255.255.255.240");
return Mask;
}
else if(!strcmp("29",mask))
{ 
strcpy(Mask,"255.255.255.248");
return Mask;
}
else if(!strcmp("30",mask))
{ 
strcpy(Mask,"255.255.255.252");
return Mask;
}
else if(!strcmp("31",mask))
{ 
strcpy(Mask,"255.255.255.254");
return Mask;
}
else if(!strcmp("32",mask))
{ 
strcpy(Mask,"255.255.255.255");
return Mask;
}

return NULL;
}

int separate(char* line,char** IP,char** MASK,char* MAC,char* STATIC,char* GATE,char* HOST,char* REMARK, char* MAC_LIST)

{
	//int i=0;
	int j=0;
	int k=0;
	int m=0;
	char* temp2 = NULL;
	char ip[64];
//	char* ip = NULL;

//	ip=malloc(sizeof(char) * 30);
	temp2=malloc(sizeof(char) * 1024);

	if(line[0] == NULL)
	{
		printf("error :line is empty !\n");
		return -1;
	}
		

		while(line[j])
		{
			if(line[j]=='\r'||line[j] == '\n')
			{
				temp2[m]='\0';
				break;
			}
			if(line[j]=='|')
			{
				temp2[m]='\0';
				k++;
				if(k==1) 
				{
					strtok(temp2, ";");
					strtok(NULL, ";");
					strcpy(MAC, strtok(NULL, ";"));
				//	strcpy(MAC,temp2);
					//printf("MAC=%s\n" , MAC);
				}	
				else if(k==2)
				{
					strcpy(ip,temp2);
					//printf("ip=%s\n" , ip);
				}	
				else if(k==3)
				{
					strcpy(STATIC,temp2);
					//printf("static = %s\n" , STATIC);
				}
				else if(k==4)
				{
					strcpy(GATE,temp2);	
					//printf("GATE = %s\n" , GATE);
				}
				else if(k==5)
				{
					strcpy(HOST,temp2);
					//printf("HOST = %s\n" , HOST);
				}
				else if(k==6)
				{	
					strcpy(REMARK,temp2);
				}
/*				else if(k==7)
				{
					strcpy(MAC_LIST,temp2);
					//printf("MAC_LIST = %s\n" , MAC_LIST);
				}*/
				m=0;
				if(line[j+1]=='\r'||line[j+1] == '\n') break;

			}
			else
			{
				temp2[m]=line[j];
				m++;
			}
			j++;
			
		}
		if(m > 0)
		{
			strcpy(MAC_LIST,temp2);
		}
		/*if(MAC[0])
		{
			char* p ;
			strtok(MAC,";");
			strtok(NULL,";");
			p = strtok(NULL , ";");
			strcpy(MAC , p);
	  	}*/
		if(ip[0])
		{
			strcpy(*IP,strtok(ip,"/"));
			*MASK=mask_turn(strtok(NULL,"/"));
		}
		free(temp2);

		//printf("mac=%s\n" , MAC);
		//free(ip);
}
int _separate(char* line,char* IP_MASK,char* MAC,char* STATIC,char* GATE,char* HOST,char* REMARK, char* MAC_LIST)
{
	//int i=0;
	int j=0;
	int k=0;
	int m=0;
	char* temp2;
	temp2=malloc(sizeof(char) * 1024);
	if(line[0] == NULL)
	{
		printf("error :line is empty !\n");
		return -1;
	}

		while(line[j])
		{
			if(line[j]=='\r'||line[j] == '\n') 
			{
				temp2[m] = '\0';
				break;
			}
			if(line[j]=='|')
			{
				temp2[m]='\0';
				k++;
				if(k==1)
				{ 
					strtok(temp2, ";");
					strtok(NULL, ";");
					strcpy(MAC, strtok(NULL, ";"));
				}	
				else if(k==2) strcpy(IP_MASK,temp2);	
				else if(k==3) strcpy(STATIC,temp2);	
				else if(k==4) strcpy(GATE,temp2);	
				else if(k==5) strcpy(HOST,temp2);
				else if(k==6) strcpy(REMARK,temp2);
	//			else if(k==7) strcpy(MAC_LIST,temp2);
			//	memset(temp1,0,1024);
				m=0;
				if(line[j+1]=='\r'||line[j+1] == '\n') break;

			}
			else
			{
				temp2[m]=line[j];
				m++;
			}
			j++;
			
		}
	if(m > 0)
	{
	 	strcpy(MAC_LIST,temp2);
		printf("temp2= %s\n" , temp2);
	}

/*	if(MAC[0])
	{
		char* p;
		strtok(MAC,";");
		strtok(NULL,";");
		p = strtok(NULL , ";");
		strcpy( MAC , p );
	}*/
	free(temp2);
	return 1;
}
void _separate_rule(char* line,char* IP_MASK,char* MAC,char* STATIC,char* GATE,char* HOST,char* REMARK)

{
	int i=0;
	int j=0;
	int k=0;
	int m=0;
	char* temp1;
	temp1=malloc(sizeof(char) * 100);

		while(line[i])
		{
			if(line[i]==';') line[i]='|';
			i++;
		}
		while(line[j])
		{
			if(line[j]=='|')
			{
				temp1[m]='\0';
				k++;
				if(k==3) strcpy(IP_MASK,temp1);	
				else if(k==4) strcpy(MAC,temp1);	
				else if(k==5) strcpy(STATIC,temp1);	
				else if(k==6) strcpy(GATE,temp1);	
				else if(k==7) strcpy(HOST,temp1);
			//	memset(temp1,0,1024);
				m=0;
				if(line[j+1]=='\r'||line[j+1]=='\n') break;
			}
			else
			{
				temp1[m]=line[j];
				m++;
			}
			j++;
			//printf("[%d]\n",j);
			//printf("[%s]\n",line[j]);
			
		}
		if(m>0)
		{
			temp1[m-2]='\0';
			//strcpy(REMARK,temp1);
		}
	free(temp1);
}

int ReadCfg()
{
	int n1=1;
	int n2=1;
	int n3=1;
	int n4=1;
	int k1=0;
	int k2=0;
	int k3=0;
	int k4=0;
	//int s1;
	int temp_offset=0;
	char line[LINEMAX]={0};
	int long p[6]={0};
	FILE* cfg;
	IpLine* ptr1;
	IpLine* ptr2;
	IpLine* ptr3;
	IpLine* ptr4;
	
	ptr1=calloc(4,sizeof(IpLine));
	ptr2=calloc(4,sizeof(IpLine));
	ptr3=calloc(4,sizeof(IpLine));
	ptr4=calloc(4,sizeof(IpLine));

	info_chan1.Head=ptr1;
	info_chan1.Size=4;
	info_chan2.Head=ptr2;
	info_chan2.Size=4;
	info_chan3.Head=ptr3;
	info_chan3.Size=4;
	info_chan4.Head=ptr4;
	info_chan4.Size=4;

	if(!(analysis(p,p+1,p+2,p+3,p+4,p+5))) printf("Addcfg:open cfgFile error;");
	 
	if((cfg=fopen(NetCfgFile,"rb"))==NULL) return -1;
	fseek(cfg,p[0],SEEK_SET);
	fgets(line,LINEMAX,cfg);
	temp_offset=ftell(cfg);
	while(temp_offset<p[1])
	{	
		fgets(line,LINEMAX,cfg);
		_separate(line,(ptr1+k1)->ip,(ptr1+k1)->mac,(ptr1+k1)->sta,(ptr1+k1)->gate,(ptr1+k1)->host,(ptr1+k1)->remark, (ptr1+k1)->mac_list);
		temp_offset=ftell(cfg);
		if(temp_offset==p[1]) break;
		else
		{
			n1++;
			k1++;
			if(n1>info_chan1.Size)
			{
				info_chan1.Size= info_chan1.Size* 2;
				info_chan1.Head= realloc(info_chan1.Head, info_chan1.Size* sizeof(IpLine));
				ptr1=info_chan1.Head;	
				memset(ptr1+k1,0,info_chan1.Size/ 2);	
			}
		}


	}
	
	info_chan1.Head=realloc(info_chan1.Head,n1* sizeof(IpLine));
	info_chan1.Size=n1;

	fseek(cfg,p[1],SEEK_SET);
	fgets(line,LINEMAX,cfg);
	temp_offset=ftell(cfg);

	while(temp_offset<p[2])
	{
		fgets(line,LINEMAX,cfg);
		_separate(line,(ptr2+k2)->ip,(ptr2+k2)->mac,(ptr2+k2)->sta,(ptr2+k2)->gate,(ptr2+k2)->host,(ptr2+k2)->remark,(ptr2+k2)->mac_list);
		temp_offset=ftell(cfg);
		if(temp_offset==p[2]) break;
		else
		{
			n2++;
			k2++;
			if(n2>info_chan2.Size)
			{
				info_chan2.Size= info_chan2.Size* 2;
				info_chan2.Head= realloc(info_chan2.Head, info_chan2.Size* sizeof(IpLine));
				ptr2=info_chan2.Head;	
				memset(ptr2+k2,0,info_chan2.Size/ 2);	
			}
		}
		
	}

	info_chan2.Head=realloc(info_chan2.Head,n2* sizeof(IpLine));
	info_chan2.Size=n2;
	
	fseek(cfg,p[3],SEEK_SET);
	fgets(line,LINEMAX,cfg);
	temp_offset=ftell(cfg);

	while(temp_offset<p[4])
	{
		fgets(line,LINEMAX,cfg);
		_separate(line,(ptr3+k3)->ip,(ptr3+k3)->mac,(ptr3+k3)->sta,(ptr3+k3)->gate,(ptr3+k3)->host,(ptr3+k3)->remark ,(ptr3+k3)->mac_list);
		temp_offset=ftell(cfg);
		if(temp_offset==p[4]) break;
		else
		{
			n3++;
			k3++;
			if(n3>info_chan3.Size)
			{
				info_chan3.Size= info_chan3.Size* 2;
				info_chan3.Head= realloc(info_chan3.Head, info_chan3.Size* sizeof(IpLine));
				ptr3=info_chan3.Head;	
				memset(ptr3+k3,0,info_chan3.Size/ 2);	
			}
		}
		
	}

	info_chan3.Head=realloc(info_chan3.Head,n3* sizeof(IpLine));
	info_chan3.Size=n3;
	
	fseek(cfg,p[4],SEEK_SET);
	fgets(line,LINEMAX,cfg);
	temp_offset=ftell(cfg);

	while(temp_offset<p[5])
	{
		fgets(line,LINEMAX,cfg);
		_separate(line,(ptr4+k4)->ip,(ptr4+k4)->mac,(ptr4+k4)->sta,(ptr4+k4)->gate,(ptr4+k4)->host,(ptr4+k4)->remark, (ptr4+k4)->mac_list);
		temp_offset=ftell(cfg);
		if(temp_offset==p[5]) break;
		else
		{
			n4++;
			k4++;
			if(n4>info_chan4.Size)
			{
				info_chan4.Size= info_chan4.Size* 2;
				info_chan4.Head= realloc(info_chan4.Head, info_chan4.Size* sizeof(IpLine));
				ptr4=info_chan4.Head;	
				memset(ptr4+k4,0,info_chan4.Size/ 2);	
			}
		}
		
	}

	info_chan4.Head=realloc(info_chan4.Head,n4* sizeof(IpLine));
	info_chan4.Size=n4;

	fclose(cfg);
	return 1;
}

char* Rule_change(char* line,Lines *line1,Lines *line2)
{
	char* temp_channel;
	char* temp_flag;
	char* temp_src;
	char* temp_dest;
	char* rest;
	char* new_line;

	new_line=malloc(sizeof(char)*1024);
	
	char src_host[30]={0};
	char dest_host[30]={0};
	
	int n1=0,n2=0;

	IpLine *p1,*p2;
	
	p1=line1->Head;
	p2=line2->Head;
	
	temp_channel=strtok(line,";");
	temp_flag=strtok(NULL,";");
	temp_src=strtok(NULL,"|");
	temp_dest=strtok(NULL,"|");
	rest=strtok(NULL,"\n");
	
	while(n1< line1->Size)
	{
		if(!strcmp(temp_src,(p1+n1)->ip))
		{
			strcpy(src_host,(p1+n1)->host);
			break;
		}
		n1++;
	}
	if(*src_host==NULL) strcpy(src_host,"All");
	
	while(n2< line2->Size)
	{
		if(!strcmp(temp_dest,(p2+n2)->ip))
		{
			strcpy(dest_host,(p2+n2)->host);
			break;
		}
		n2++;
	}
	if(*dest_host==NULL) strcpy(dest_host,"All");

	sprintf(new_line,"%s;%s;%s|%s|%s\n",temp_channel,temp_flag,src_host,dest_host,rest);
	new_line=realloc(new_line,sizeof(char)* (strlen(new_line)+ 1));

	return new_line;
}


void DeleteCfg_rule(int rownum,long int offset1,long int offset2)
{
	FILE* fp;
	char* sum;
	int temp_offset=0;
	int num_all=0; 
	char line[LINEMAX]={0};
	char* line_temp;
	int line_num = 0;	

	
	line_temp=malloc(sizeof(char)*LINEMAX);


	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 
	fseek(fp,0,SEEK_END);
	num_all=ftell(fp);
	fseek(fp,0,SEEK_SET);
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);

	while(temp_offset < offset1)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	fgets(line,LINEMAX,fp);
	strcat(sum,line);

	while(temp_offset < offset2)
	{	
		
	//	printf("%s\n",sum);
		line_num++;
		fgets(line,LINEMAX,fp);
		if(line_num == rownum) 
		{
			break;
			
		}
		strcat(sum,line);
		temp_offset=ftell(fp);
		
	}

	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		strcat(sum,line);
	}
	fclose(fp);


	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);


	free(sum);
	free(line_temp);
	sync();
}


void ReplaceCfg_rule(int rownum,char* buf,long int offset1,long int offset2)
{
	FILE* fp;
	char* sum;
	int temp_offset=0;
	int num_all=0;
	int line_num=0; 
	char line[LINEMAX]={0};	
	char* line_temp;	

	line_temp=malloc(sizeof(char)*300);
	
	if((fp=fopen(NetCfgFile,"rb"))==NULL)
	{
		printf("open cfgfile failed;");
	} 
	fseek(fp,0,SEEK_END);
	num_all=ftell(fp)+strlen(buf)+5;
	fseek(fp,0,SEEK_SET);
	sum=malloc(sizeof(char)*num_all);
	memset(sum,0,num_all);
	

	while(temp_offset < offset1)
	{
		fgets(line,LINEMAX,fp);
		strcat(sum,line);
	//	printf("%s\n",sum);
		temp_offset=ftell(fp);
	}
	fgets(line,LINEMAX,fp);
	strcat(sum,line);

	while(temp_offset < offset2)
	{	
		line_num++;
		fgets(line,LINEMAX,fp);
		if(line_num == rownum) 
		{
			strcat(sum,buf);
			strcat(sum,"\n");
			break;
			
		}
		strcat(sum,line);
		temp_offset=ftell(fp);
		
	}
	
	while(fgets(line,LINEMAX,fp)!=NULL)
	{
		strcat(sum,line);
	}
	fclose(fp);

	//printf("%s",sum);

	if((fp=fopen(NetCfgFile,"wb"))==NULL) printf("open netconfig failed;\n");
	fputs(sum,fp);
	fclose(fp);

	free(sum);
	free(line_temp);
	sync();
}

int mac_list(char* host, char* mac_l, int n)
{
	ReadCfg();
	int k = 0;
	IpLine* ptr;
	
	if (n == 1)
	{	
		ptr = info_chan1.Head;
		
		if(ptr == NULL)
			return -1;
	
		for(k=0; k < info_chan1.Size; k++)
		{		
			if (!strcmp(host, (ptr + k)->host))
			{
				if((ptr + k)->mac_list[0])
				{
					strcpy(mac_l, (ptr + k)->mac_list);
					return 1;
				}	
				else
				{
					//return -1;
				}
			}
		}
		
		printf("error: find not host!!\n");
		return -1;
	}
	else if(n == 2)
	{
		ptr = info_chan2.Head;
		
		if(ptr == NULL)
			return -1;
		
		for(k=0; k < info_chan2.Size; k++)
		{
			if (!strcmp(host, (ptr + k)->host))
			{
				if((ptr + k)->mac_list[0])
				{
					strcpy(mac_l, (ptr + k)->mac_list);
					return 1;
				}	
				else
				{
					return -1;
				}
			}
		}
		
		printf("error: find not host!!\n");
		return -1;
	}
	
	else if(n == 3)
	{
		ptr = info_chan3.Head;
		
		if(ptr == NULL)
			return -1;
		
		for(k=0; k < info_chan3.Size; k++)
		{
			if (!strcmp(host, (ptr + k)->host))
			{
				if((ptr + k)->mac_list[0])
				{
					strcpy(mac_l, (ptr + k)->mac_list);
					return 1;
				}	
				else
				{
					return -1;
				}
			}
		}
		
		printf("error: find not host!!\n");
		return -1;
	}
	
	else if(n == 4)
	{
		ptr = info_chan4.Head;
		
		if(ptr == NULL)
			return -1;
		
		for(k=0; k < info_chan4.Size; k++)
		{
			if (!strcmp(host, (ptr + k)->host))
			{
				if((ptr + k)->mac_list[0])
				{
					strcpy(mac_l, (ptr + k)->mac_list);
					return 1;
				}	
				else
				{
					return -1;
				}
			}
		}
		
		printf("error: find not host!!\n");
		return -1;
	}
	
	return 0;
}




