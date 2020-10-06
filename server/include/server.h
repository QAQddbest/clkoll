//
// Created by oliverdd on 10/6/20.
//

#ifndef CLKOLL_SERVER_H
#define CLKOLL_SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <netinet/in.h>
#include <assert.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

void *handleCommand(int *);
bool setNonBlocking(int);

#endif //CLKOLL_SERVER_H
