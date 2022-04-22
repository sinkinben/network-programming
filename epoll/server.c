#include "common.h"
#define NR_CLIENT (1024) /* NR_CLIENT= 1024 */
#define NR_EVENT (16)

int init_listen()
{
    int listenfd = -1;

    /* create listen socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listenfd != -1);

    /* set non-blocking listenfd, calling 
     * socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0) has the same effect.
     */
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

void epoll_add(int epfd, int newfd)
{
    static epoll_event_t ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = newfd;
    assert(epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &ev) != -1);
}

int main()
{
    int listenfd = init_listen();
    epoll_event_t listen_event = {
        .data.fd = listenfd,
        .events = EPOLLIN};

    /* create epoll object */
    int epfd = epoll_create1(0);
    assert(epfd != -1);

    /* monitor listen_event on listenfd */
    assert(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &listen_event) != -1);

    /* client address */
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);

    int nfds, i, connfd, ret;
    epoll_event_t events[NR_EVENT], ev;
    task_t task;
    while (1)
    {
        nfds = epoll_wait(epfd, events, NR_EVENT, -1);
        for (i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listenfd)
            {
                if ((events[i].events & EPOLLIN) == 0)
                    continue;
                /* accept the connection request, and build the connection */
                connfd = accept(listenfd, (sockaddr_t *)(&caddr), &len);

                /* set non-blocking */
                fcntl(connfd, F_SETFL, fcntl(listenfd, F_GETFL, 0) | O_NONBLOCK);

                /* monitor EPOLLIN event on connfd */
                epoll_add(epfd, connfd);
            }
            else
            {
                if ((events[i].events & EPOLLIN) == 0)
                    continue;
                connfd = events[i].data.fd;
                ret = recv(connfd, &task, sizeof(task_t), 0);
                assert(ret >= 0);
                if (ret > 0)
                {
                    compute(&task);
                    send(connfd, &task, sizeof(task_t), 0);
                }
                else if (ret == 0)
                {
                    /* client close the connection, hence we should remove connfd from epoll */
                    assert(epoll_ctl(epfd, EPOLL_CTL_DEL, connfd, NULL) != -1);
                    close(connfd);
                }
            }
        }
    }
}