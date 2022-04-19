#include "common.h"
#define NR_CLIENT (16) /* Max number of threads for clients. */

int init_listen()
{
    int listenfd = -1;

    /* create listen socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);

    /* server address */
    struct sockaddr_in saddr;

    /* init saddr */
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(SERVER_PORT_INT);

    /* bind listedfd and saddr */
    assert(bind(listenfd, (sockaddr_t *)(&saddr), sizeof(saddr)) != -1);
    assert(listen(listenfd, NR_CLIENT) != -1);

    return listenfd;
}

void *client_hanlder(void *args)
{
    /* detach current thread */
    pthread_detach(pthread_self());

    int fd = (int)(args), ret = 0;
    task_t task;
    while (1)
    {
        ret = recv(fd, &task, sizeof(task_t), 0);
        assert(ret >= 0);
        if (ret > 0)
        {
            compute(&task);
            send(fd, &task, sizeof(task_t), 0);
        }
        else if (ret == 0)
            break;
    }
    close(fd);
    return NULL;
}

int main()
{
    int listenfd = init_listen();
    int connfd = -1;
    pthread_t th;

    /* client address */
    sockaddr_t caddr;
    socklen_t len = sizeof(caddr);

    /* listen for the connection from clients */
    while (1)
    {
        /* listenfd will block until accept return a valid fd */
        connfd = accept(listenfd, &caddr, &len);
        assert(connfd != -1);

        /* create one thread for the client */
        assert(pthread_create(&th, NULL, client_hanlder, (void *)(connfd)) == 0);
    }
    close(listenfd);
}