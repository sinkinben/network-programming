#include "common.h"

int main()
{
    /* diagram oriented socket */
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    /* server address */
    struct sockaddr_in saddr;

    /* init saddr */
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SERVER_PORT_INT);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* bind sockfd and address */
    assert(bind(sockfd, (sockaddr_t *)(&saddr), sizeof(saddr)) != -1);

    /* client address */
    struct sockaddr_in caddr;
    socklen_t len = sizeof(struct sockaddr_in);

    /* compute task buffer */
    task_t task;

    /* IP string buffer */
    char ip[16];
    while (1)
    {
        /* set addr = addr_len = null, if we are not insterested in them */
        assert(recvfrom(sockfd, &task, sizeof(task_t), 0, (sockaddr_t *)(&caddr), &len) != -1);

        /* the address of client (source) */
        printf("client from [%s:%d] \n",
               inet_ntop(AF_INET, &(caddr.sin_addr.s_addr), ip, sizeof(ip)),
               ntohs(caddr.sin_port));
        compute(&task);
        assert(sendto(sockfd, &task, sizeof(task_t), 0, (sockaddr_t *)(&caddr), len) != -1);
    }

    close(sockfd);
}