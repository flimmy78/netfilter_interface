/* CLOSURE_MODE */

#ifndef _MISC_H
#define _MISC_H
#include <stdio.h>
#include <stdlib.h>

int parse_mac(const char *mac, char *macbuf);
char* strnchcpy(char *src, char *dest, int destlen, int c);
char *ipaddr2string(ulong addr, char *buf, int size);
void print_buf(unsigned char *buf, int nbuf, unsigned int column);
void print_buf2(unsigned char *buf, int nbuf, unsigned int column);

int char2num(char p);
char num2char(int p);
char *search_nosense_char(char *begin, int nsearch, int step);
char *search_sense_char(char *begin, int nsearch, int step);

int asc2hex(char *src, int slen, unsigned char *dst, int dmaxlen);
int hex2asc(unsigned char *src, int slen, char *dst, int dmaxlen);

extern inline void rest(void);

/* 
 * unsigned int comparison
 */
#define UINT_GT(x,y)     ((y) - (x) > 0x7fffffff)
#define UINT_LT(x,y)     ((x) - (y) > 0x7fffffff)
#define UINT_EQ(x,y)     ((x) == (y))
#define UINT_GE(x,y)     ((x) - (y) < 0x7fffffff)
#define UINT_LE(x,y)     ((y) - (x) < 0x7fffffff)


#ifndef MIN
#define MIN(a, b)	((a) > (b) ? (b) : (a))
#endif //MIN

#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif //MAX

#ifdef CLOSURE_MODE
#define N_PRINTF(...) \
	do {\
	} while(0)

#define N_FPRINTF(...) \
	do {\
	} while(0)

#define sN_PRINTF(...) \
	do {\
		sprintf(__VA_ARGS__); \
	} while(0)

#define snN_PRINTF(...) \
	do {\
		snprintf(__VA_ARGS__); \
	} while(0)


#define fN_PRINTF(...) \
	do {\
	} while(0)

#define N_SPRINTF(...) \
	do {\
	} while(0)

#else

#define N_PRINTF(...) \
	do {\
		printf(__VA_ARGS__); \
	} while(0)

#define N_FPRINTF(...) \
	do {\
		fprintf(__VA_ARGS__); \
	} while(0)

#define sN_PRINTF(...) \
	do {\
		sprintf(__VA_ARGS__); \
	} while(0)

#define snN_PRINTF(...) \
	do {\
		snprintf(__VA_ARGS__); \
	} while(0)

#define fN_PRINTF(...) \
	do {\
		fprintf(__VA_ARGS__); \
	} while(0)

#define N_SPRINTF(...) \
	do {\
		sprintf(__VA_ARGS__); \
	} while(0)

#endif

#ifndef offset_of
#define offset_of(str, member)  ((char *)(&((str *)0)->member) - (char *)0)
#endif

#endif // _MISC_H
