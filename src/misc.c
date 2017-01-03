#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <sched.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "misc.h"

char *ipaddr2string(ulong addr, char *buf, int size)
{
	struct in_addr in;
	char *p;
	
	in.s_addr = addr;
	
	p = inet_ntoa(in);
	if (buf != NULL && size > 0) {
		int ncpy = 15 > size-1 ? size-1 : 15;
		strncpy(buf, p, ncpy);
		buf[ncpy] = 0;
		return buf;
	} else
		return NULL;

}

int
parse_mac(const char *mac, char *macbuf)
{
	unsigned int i = 0;

	if (strlen(mac) != 6*3-1) {
		fprintf(stderr, "Bad mac address '%s'", mac);
		goto err;
	}	

	for (i = 0; i < 6; i++) {
		long number;
		char *end;

		number = strtol(mac + i*3, &end, 16);

		if (end == mac + i*3 + 2
		    && number >= 0
		    && number <= 255)
			macbuf[i] = number;
		else {
			fprintf(stderr, "Bad mac address '%s'", mac);
			goto err;
		}

	}
	return 0;
	
err:
	return -1;
}

void print_buf(unsigned char *buf, int nbuf, unsigned int column)
{
	int i;
	int col;

	if (column == 0)
		col = 30;
	else
		col = column;

	for (i = 0; i < nbuf; i++) {
		N_PRINTF("%x%x ", buf[i]>>4, buf[i]&0x0f);
		if ((i+1) % col == 0)
			N_PRINTF("\n");
	}
	N_PRINTF("\n");

	return;
}

void print_buf2(unsigned char *buf, int nbuf, unsigned int column)
{
	int i, j;
	int col;
	unsigned char ch;

	if (column == 0)
		col = 16;
	else
		col = column;

	for (i = 0; i < nbuf; i++) {
		printf("%x%x ", buf[i] >> 4, buf[i] & 0xf);
		if ((i + 1) % col == 0) {
			for (j = 0; j < col; j++) {
				ch = *(buf + i + j - (col-1));
				printf("%c", isprint (ch) ? ch : '.');
			}
			printf("\n");
		}
	}
	for (j = 0; j < col - i % col; j++)
		printf("   ");
	for (j = 0; j < i % col; j++) {
		ch = *(buf + i + j - i % col);
		printf("%c", isprint (ch) ? ch : '.');
	}
	printf("\n");

	return;
}

// 
char *search_nosense_char(char *begin, int nsearch, int step)
{
	char *p;
	char *pret = NULL;
	int i;

	p = begin;
	for (i = 0; i < nsearch; i++) {
		if (*p == 0 || *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
			pret = p;
			goto ret;
		}

		if (step >= 0)
			p++;
		else 
			p--;
	}

ret:
	return pret;
}

// 
char *search_sense_char(char *begin, int nsearch, int step)
{
	char *p;
	char *pret = NULL;
	int i;

	p = begin;
	for (i = 0; i < nsearch; i++) {
		if (*p != 0 && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r') {
			pret = p;
			goto ret;
		}

		if (step >= 0)
			p++;
		else 
			p--;
	}

ret:
	return pret;
}

/*
 *
 */
int char2num(char p)
{
	int ret;

	if ((p >= '0') && (p <= '9'))	
		ret = p - '0';
	else if ((p >= 'a') && (p <= 'f'))	
		ret = p - 'a' + 10;
	else if ((p >= 'A') && (p <= 'F'))
		ret = p - 'A' + 10;	
	else
		ret = p;

	return ret;
}

char num2char(int p)
{
	int ret;

	if ((p >= 0) && (p <= 9)) {
		ret = p + '0';
	} else if ((p >= 10) && (p <= 15)) {
		ret = p - 10 + 'a';
	} else
		ret = 0;

	return ret;
}

/*
 *
 */
int asc2hex(char *src, int slen, unsigned char *dst, int dmaxlen)
{
	char *p;
	int cnt = 0;

	p = src;
	while(slen > 0 && cnt < dmaxlen)
	{
		dst[cnt] = (char2num(p[0]) * 16 + char2num(p[1]));
		slen -= 2;
		cnt++;
		p = p + 2;
	}

	return cnt;
}


/*
 *
 */
int hex2asc(unsigned char *src, int slen, char *dst, int dmaxlen)
{
	int dlen = 0;
	int si = 0;

	for (si = 0; si < slen && dlen < dmaxlen-1; si++) {
		int high = src[si]>>4;
		int low = src[si] & 0xf;

		dst[dlen] = num2char(high);
		dst[dlen+1] = num2char(low);

		dlen += 2;
	}

	return dlen;
}


/*
 * function:
 *    copy no more than [destlen - 1] bytes from [src] to [dest], stop
 * when the character [c] or '\0' found.
 *
 * return: 
 *    if [c] is found, return the pointer pointing the charater after 
 * [c]; if '\0' is found, return the pointer pointing the '\0'; else
 * return the pointer pointing after [destlen - 1] charaters.
 *    if src is NULL, set dest[0]='\0' and return NULL.
 */
char* strnchcpy(char *src, char *dest, int destlen, int c)
{
	int i = 0;
	char *pstr = src;

	if (src == NULL)
	{
		dest[0] = 0;
		return NULL;
	}

	while ((*pstr != c) && (*pstr != '\0') && (i < destlen - 1) && (*pstr != '\r') && (*pstr != '\n'))
	{
		dest[i] = *pstr;
		pstr++;
		i++;
	}

	dest[i] = '\0';
	if (*pstr == c)
		pstr++;

	return pstr;
}
