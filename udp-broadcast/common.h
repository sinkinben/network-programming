#ifndef __UNP_H
#define __UNP_H
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define BROADCAST_PORT_INT 9877
#define BROADCAST_PORT_STR "9877"
#define BROADCAST_MASK_STR "192.168.31.255" /* Get via `ifconfig` command */

typedef struct sockaddr sockaddr_t;

#endif