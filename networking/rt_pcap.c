#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_LEN     65536

void ip_hdr(unsigned char* buf);
void ethr_hdr(unsigned char* buf);
void process_packet(unsigned char* buf, size_t plen);

FILE *logdir;
struct sockaddr_in  src, dst;

int main()
{
    int             fd;
    size_t          slen, dlen;
    struct sockaddr saddr;
    unsigned char   *buf = (unsigned char *)malloc(MAX_LEN);

    slen = sizeof(saddr);

    logdir = fopen("out.log","w");
    if(NULL == logdir) {
        printf("Unable to create out.log file\n");
        return -1;
    }

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) ;
    if(fd < 0) {
        perror("Socket Error");
        return -1;
    }

    while(1) {
        dlen = recvfrom(fd, buf, MAX_LEN, 0, &saddr, (socklen_t *)&slen);
        if(dlen < 0) {
            printf("Recvfrom error , failed to get packets\n");
            goto bailout;
        }

        process_packet(buf, dlen);
    }

bailout:
    if(fd)
        close(fd);
    return 0;
}

void process_packet(unsigned char* buf, size_t plen)
{
    ethr_hdr(buf);
    ip_hdr(buf);
    fprintf(logdir , "\n###################################################\n");

    //Extracting IP Header
    struct iphdr *iph = (struct iphdr*)(buf + sizeof(struct ethhdr));
    switch (iph->protocol) {
        case IPPROTO_ICMP:  //1
            //TODO
            break;

        case IPPROTO_IGMP:  //2
            //TODO
            break;

        case IPPROTO_TCP:   //6
            //TODO
            break;

        case IPPROTO_UDP:   //17
            //TODO
            break;

        case IPPROTO_GRE:   //47
            //TODO
            break;

        case IPPROTO_ESP:   //50
            //TODO
            break;

        case IPPROTO_MPLS:  //137
            //TODO
            break;

        default:            //other protocols can be found in
                            //include/uapi/linux/in.h
            break;
    }
}

void ethr_hdr(unsigned char* buf)
{
    struct ethhdr *eth = (struct ethhdr *)buf;

    fprintf(logdir , "\n");
    fprintf(logdir , "Ethernet Header\n");
    fprintf(logdir , "   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
        eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
        eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(logdir , "   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
        eth->h_source[0], eth->h_source[1], eth->h_source[2],
        eth->h_source[3], eth->h_source[4], eth->h_source[5] );
    fprintf(logdir , "   |-Protocol            : %u\n",
        (unsigned short)eth->h_proto);
}

void ip_hdr(unsigned char* buf)
{
    struct iphdr *iph = (struct iphdr *)(buf  + sizeof(struct ethhdr) );

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = iph->saddr;

    memset(&dst, 0, sizeof(dst));
    dst.sin_addr.s_addr = iph->daddr;

    fprintf(logdir , "\n");
    fprintf(logdir , "IP Header\n");
    fprintf(logdir , "   |-IP Version        : %d\n",
        (unsigned int)iph->version);
    fprintf(logdir , "   |-IP Header Length  : %d DWORDS or %d Bytes\n",
        (unsigned int)iph->ihl, ((unsigned int)(iph->ihl))*4);
    fprintf(logdir , "   |-Type Of Service   : %d\n",
        (unsigned int)iph->tos);
    fprintf(logdir , "   |-IP Total Length   : %d  Bytes(Size of Packet)\n",
        ntohs(iph->tot_len));
    fprintf(logdir , "   |-Identification    : %d\n",
        ntohs(iph->id));
    fprintf(logdir , "   |-TTL      : %d\n",
        (unsigned int)iph->ttl);
    fprintf(logdir , "   |-Protocol : %d\n",
        (unsigned int)iph->protocol);
    fprintf(logdir , "   |-Checksum : %d\n",
        ntohs(iph->check));
    fprintf(logdir , "   |-Source IP        : %s\n",
        inet_ntoa(src.sin_addr));
    fprintf(logdir , "   |-Destination IP   : %s\n",
        inet_ntoa(dst.sin_addr));
}
