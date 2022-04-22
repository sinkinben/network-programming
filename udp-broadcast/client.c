#include "common.h"

int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    /* We need to bind the client with an board-cast port, the server 
     * will broadcast on this port in a local network.
     */
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    bzero(&caddr, sizeof(caddr));
    caddr.sin_family = AF_INET;
    caddr.sin_port = htons(BROADCAST_PORT_INT);

    /* set shared port (reused address), it will allow multiple clients
     * to listen on same port, waiting the broadcast message.
     */
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    assert(bind(sockfd, (sockaddr_t *)(&caddr), len) != -1);

    /* recv from server */
    char buf[32];
    struct sockaddr_in saddr;
    socklen_t slen = sizeof(saddr);

    /* recvfrom will block if data is not ready */
    assert(recvfrom(sockfd, buf, sizeof(buf), 0, (sockaddr_t *)(&saddr), &slen) != -1);

    char ip[16];
    printf("[pid = %d] recv '%s' from %s:%u \n", getpid(), buf,
           inet_ntop(AF_INET, &(saddr.sin_addr.s_addr), ip, sizeof(ip)),
           ntohs(saddr.sin_port));

    close(sockfd);
}