#include "common.h"
#define NR_CLIENT (1024) /* NR_LISTEN = 1024 */

int init_listen()
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

void poll_init(pollfd_t *pollfds, int nfds)
{
    for (int i = 0; i < nfds; ++i)
    {
        pollfds[i].fd = -1;
        pollfds[i].events = pollfds[i].revents = 0;
    }
}
void poll_add(pollfd_t *pollfds, int nfds, int newfd)
{
    /* Find an empty position to put newfd in. */
    int i;
    for (i = 0; i < nfds; ++i)
    {
        if (pollfds[i].fd == -1)
        {
            pollfds[i].fd = newfd;
            pollfds[i].events = POLLIN | POLLOUT;
            pollfds[i].revents = 0;
            break;
        }
    }
    assert(i < nfds);
}

void poll_all(pollfd_t *pollfds, int nfds, int listenfd)
{
    int i, fd, connfd, revent, ret;
    task_t task;

    /* client address */
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);

    for (i = 0; i < nfds; ++i)
    {
        fd = pollfds[i].fd, revent = pollfds[i].revents;
        if (fd == listenfd)
        {
            if ((POLLIN & revent) == 0)
                continue;

            /* accept the connection request, and build the connection */
            connfd = accept(listenfd, (sockaddr_t *)(&caddr), &len);

            /* set non-blocking */
            fcntl(connfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK);

            /* add connfd into poll monitor */
            poll_add(pollfds, nfds, connfd);
        }
        else
        {
            if (revent & POLLIN)
            {
                pollfds[i].revents = 0;
                ret = recv(fd, &task, sizeof(task_t), 0);
                assert(ret >= 0);
                if (ret > 0)
                {
                    compute(&task);
                    send(fd, &task, sizeof(task_t), 0);
                }
                else
                {
                    close(fd);
                    pollfds[i].fd = -1;
                }
            }
            if (revent & POLLOUT)
            {
                /* wirte some data to fd here if you want */
            }
        }
    }
}

int main()
{
    /* get listen socket fd */
    int listenfd = init_listen();

    /* init poll */
    pollfd_t pollfds[NR_CLIENT];
    poll_init(pollfds, NR_CLIENT);

    /* monitor listenfd */
    poll_add(pollfds, NR_CLIENT, listenfd);

    /* single thread to monitor all fds */
    while (1)
    {
        /* wait unitl the valid events arrive. */
        int ret = poll(pollfds, NR_CLIENT, -1);
        if (ret == -1)
        {
            /* report error */
            assert(0);
        }
        else if (ret == 0)
        {
            /* timeout */
        }
        else
        {
            poll_all(pollfds, NR_CLIENT, listenfd);
        }
    }
}