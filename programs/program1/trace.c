#include "trace.h"


void ethernet(const unsigned char *packet, int packet_num){
}
void arp(const unsigned char *packet){}
void ip(const unsigned char *packet){}
void icmp(const unsigned char *packet, int ip_len){}
void tcp(const unsigned char *packet, int ip_len){}
void udp(const unsigned char *packet, int ip_len){}


int main(int argc, char *argv[]) {


    /*
    Ethernet -> ARP or IP
    If IP -> read header
        can be ICMP, TCP, UDP
        
    */



    pcap_t *handle;
    char errbuf[256];
    struct pcap_pkthdr *header;
    const unsigned char *packet;
    int result;
    int packet_num = 1;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <trace file>\n", argv[0]);
        return 1;
    }

    handle = pcap_open_offline(argv[1], errbuf);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_offline failed: %s\n", errbuf);
        return 1;
    }

    while ((result = pcap_next_ex(handle, &header, &packet)) >= 0) {
        ethernet_o *eth;
        uint16_t ether_type;

        if (result == 0) {
            continue;
        }

        if (header->caplen < sizeof(ethernet_o)) {
            continue;
        }

        eth = (ethernet_o *) packet;
        ether_type = ntohs(eth->ether_type);

        printf("Packet number: %d\n", packet_num);
        ethernet(packet, packet_num);

        if (ether_type == 0x0806) {
            arp(packet + sizeof(ethernet_o));
        } else if (ether_type == 0x0800) {
            ip_o *ip_hdr;
            int ip_len;

            if (header->caplen < sizeof(ethernet_o) + sizeof(ip_o)) {
                packet_num++;
                continue;
            }

            ip_hdr = (ip_o *)(packet + sizeof(ethernet_o));
            ip_len = (ip_hdr->ver_ihl & 0x0F) * 4;

            ip(packet + sizeof(ethernet_o));

            if (ip_hdr->protocol == 1) {
                icmp(packet + sizeof(ethernet_o) + ip_len, ip_len);
            } else if (ip_hdr->protocol == 6) {
                tcp(packet + sizeof(ethernet_o) + ip_len, ip_len);
            } else if (ip_hdr->protocol == 17) {
                udp(packet + sizeof(ethernet_o) + ip_len, ip_len);
            } else {
                printf("what is this");
            }
        }
        printf("\n");
        packet_num++;
    }

    if (result == -1) {
        fprintf(stderr, "error");
        pcap_close(handle);
        return 1;
    }

    pcap_close(handle);
    return 0;
}