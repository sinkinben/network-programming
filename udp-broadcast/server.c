#include "common.h"
int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    /* Set or get the broadcast flag. When enabled, datagram sockets are allowed
     * to send packets to a broadcast address. This option has no effect on 
     * stream-oriented sockets. See `man 7 socket`.
     */
    int optval = 1;
    assert(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) != -1);

    /* create a board-cast address */
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT_INT);
    addr.sin_addr.s_addr = inet_addr(BROADCAST_MASK_STR);

    /* broadcast msg to all clients binding to the BROADCAST_PORT */
    char msg[] = "hello";
    assert(sendto(sockfd, msg, sizeof(msg), 0, (sockaddr_t *)(&addr), sizeof(addr)) != -1);

    close(sockfd);
}