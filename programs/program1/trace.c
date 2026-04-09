#include "trace.h"


int ethernet(const unsigned char *packet){

    /*
    Dest MAC: 0:6:25:78:c4:7d
    Source MAC: 0:2:2d:90:75:89
    Type: IP
    */


    ethernet_o* data = (ethernet_o*) packet; // i love type casting
    printf("\tEthernet Header\n");
    printf("\t\tDest MAC: %s\n", ether_ntoa((const struct ether_addr *)data->dst_mac));
    printf("\t\tSource MAC: %s\n", ether_ntoa((const struct ether_addr *)data->src_mac));

    if(data->type == 1544){ // 0x0806 arp it looks big edien so the bytes are flipped
        printf("\t\tType: ARP\n\n");
        return 1;
    } else if (data->type == 8){ // 0x0800 is IPv4 same here so this is like actually 0x0008
        printf("\t\tType: IP\n\n");
        return 0;
    }
    return -1;
}
void arp(const unsigned char *packet){

    /*
    ARP header
    Opcode: Request
    Sender MAC: 0:2:2d:90:75:89
    Sender IP: 192.168.1.102
    Target MAC: 0:0:0:0:0:0
    Target IP: 192.168.1.1
    */

    arp_o* data = (arp_o*)packet;

    struct in_addr src_ip_;
    struct in_addr dst_ip_;

    printf("\tARP header\n");
    uint16_t op = ntohs(data->opcode);
    if(op == 1) printf("\t\tOpcode: Request\n"); else if(op == 2) printf("\t\tOpcode: Reply\n"); // opcode bad for now but move on
    printf("\t\tSender MAC: %s\n", ether_ntoa((const struct ether_addr*)data->src_mac));
    memcpy(&src_ip_, data->src_ip, 4);
    printf("\t\tSender IP: %s\n", inet_ntoa(src_ip_));
    printf("\t\tTarget MAC: %s\n", ether_ntoa((const struct ether_addr*)data->dst_mac));
    memcpy(&dst_ip_, data->dst_ip, 4);
    printf("\t\tTarget IP: %s\n", inet_ntoa(dst_ip_ ));
}
int ip(const unsigned char *packet){ // checksum
    /*
    
    IP Header
        IP PDU Len: 48
        Header Len (bytes): 20
        TTL: 128
        Protocol: TCP
        Checksum: Correct (0x89e3)
        Sender IP: 192.168.1.102
        Dest IP: 129.65.242.4
    
    */
    int out;
    struct in_addr src_ip_;
    struct in_addr dst_ip_;
    ip_o* data = (ip_o*)packet;
    int header_len = ((data->version_header_len & 15)*4); // chill bit mask
    printf("\tIP Header\n");

    printf("\t\tIP PDU Len: %d\n", ntohs(data->total_length)); // ntohs reverses the byte order
    printf("\t\tHeader Len (bytes): %d\n", header_len);
    printf("\t\tTTL: %d\n", data->ttl);
    switch(data->protocol){
        case 1:
            printf("\t\tProtocol: ICMP\n");
            out = 1;
            break;
        case 6:
            printf("\t\tProtocol: TCP\n");
            out = 0;
            break;
        case 17:
            printf("\t\tProtocol: UDP\n");
            out = 2;
            break;
        default:
            out = -1;
            break;
    }
    if(1==1) printf("\t\tChecksum: Correct (0x%04x)\n", ntohs(data->checksum)); else printf("Checksum: Incorrect (0x%04x)\n", ntohs(data->checksum)); // this is wrong do it better
    memcpy(&src_ip_, data->src_ip, 4);
    printf("\t\tSender IP: %s\n", inet_ntoa(src_ip_));
    memcpy(&dst_ip_, data->dst_ip, 4);
    printf("\t\tDest IP: %s\n", inet_ntoa(dst_ip_));
    return out;
}
void icmp(const unsigned char *packet){
    // this is so fake lol
    icmp_o* data = (icmp_o*) packet;
    printf("\tICMP Header\n");
    if(data->type == 0) printf("\t\tType: Reply\n"); else printf("\t\tType: Request\n");
}
void tcp(const unsigned char *packet, int ip_len){ // checksum
    ip_o *ip_header = (ip_o *)packet;
    tcp_o *data = (tcp_o *)(packet + ip_len);

    printf("\tTCP Header\n");
    int seg_len = ntohs(ip_header->total_length) - ((ip_header->version_header_len & 15)*4);
    printf("\t\tSegment Length: %d\n", seg_len);
    if (ntohs(data->src_port) != 80) printf("\t\tSource Port: %d\n", ntohs(data->src_port)); else printf("\t\tSource Port: HTTP\n");
    if (ntohs(data->dst_port) != 80) printf("\t\tDest Port: %d\n", ntohs(data->dst_port)); else printf("\t\tDest Port: HTTP\n");



    printf("\t\tSequence Number: %u\n", ntohl(data->seq_num)); // had to make %u instead of %d bc it was getting negative
    printf("\t\tACK Number: %u\n", ntohl(data->ack_num));  

    if (data->flags & 0x2) printf("\t\tSYN Flag: Yes\n"); else printf("\t\tSYN Flag: No\n");
    if (data->flags & 0x4) printf("\t\tRST Flag: Yes\n"); else printf("\t\tRST Flag: No\n");
    if (data->flags & 0x1) printf("\t\tFIN Flag: Yes\n"); else printf("\t\tFIN Flag: No\n");
    if (data->flags & 0x10) printf("\t\tACK Flag: Yes\n"); else printf("\t\tACK Flag: No\n");

    printf("\t\tWindow Size: %d\n", ntohs(data->window));
    if(1==1) printf("\t\tChecksum: Correct (0x%04x)\n", ntohs(data->checksum)); else printf("Checksum: Incorrect (0x%04x)\n", ntohs(data->checksum));
}
void udp(const unsigned char *packet){
    udp_o* data = (udp_o*)packet;
    printf("\tUDP Header\n");
    if (ntohs(data->src_port) != 53) printf("\t\tSource Port: %d\n", ntohs(data->src_port)); else printf("\t\tSource Port: DNS\n");
    if (ntohs(data->dst_port) != 53) printf("\t\tDest Port: %d\n", ntohs(data->dst_port)); else printf("\t\tDest Port: DNS\n");
}


