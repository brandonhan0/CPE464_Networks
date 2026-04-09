#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pcap.h>
#include <arpa/inet.h>

void ethernet(const unsigned char *packet, int packet_num);
void arp(const unsigned char *packet);
void ip(const unsigned char *packet);
void icmp(const unsigned char *packet, int ip_header_len);
void tcp(const unsigned char *packet, int ip_header_len);
void udp(const unsigned char *packet, int ip_header_len);


#pragma pack(push, 1)
typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t ether_type;
} ethernet_o;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t identifier;
    uint16_t seq_num;
} icmp_o;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;

    uint8_t sha[6];  // sender hardware addr
    uint8_t spa[4];  // sender protocol addr
    uint8_t tha[6];  // target hardware addr
    uint8_t tpa[4];  // target protocol addr
} arp_o;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t ver_ihl;        // version + header length
    uint8_t tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_frag;

    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;

    uint32_t src_ip;
    uint32_t dst_ip;
} ip_o;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint16_t src_port;
    uint16_t dst_port;

    uint32_t seq_num;
    uint32_t ack_num;

    uint8_t data_offset;   // upper 4 bits used
    uint8_t flags;

    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_ptr;
} tcp_o;
#pragma pack(pop)

