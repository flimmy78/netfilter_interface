#ifndef _PACKET_INFO_H
#define _PACKET_INFO_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/igmp.h>
#include <netinet/ip_icmp.h>
#include "mempool.h"


/* cut from /usr/include/linux/if_ether.h */
struct Ethhdr {
	unsigned char	h_dest[6];	/* destination eth addr	*/
	unsigned char	h_source[6];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
};

/* cut from /usr/include/netinet/ip.h */
struct Iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ihl:4;
    unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
    unsigned int version:4;
    unsigned int ihl:4;
#else
#error "Please fix <bits/endian.h>"    
#endif
    u_int8_t tos;
    u_int16_t tot_len;
    u_int16_t id;
    u_int16_t frag_off;
    u_int8_t ttl;
    u_int8_t protocol;
    u_int16_t check;
    u_int32_t saddr;
    u_int32_t daddr;
    /*The options start here. */
};

/* cut from /usr/include/netinet/tcp.h */
struct Tcphdr {
    u_int16_t source;
    u_int16_t dest;
    u_int32_t seq;
    u_int32_t ack_seq;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int16_t res1:4;
    u_int16_t doff:4;
    u_int16_t fin:1;
    u_int16_t syn:1;
    u_int16_t rst:1;
    u_int16_t psh:1;
    u_int16_t ack:1;
    u_int16_t urg:1;
    u_int16_t res2:2;
#elif __BYTE_ORDER == __BIG_ENDIAN
    u_int16_t doff:4;
    u_int16_t res1:4;
    u_int16_t res2:2;
    u_int16_t urg:1;
    u_int16_t ack:1;
    u_int16_t psh:1;
    u_int16_t rst:1;
    u_int16_t syn:1;
    u_int16_t fin:1;
#else
#error "Adjust your <bits/endian.h> defines"
#endif
    u_int16_t window;
    u_int16_t check;
    u_int16_t urg_ptr;	
};

/* cut from /usr/include/netinet/tcp.h FAVOR_BSD for convenience */
typedef u_int32_t tcp_seq;
struct BSDTcphdr 
  {
    u_int16_t th_sport;		/* source port */
    u_int16_t th_dport;		/* destination port */
    tcp_seq th_seq;		/* sequence number */
    tcp_seq th_ack;		/* acknowledgement number */
#  if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t th_x2:4;		/* (unused) */
    u_int8_t th_off:4;		/* data offset */
#  endif
#  if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t th_off:4;		/* data offset */
    u_int8_t th_x2:4;		/* (unused) */
#  endif
    u_int8_t th_flags;
#  define TH_FIN	0x01
#  define TH_SYN	0x02
#  define TH_RST	0x04
#  define TH_PUSH	0x08
#  define TH_ACK	0x10
#  define TH_URG	0x20
    u_int16_t th_win;		/* window */
    u_int16_t th_sum;		/* checksum */
    u_int16_t th_urp;		/* urgent pointer */
};

struct Udphdr {
  u_int16_t     source;
  u_int16_t     dest;
  u_int16_t     len;
  u_int16_t     check;
};

/* cut from /usr/include/netinet/ip_icmp.h */
struct Icmphdr
{
  u_int8_t type;		/* message type */
  u_int8_t code;		/* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t	id;
      u_int16_t	sequence;
    } echo;			/* echo datagram */
    u_int32_t	gateway;	/* gateway address */
    struct
    {
      u_int16_t	__unused;
      u_int16_t	mtu;
    } frag;			/* path mtu discovery */
  } un;
};

struct _session;
typedef struct _packet_info_struct {
	u_int8_t magic[4];

	struct _packet_info_struct *next;
	struct _packet_info_struct *prev;
	
	u_int8_t *raw_packet; 

	struct Ethhdr *eth_hdr;
	u_int8_t *eth_payload;

	struct Iphdr *ip_hdr;
	u_int8_t *ip_payload;

	struct Tcphdr *tcp_hdr;	
	u_int8_t *tcp_payload;

	struct Udphdr *udp_hdr;
	u_int8_t *udp_payload;

	struct Icmphdr *icmp_hdr;
	u_int8_t *icmp_payload;

	u_int16_t raw_packet_len;
	u_int16_t eth_payload_len;

	u_int16_t eth_proto;
	u_int16_t ip_payload_len;

	u_int16_t ip_length;
	u_int16_t ip_hlength;

	u_int32_t ip_sip;
	u_int32_t ip_dip;

	u_int16_t tcp_payload_len;
	u_int16_t tcp_hlength;

	u_int16_t tdp_sport;
	u_int16_t tdp_dport;

	u_int32_t tcp_seq;

	u_int16_t udp_payload_len;
	u_int16_t icmp_payload_len;

	u_int16_t ip_proto;
	char ifindex;
	char channel;

	u_int8_t session_id;
	u_int8_t proto_id;
#define PROTO_ID_NULL	0
#define PROTO_ID_TCP	1
#define PROTO_ID_UDP 	2
#define PROTO_ID_ARP 	3
#define PROTO_ID_ICMP 	4
#define PROTO_ID_IP		5

	u_int16_t action;
#define ACT_ZERO 		0x0
#define ACT_FORWARD	0x1
#define ACT_ACK 			0x2
#define ACT_WAIT		0x4
#define ACT_RETURN1BIT	0x8
#define ACT_RETURN1BIT_ANSWER		0x10
#define ACT_NO_FILTER_RULE			0x20

	struct _session *tcp_session;

	MemBucket *bucket; // mempool

	u_int32_t time_stamp; // 07-12-26,paused.
	u_int32_t resend_times; // 10-03-21, RESEND_SLOWDOWN
	u_int8_t stream_dir;	// paused.
	u_int8_t return_byte; // RETURN_1BIT
	u_int16_t datalen;

	
} Packet_t;
#define set_drop_flag(x) ((x) &= ~ACT_FORWARD);

#endif // _PACKET_INFO_H