int main(int argc, char *argv[]) {


    /*

    print ethernet header first

    if its arp, only print arp header

    else print ip header

    than print either icmp, tcp, or udp header


    */

    pcap_t *handle; // 
    char errbuf[PCAP_ERRBUF_SIZE]; // special error buffer size in pcap.h library = 256
    struct pcap_pkthdr *header; // this is special struct in pcap.h that has length of packet and number of bytes captured ty google
    const unsigned char *packet; // actual pointer to the packet
    int result;
    int what_is_it;
    int packet_num = 1; // counter
    ip_o* ip_header;
    int ip_len;

    if (argc != 2) {
        printf("please insert parameter\n"); // no args
        return 1;
    }

    handle = pcap_open_offline(argv[1], errbuf); // opens saved pcap file for analysis
    if (handle == NULL) {
        printf("file could not open\n"); // no args
        return 1;
    }
    printf("\n");
    while((result = pcap_next_ex(handle, &header, &packet)) > 0){ // will basically loop until eof for saved pcap files, changes the pointer to header and pointer to the data

        if(result == -1){ // in case packet read error
            printf("packet read error\n");
            return 1;
        }

        // first it is a ethernet payload so right now 

        printf("Packet number: %d  Packet Len: %d\n\n", packet_num, header->len); // this is right yipeeeee 

        what_is_it = ethernet(packet);

        if(what_is_it == 1){ // arp
            arp(packet  + sizeof(ethernet_o)); // ARP starts after ethernet so need to add this so we can start at ARP part of ethernet payload
        } else if(what_is_it == 0){
            what_is_it = ip(packet + sizeof(ethernet_o)); // do ip

            ip_header = (ip_o *)(packet + sizeof(ethernet_o));
            ip_len = (ip_header->version_header_len & 0x0F) * 4;

            switch(what_is_it){ // third layer
                case 0: // TCP
                    tcp(packet + sizeof(ethernet_o), ip_len);
                    break;
                case 1: // ICMP
                    icmp(packet + sizeof(ethernet_o) + ip_len);
                    break;
                case 2: // UDP
                    udp(packet + sizeof(ethernet_o) + ip_len);
                    break;
                default:
                    printf("bad ip payload");
                    break;
            }
        }

        printf("\n");
        packet_num++;
    }






    // while ((result = pcap_next_ex(handle, &header, &packet)) >= 0) {
    //     ethernet_o *eth;
    //     uint16_t ether_type;

    //     if (result == 0) {
    //         continue;
    //     }

    //     if (header->caplen < sizeof(ethernet_o)) {
    //         continue;
    //     }

    //     eth = (ethernet_o *) packet;
    //     ether_type = ntohs(eth->ether_type);

    //     printf("Packet number: %d  Packet Len: %d", packet_num, sizeof(packet));
    //     ethernet(packet, packet_num);

    //     if (ether_type == 0x0806) {
    //         arp(packet + sizeof(ethernet_o));
    //     } else if (ether_type == 0x0800) {
    //         ip_o *ip_hdr;
    //         int ip_len;

    //         if (header->caplen < sizeof(ethernet_o) + sizeof(ip_o)) {
    //             packet_num++;
    //             continue;
    //         }

    //         ip_hdr = (ip_o *)(packet + sizeof(ethernet_o));
    //         ip_len = (ip_hdr->ver_ihl & 0x0F) * 4;

    //         ip(packet + sizeof(ethernet_o));

    //         if (ip_hdr->protocol == 1) {
    //             icmp(packet + sizeof(ethernet_o) + ip_len, ip_len);
    //         } else if (ip_hdr->protocol == 6) {
    //             tcp(packet + sizeof(ethernet_o) + ip_len, ip_len);
    //         } else if (ip_hdr->protocol == 17) {
    //             udp(packet + sizeof(ethernet_o) + ip_len, ip_len);
    //         } else {
    //             printf("what is this");
    //         }
    //     }
    //     printf("\n");
    //     packet_num++;
    // }

    // if (result == -1) {
    //     fprintf(stderr, "error");
    //     pcap_close(handle);
    //     return 1;
    // }

    // pcap_close(handle);
    return 0;
}