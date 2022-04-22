#ifndef __UNP_H
#define __UNP_H
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define MULTICAST_PORT_INT 9877
#define MULTICAST_PORT_STR "9877"

/* The range of multicast IP is from 224.0.0.0 to 239.255.255.255. */
#define MULTICAST_GROUP_STR "224.0.0.1"

typedef struct sockaddr sockaddr_t;

#endif