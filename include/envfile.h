#ifndef ENVFILE_H 
#define ENVFILE_H 

#define LogFile "/var/www/log/log_record.log"
#define LogError "/var/www/log/log_error.log"
#define UserCfgFile "/var/www/log/user.cfg"
#define NetCfgFile "/usr/dfgap/dfgap-cfg"
#define HtmlShow "/var/www/html/policy_setting.htm"
#define DEVICEINFO "/home/jiekou/src/deviceinfo.xml"
#define LINEMAX 1024 

typedef struct{
	char username[20];
	char password[20];
	int role;}LoginUser;
LoginUser User[5];

typedef struct{
	char mac[30];
	char sta[25];
	char gate[2];
	char host[16];
	char remark[100];
	char ip[25];
	char mac_list[1024];
}IpLine;

typedef struct{
	IpLine *Head;
	int Size;
	}Lines;
Lines info_chan1,info_chan2,info_chan3,info_chan4;

typedef struct{
	char ip_src[25];
	char ip_dest[25];
	char xieyi[5];
	char duankou[8];
	char sign[2];}RuleLine;

	
extern long int Ipcfg_offset[5];
extern int UserNum;
int UserAuth(char*,char*);
int GetUsercfg();
void DeleteCfg();
void ReplaceCfg();
void ShowCfg(char*);
char* mask_turn(char*);
int analysis(long int*,long int*,long int*,long int*,long int*,long int*);
void AddNetcfg(char*,long int);
int separate(char*,char**,char**,char*,char*,char*,char*,char*,char*);
int _separate(char*,char*,char*,char*,char*,char*,char*,char*);
int mac_list(char*,char*,int);
int ReadCfg();
char* Rule_change(char*,Lines*,Lines*);
void printf_value(char *name, char *value);
void printf_int(char *name, int value);
#endif
