#ifndef __UNP_H
#define __UNP_H
#include "task.h"
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
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define SERVER_PORT_INT 9877
#define SERVER_PORT_STR "9877"
#define SERVER_IP_STR "127.0.0.1"

typedef struct sockaddr sockaddr_t;

#endif