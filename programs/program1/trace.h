#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netinet/ether.h> // ether_ntoa


int ethernet(const unsigned char *packet);
void arp(const unsigned char *packet);
int ip(const unsigned char *packet);
void icmp(const unsigned char *packet, int ip_header_len);
void tcp(const unsigned char *packet, int ip_header_len);
void udp(const unsigned char *packet, int ip_header_len);


#pragma pack(push, 1)
typedef struct {
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint16_t type;
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
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_len;
    uint8_t protocol_len;
    uint16_t opcode;

    uint8_t src_mac[6]; // sender hardware addr
    uint8_t src_ip[4]; // sender protocol addr
    uint8_t dst_mac[6]; // target hardware addr
    uint8_t dst_ip[4]; // target protocol addr
} arp_o;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint8_t version_header_len; // header_len is in words so like 0101 =5 and than it would be size 20 bc 20 bytes
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t id;
    uint16_t flags_frag;

    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;

    uint8_t src_ip[4];
    uint8_t dst_ip[4];
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

