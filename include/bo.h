#ifndef BO_H 
#define BO_H

#include"interface.h" 

extern  int get_baseinfo(  char *xmlfilepath) ;				      	/* 获取基本信息*/

extern  int add_safedev(Data ds, char *xmlfilepath) ;					/* 添加安全设备*/
	
extern  int confirm_safedev(Data ds, char *xmlfilepath) ;				/* 确认安全设备 */

extern int del_safedev(Data ds, char *xmlfilepath )  ;					/* 删除安全设备 */

extern int modi_safedevip(Data ds, char *xmlfilepath);				        /*安管IP指向修改*/
	
extern int set_baseinfo(Data ds , char *xmlfilepath) 	;				/* 基本信息设置 */

extern int get_packfilter_rule (Data ds, char *xmlfilepath) ; // 包过滤策略获取
#endif

