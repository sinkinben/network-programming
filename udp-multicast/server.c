#include "common.h"

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    /* multicast group address */
    struct in_addr optval;
    optval.s_addr = inet_addr(MULTICAST_GROUP_STR);

    /* Set `sockfd` as a mutlicast socket, see `man 7 ip` to see its usage. */
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &optval, sizeof(optval));

    /* send msg to group */
    struct sockaddr_in group;
    group.sin_family = AF_INET;
    group.sin_port = htons(MULTICAST_PORT_INT);
    group.sin_addr.s_addr = optval.s_addr;

    char msg[] = "hello, multicast";
    sendto(sockfd, msg, sizeof(msg), 0, (sockaddr_t *)(&group), sizeof(group));

    close(sockfd);
}