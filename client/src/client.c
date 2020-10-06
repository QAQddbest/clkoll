//
// Created by oliverdd on 10/6/20.
//

#include "client.h"

#define PORT 8888
#define BUF_SIZE 1024

int main() {
    int rotfd = socket(PF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = PF_INET;
    remote_addr.sin_port = htons(PORT);
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    socklen_t remote_len = sizeof(remote_addr);

    char buffer[BUF_SIZE];
    while (true) {
        fgets(buffer, BUF_SIZE, stdin); // input command
        printf("%s", buffer);
        if (strncmp(buffer, "exit", 4) == 0) { // "exit" to exit
            break;
        } else { // delete '\n' in the end
            buffer[strlen(buffer) - 1] = '\0';
        }
        int ret = sendto(rotfd, buffer, strlen(buffer), 0, (struct sockaddr *) &remote_addr, remote_len);
        if (ret < 0) {
            perror("Error when sending");
        }
        ret = recvfrom(rotfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &remote_addr, &remote_len);
        if (ret > 0) {
            buffer[ret] = '\0';
            printf("%s\n", buffer);
        }
    }
    close(rotfd);
    return 0;
}

