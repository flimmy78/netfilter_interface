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

	// ����һ�����ĵ�������Ҫroot�ڵ�
	// ��XML�ĵ�������ֻ��һ��root�ڵ�
	pdoc = xmlNewDoc( BAD_CAST "1.0");
	proot_node  = xmlNewNode(NULL , BAD_CAST "plist");
	xmlNewProp(proot_node , BAD_CAST "version" , BAD_CAST "1.0");
	xmlDocSetRootElement(pdoc , proot_node);
	pnode = xmlNewNode( NULL , BAD_CAST "dict" );
	// ����root�ڵ���ӽڵ�
	xmlNewChild(pnode , NULL , BAD_CAST "key" , BAD_CAST "info");
	pnode1 = xmlNewNode( NULL , BAD_CAST "dict" );

	//�����ĵ�
	xmlSaveFormatFileEnc("/mytest.xml", pdoc, "UTF-8", 1);

	//�ͷ��ĵ�ָ��
	xmlFreeDoc(pdoc);
	xmlCleanupParser();

	xmlMemoryDump();      //debug memory for regression tests
}



