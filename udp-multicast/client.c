#include "common.h"
int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    /* bind client to a multicast port*/
    struct sockaddr_in caddr;
    bzero(&caddr, sizeof(caddr));
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(MULTICAST_PORT_INT);

    /* set shared port (reused address), it will allow multiple clients
     * to listen on same port, waiting the multicast message.
     */
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    assert(bind(sockfd, (sockaddr_t *)(&caddr), sizeof(caddr)) != -1);

    /* Calling `setsockopt` to add `sockfd` into a multicast group. 
     * Multicast is at IP-Level (the 3rd level) of TCP/IP reference model.
     * See `man 7 ip` to see its usage.
     */
    struct ip_mreqn group;
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP_STR);
    group.imr_address.s_addr = htonl(INADDR_ANY);
    setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));

    /* address of server */
    sockaddr_t saddr;
    socklen_t slen = sizeof(saddr);

    /* recv from server, if we don't care about address of server, 
     * then set addr = addr_len = NULL.
     */
    char buf[32];
    recvfrom(sockfd, buf, sizeof(buf), 0, &saddr, &slen);
    printf("[client %s] recv '%s' \n", MULTICAST_GROUP_STR, buf);
    close(sockfd);
}
