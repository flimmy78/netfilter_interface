#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "envfile.h"
#include <unistd.h>
#define DEBUG_CONF "/var/www/test"

/***************************************************************
 Usage : int get_debug_flag();
 Parameters: void
 Return: int flag  0:±í´ò÷Å¢  1£º±í²»´ò÷Å¢
 Do    : ¸ùBUG_CONFÎ¼þÊ·ñڣ¬·µ»Ø»ò
****************************************************************/
int get_debug_flag()
{
	int flag = 0;

	flag = access(DEBUG_CONF,F_OK);	
	return flag;
}

/****************************************************************************
** Usage : void printf_value(char *name, char *value);
** Parameters: 
               char *name£º ±íµ÷Å¢µÄû              char *value£º±íµ÷Å¢µÄµ
** Return: void
** Do    : ´ò÷Å¢£¬½«µ÷Å¢Ö¶¨Ïµ½ÁÍµÄ»¸öþµ±Ö
*****************************************************************************/
void printf_value(char *name, char *value)
{
	int debug_flag = 0;
	char tmp[1256];

	debug_flag = get_debug_flag();

	if(debug_flag == 0)
	{
		sprintf(tmp, "echo \"%s = %s\" >> %s", name, value, DEBUG_CONF);
		system(tmp);
	}

	return ;
}

void printf_int(char *name, int value)
{
	int debug_flag = 0;
	char tmp[1256];

	debug_flag = get_debug_flag();

	if(debug_flag == 0)
	{
		sprintf(tmp, "echo \"%s = %d\" >> %s", name, value, DEBUG_CONF);
		system(tmp);
	}

	return ;
}

int GetUsercfg()
{
	FILE* usercfg;
	int i=0,j=0;
	int UserNum=0;	
	char tempstr[20];	
	char line[LINEMAX];
	if((usercfg=fopen(UserCfgFile,"r"))==NULL) return 0;
	while(fgets(line,LINEMAX,usercfg)!=NULL)
	{
		while(line[i])
		{
			if(line[i]=='=')
			{
				tempstr[j]='\0';
				j=0;
				i++;
				if(!strcmp(tempstr,"user"))
				{
					while(line[i]!='\n')
					{
						tempstr[j]=line[i];
						j++;
					 	i++;
					}
					tempstr[j]='\0';
					strcpy(User[UserNum].username,tempstr);
					break;
					
				}
				else if(!strcmp(tempstr,"pswd"))
				{
					while(line[i]!='\n')
					{
						tempstr[j]=line[i];
						j++;
					 	i++;
					}
					tempstr[j]='\0';
					strcpy(User[UserNum].password,tempstr);
					break;
				}
				else if(!strcmp(tempstr,"role"))
				{
					while(line[i]!='\n')
					{
						tempstr[j]=line[i];
						j++;
					 	i++;
					}
					tempstr[j]='\0';
					User[UserNum].role=atoi(tempstr);
					UserNum++;
					break;
				}
				
			}
			tempstr[j]=line[i];
			i++;
			j++;	
		}
		j=0;
		i=0;
	}
	fclose(usercfg);
	return 1;
}


int UserAuth(char* usrname,char* passwd)
{
	int i=0;
	//int UserNum;	
	for(i=0;i<3;i++)
	{
		if(!strcmp(User[i].username,usrname))
		{
			if(!strcmp(User[i].password,passwd)) return 0;
			else return User[i].role;
		}
	}
	return -1;
		
			
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
