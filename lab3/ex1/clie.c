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

#define MAX_MESSEDGE_LENGHT    30

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        puts("Undefined command. Usage: clie [server IP] [server port]");
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
    hp = gethostbyname(argv[1]) ;
    memcpy(&serv_addr.sin_addr, hp -> h_addr, hp -> h_length);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if( connect(sock_client, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 )
    {
        perror("Connect failed"); 
        exit(EXIT_FAILURE);
    }
    puts("Client ready");
    while(1)
    {
        char buf[MAX_MESSEDGE_LENGHT];
        if (fgets(buf, MAX_MESSEDGE_LENGHT, stdin) == 0) 
        {
            break;  
        }
        if(send(sock_client, buf, strlen(buf), 0) == -1)
        {
            perror("Send error\n");
        }
    }
    close(sock_client);
    return 0;
}