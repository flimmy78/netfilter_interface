#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
char InputBuffer[4096];
int main(int argc, char *argv[])
{
	int ContentLength; /*数据长度*/
	int x;
	int i;
	char *p;
	char *pRequestMethod; /*   METHOD属性值   */
	setvbuf(stdin, NULL, _IONBF, 0); /*关闭stdin的缓冲*/
	printf("Content-type:   text/html/n"); /*从stdout中输出，告诉Web服务器返回的信息类型*/
	printf("/n"); /*插入一个空行，结束头部信息*/
	/*   从环境变量REQUEST_METHOD中得到METHOD属性值   */
	pRequestMethod = getenv("REQUEST_METHOD");
	if (pRequestMethod == NULL)
	{
		return 0;
	}
	if (strcmp(pRequestMethod, "POST") == 0)
	{
		p = getenv("CONTENT_LENGTH"); /*从环境变量CONTENT_LENGTH中得到数据长度*/
		if (p != NULL)
		{
			ContentLength = atoi(p);
		}
		else
		{
			ContentLength = 0;
		}
		if (ContentLength > sizeof(InputBuffer) - 1)
		{
			ContentLength = sizeof(InputBuffer) - 1;
		}
		i = 0;
		while (i < ContentLength)
		{ /*从stdin中得到Form数据*/
			x = fgetc(stdin);
			if (x == EOF)
				break;
			InputBuffer[i++] = x;
		}
		InputBuffer[i] = '/0';
		ContentLength = i;
		DecodeAndProcessData(); /*具体译码和处理数据，该函数代码略*/
	}
	else if (_stricmp(pRequestMethod, "GET") == 0)
	{
		p = getenv("QUERY_STRING"); /*从环境变量QUERY_STRING中得到Form数据*/
		if (p != NULL)
		{
			strncpy(InputBuffer, p, sizeof(InputBuffer));
			DecodeAndProcessData(); /*具体译码和处理数据，该函数代码略*/
		}
	}
	printf("<HEAD><TITLE>Submitted   OK</TITLE></HEAD>/n"); /*从stdout中输出返回信息*/
	printf(
			"<BODY>The   information   you   supplied   has   been   accepted.</BODY>/n");
	return 0;
}
