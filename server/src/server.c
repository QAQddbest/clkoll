//
// Created by oliverdd on 10/6/20.
//

#include "server.h"

extern int errno;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define PORT 8888
#define MAX_EPOLL_SIZE 100
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int listener; // Listener socket: server as event listener for epoll
    if ((listener = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        pthread_mutex_lock(&mutex);
        perror("socket");
        pthread_mutex_unlock(&mutex);
        return errno;
    }

    // Set the option of the port to be reusable
    int opt = SO_REUSEADDR;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (setNonBlocking(listener) == false) {
        pthread_mutex_lock(&mutex);
        perror("non-blocking");
        pthread_mutex_unlock(&mutex);
        return errno;
    }

    // Configure local address
    struct sockaddr_in local_addr;
    bzero(&local_addr, sizeof(local_addr));
    local_addr.sin_family = PF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind sockfd to the local address
    if (bind(listener, (struct sockaddr *) &local_addr, sizeof(struct sockaddr)) == -1) {
        pthread_mutex_lock(&mutex);
        perror("bind");
        pthread_mutex_unlock(&mutex);
        return errno;
    }

    // Register epoll event
    struct epoll_event ev;
    struct epoll_event events[MAX_EPOLL_SIZE];
    int kdpfd = epoll_create(MAX_EPOLL_SIZE);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener;

    if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
        pthread_mutex_lock(&mutex);
        perror("epoll ctl");
        pthread_mutex_unlock(&mutex);
        return errno;
    }
    int count;
    while (true) {
        // Wait for event
        count = epoll_wait(kdpfd, events, 10000, -1);
        if (count == -1) {
            pthread_mutex_lock(&mutex);
            perror("epoll wait");
            pthread_mutex_unlock(&mutex);
            break;
        }
        // **Here I get a better idea: use glib to use thread pool.**
        for (int i = 0; i < count; ++i) { // Loop all the events
            pthread_t id; // Everytime a message comes, the server will create a thread to handle it.
            pthread_create(&id, NULL, (void *) handleCommand, (void *) &(events[i].data.fd));
        }
    }
    close(listener);
    pthread_mutex_destroy(&mutex);
    return 0;
}

/**
 * Subthread to handle command.
 * @param sockfd remote sockfd
 * @return NULL
 */
void *handleCommand(int *sockfd) {
    char buffer[BUF_SIZE];
    bzero(buffer, BUF_SIZE);
    struct sockaddr_in remote_addr;
    socklen_t remote_len = sizeof(remote_addr);
    int rotfd = *sockfd;
    int ret = recvfrom(rotfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &remote_addr, &remote_len);
    if (ret > 0) {
        pthread_mutex_lock(&mutex);
        printf("[%d] received from [%s:%d]: \"%s\"(%d)\n",
               rotfd, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port), buffer, ret);
        pthread_mutex_unlock(&mutex);
        time_t nSeconds;
        struct tm *pTm;
        time(&nSeconds);
        pTm = localtime(&nSeconds);
        if (ret == 8 && strncmp(buffer, "get date", 8) == 0) {
            snprintf(buffer, BUF_SIZE, "%d.%d.%d", 1900 + pTm->tm_year, pTm->tm_mon + 1, pTm->tm_mday);
        } else if (ret == 8 && strncmp(buffer, "get time", 8) == 0) {
            snprintf(buffer, BUF_SIZE, "%d:%d:%d", pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
        } else if (ret == 12 && strncmp(buffer, "get datetime", 12) == 0) {
            snprintf(buffer, BUF_SIZE, "%d.%d.%d %d:%d:%d", 1900 + pTm->tm_year, pTm->tm_mon + 1, pTm->tm_mday,
                     pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
        } else {
            snprintf(buffer, BUF_SIZE, "Command Error!");
        }
        ret = sendto(rotfd, buffer, strlen(buffer), 0, (struct sockaddr *) &remote_addr, remote_len);
        if (ret < 0) {
            pthread_mutex_lock(&mutex);
            perror("send");
            pthread_mutex_unlock(&mutex);
        }
    } else {
        pthread_mutex_lock(&mutex);
        perror("receive");
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
    fflush(stdout);
    pthread_exit(NULL);
}

/**
 * Set file handle to be non-blocking
 * @param sockfd
 * @return true when succeed and false in the other wise.
 */
bool setNonBlocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return false;
    }
    return true;
}
