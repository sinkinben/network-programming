#include "common.h"
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    /* init addr */
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP_STR);
    addr.sin_port = htons(SERVER_PORT_INT);

    assert(connect(sockfd, (sockaddr_t *)(&addr), sizeof(struct sockaddr_in)) != -1);

    /* send task to server and recv result from it */
    task_t task = {
        .op = ADD,
        .lval = 10,
        .rval = 20};
    for (int i = 0; i < 10; ++i)
    {
        task.result = 0;
        send(sockfd, &task, sizeof(task_t), 0);
        recv(sockfd, &task, sizeof(task_t), 0);
        printf("result = %d \n", task.result);
        assert(task.result == 30);
    }
    close(sockfd);
}
