#include "common.h"
#define NR_CLIENT (FD_SETSIZE) /* NR_LISTEN = 1024 */

int connfds[NR_CLIENT];

void compute(task_t *p)
{
    if (p->op == ADD)
        p->result = p->lval + p->rval;
    if (p->op == SUB)
        p->result = p->lval - p->rval;
    if (p->op == MUL)
        p->result = p->lval * p->rval;
    if (p->op == DIV)
        p->result = p->lval / p->rval;
}

void *poll_woker(void *args)
{
    task_t task;
    int ret = 0;
    for (int i = 0; i < NR_CLIENT; i = (i + 1) % NR_CLIENT)
    {
        if (connfds[i] == -1)
            continue;
        /* recv will block current thread if data is not ready */
        ret = recv(connfds[i], &task, sizeof(task_t), 0);
        assert(ret >= 0);
        if (ret == 0)
        {
            close(connfds[i]);
            connfds[i] = -1;
        }
        else
        {
            compute(&task);
            send(connfds[i], &task, sizeof(task_t), 0);
        }
    }

    return NULL;
}
int main()
{
    memset(connfds, -1, sizeof(int) * NR_CLIENT);

    int listenfd = -1, connfd = -1;

    /* create listen socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);

    /* client address and server address */
    struct sockaddr_in caddr, saddr;

    /* init saddr */
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(SERVER_PORT_INT);

    /* bind listedfd and saddr */
    assert(bind(listenfd, (sockaddr_t *)(&saddr), sizeof(saddr)) != -1);
    assert(listen(listenfd, NR_CLIENT) != -1);

    /* create a thread to poll events from clients */
    pthread_t poll_thread;
    pthread_create(&poll_thread, NULL, poll_woker, NULL);
    pthread_detach(poll_thread);

    /* listen the connection requests */
    socklen_t len = sizeof(caddr);
    while (1)
    {
        connfd = accept(listenfd, (sockaddr_t *)(&caddr), &len);
        /* add connfd into connfds if connfds is not full */
        for (int i = 0; i < NR_CLIENT; i = (i + 1) % NR_CLIENT)
        {
            /* FIXME: we should use a lock to protect connfds */
            if (connfds[i] == -1)
            {
                connfds[i] = connfd;
                break;
            }
        }
    }
}