#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>

#define SLEEP_TIME  3

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        fprintf(stderr, "Usage: %s [server IP] [server port] [messege]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sock_client;
    struct sockaddr_in serv_addr;
    struct hostent *hp;
    if((sock_client = (socket(AF_INET, SOCK_STREAM, 0))) == -1)
    {
        perror("Create socket for client failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    if(!hp)
    {
        fprintf(stderr, "Unknown host: %s\n", argv[1]);
        close(sock_client);
        exit(EXIT_FAILURE);
    }
    memcpy(&serv_addr.sin_addr, hp -> h_addr, hp -> h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock_client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 )
    {
        perror("Connect failed"); 
        exit(EXIT_FAILURE);
    }
    puts("Client ready");
    int buf = atoi(argv[3]);
    while(1)
    {
        if(send(sock_client, &buf, sizeof(int), 0) == -1)
        {
            perror("Send error\n");
        }
        int result;
        ssize_t bytes = recv(sock_client, &result, sizeof(result), 0);
        if(bytes != sizeof(result))
        {
            if(bytes == 0)
            {
                printf("Server closed connection\n");
            } 
            else
            {
                perror("recv failed");
            }
            break;
        }
        printf("Server response: %d\n", result);
        sleep(atoi(argv[3]));
    }
    close(sock_client);
    return 0;
}