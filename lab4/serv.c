#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS   5

int main()
{
    int sock_serv, sock_clie, lenght;
    int buf;
    struct sockaddr_in serv_addr;
    if((sock_serv = (socket(AF_INET, SOCK_STREAM, 0))) == -1)
    {
        perror("Create socket for server failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = 0;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if((bind(sock_serv, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1)
    {
        perror("bind error\n");
        exit(EXIT_FAILURE);
    }
    lenght = sizeof(serv_addr);
    if((getsockname(sock_serv, (struct sockaddr*)&serv_addr, (socklen_t*)&lenght) == -1))
    {
        perror("getsockname error\n");
        exit(EXIT_FAILURE);
    }
    printf("Server port: %d\n", htons(serv_addr.sin_port));
    listen(sock_serv, MAX_CLIENTS);
    int max_nfds = sock_serv;
    fd_set rfds, afds;
    FD_ZERO(&afds);
    FD_SET(sock_serv, &afds);
    while(1)
    {
        memcpy(&rfds, &afds, sizeof(rfds));
        if(select(max_nfds + 1, &rfds, (fd_set*)0, (fd_set*)0, (struct timeval *)0) < 0)
        {
            perror("Select failed\n");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(sock_serv, &rfds))
        {
            struct sockaddr_in clie_addr;  // Отдельный адрес для клиента
            socklen_t clie_len = sizeof(clie_addr);
            sock_clie = accept(sock_serv, (struct sockaddr*)&clie_addr, &clie_len);
            if(sock_clie < 0)
            {
                perror("Accept failed\n");
                continue;
            }
            FD_SET(sock_clie, &afds);
            if(sock_clie > max_nfds)
            {
                max_nfds = sock_clie;
            }
            printf("New client connected, fd=%d\n", sock_clie);
        }
        for(int fd=0; fd<=max_nfds; fd++)
        {
            if(fd != sock_serv && FD_ISSET(fd, &rfds))
            {
                int read_bytes = read(fd, &buf, sizeof(int));
                if(read_bytes <= 0)
                {
                    perror("Read failed\n");
                    close(fd);
                    FD_CLR(fd, &afds);
                    if (fd == max_nfds) 
                    {
                        while (max_nfds > 0 && !FD_ISSET(max_nfds, &afds)) 
                        {
                            max_nfds--;
                        }
                    }
                    continue;
                }
                printf("Recived messedge: %d\n", buf);
                int processed_msg = buf * buf;
                if(send(fd, &processed_msg, sizeof(int), 0) < 0)
                {
                    perror("Send failde\n");
                }
            }
        }
    }
    return 0;
}