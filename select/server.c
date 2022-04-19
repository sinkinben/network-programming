#include "common.h"
#define NR_CLIENT (FD_SETSIZE) /* NR_LISTEN = 1024 */

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
int startup()
{
    int listenfd = -1;

    /* create listen socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);

    /* set non-blocking listenfd */
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);

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

typedef struct monitor
{
    fd_set fds;
    int maxfd;
    int connfds[NR_CLIENT];
} monitor_t;

void monitor_init(monitor_t *m)
{
    FD_ZERO(&m->fds);
    m->maxfd = -1;
    memset(m->connfds, -1, sizeof(int) * NR_CLIENT);
}

void monitor_add(monitor_t *m, int newfd)
{
    /* Find an empty position to put newfd in. Actually, we should add
     * newfd into a waiting-queue firstly. Once m->connfds is not full
     * pop a fd from waiting-queue and add it into monitor.
     */
    int i;
    for (i = 0; i < NR_CLIENT; ++i)
    {
        if (m->connfds[i] == -1)
        {
            m->connfds[i] = newfd;
            break;
        }
    }
    assert(i < NR_CLIENT);
    FD_SET(newfd, &m->fds);
    m->maxfd = max(m->maxfd, newfd);
}
void monitor_remove(monitor_t *monitor, int idx)
{
    /* remove connfds[i] from monitor */
    int fd = monitor->connfds[idx];
    monitor->connfds[idx] = -1;
    FD_CLR(fd, &monitor->fds);
    if (fd == monitor->maxfd)
        monitor->maxfd--;
}

void monitor_select(monitor_t *monitor, fd_set *readfds, fd_set *writefds, int listenfd)
{
    int connfd, fd, i, ret;
    task_t task;

    /* client address */
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);

    if (FD_ISSET(listenfd, readfds))
    {
        /* accept will not keep waiting, since we set O_NONBLOCK */
        connfd = accept(listenfd, (sockaddr_t *)(&caddr), &len);

        /* set connfd non-blocking when calling recv */
        fcntl(connfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK);

        /* add connfd into select monitor */
        monitor_add(monitor, connfd);
    }

    int *connfds = monitor->connfds;
    for (i = 0; i < NR_CLIENT; ++i)
    {
        fd = connfds[i];
        if (fd == listenfd || fd == -1)
            continue;
        if (FD_ISSET(fd, readfds))
        {
            ret = recv(fd, &task, sizeof(task_t), 0);
            if (ret != 0)
            {
                compute(&task);
                send(fd, &task, sizeof(task_t), 0);
            }
            else
            {
                /* ret == 0 means the client closed the connection */
                close(fd);
                monitor_remove(monitor, i);
            }
        }
        if (FD_ISSET(fd, writefds))
        {
            /* wirte some data if you want */
        }
    }
}

int main()
{
    /* get listen socket fd */
    int listenfd = startup();

    monitor_t monitor;
    monitor_init(&monitor);
    monitor_add(&monitor, listenfd);

    timeval_t timeval = {0, 0};

    /* single thread to monitor all fds */
    while (1)
    {
        fd_set readfds = monitor.fds, writefds = monitor.fds;
        int ret = select(monitor.maxfd + 1, &readfds, &writefds, NULL, &timeval);
        if (ret == -1)
        {
            /* report error and abort */
            assert(0);
        }
        else if (ret == 0)
        {
            /* timeout */
        }
        else
        {
            monitor_select(&monitor, &readfds, &writefds, listenfd);
        }
    }
}