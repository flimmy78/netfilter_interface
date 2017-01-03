/*
 * cfg2xml.c
 *
 *  Created on: 2010-7-26
 *      Author: Administrator
 */

#include "cfg2xml.h"

void changeCfgToXml(){
	system("touch /var/www/qweasd");
	xmlDocPtr     pdoc = NULL;
	xmlNodePtr    proot_node = NULL ,pnode = NULL , pnode1 = NULL;

	// 创造一个新文档并设置要root节点
	// 在XML文档中有且只有一个root节点
	pdoc = xmlNewDoc( BAD_CAST "1.0");
	proot_node  = xmlNewNode(NULL , BAD_CAST "plist");
	xmlNewProp(proot_node , BAD_CAST "version" , BAD_CAST "1.0");
	xmlDocSetRootElement(pdoc , proot_node);
	pnode = xmlNewNode( NULL , BAD_CAST "dict" );
	// 创造root节点的子节点
	xmlNewChild(pnode , NULL , BAD_CAST "key" , BAD_CAST "info");
	pnode1 = xmlNewNode( NULL , BAD_CAST "dict" );

	//保存文档
	xmlSaveFormatFileEnc("/mytest.xml", pdoc, "UTF-8", 1);

	//释放文档指针
	xmlFreeDoc(pdoc);
	xmlCleanupParser();

	xmlMemoryDump();      //debug memory for regression tests
}



