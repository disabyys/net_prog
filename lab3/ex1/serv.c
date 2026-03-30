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

#define MAX_CLIENTS            5
#define FILE_NAME              "scroll.txt"
#define MAX_MESSEDGE_LENGHT    30

pthread_mutex_t st_mutex;
FILE* text_file;

void* handler(void* arg);

int main()
{
    int sock_serv, sock_client, lenght;
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
    text_file = fopen(FILE_NAME, "w");
    pthread_t th;
    pthread_attr_t ta;
    pthread_attr_init(&ta);
    pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&st_mutex,0);
    while(1)
    {
        if((sock_client = accept(sock_serv, (struct sockaddr*)&serv_addr, (socklen_t*)&lenght)) == -1)
        {
            perror("accept failed\n");
            if(sock_client == -1) 
            {
                if(errno == EINTR) continue; 
                if(errno == EAGAIN) continue; 
                continue;
            }
        }
        if((pthread_create(&th, &ta, (void*)handler, (void*)(intptr_t)sock_client)) != 0)
        {
            perror("pthread_create error\n");
            exit(EXIT_FAILURE);
        }

    }
    pthread_attr_destroy(&ta);
    pthread_mutex_destroy(&st_mutex);
    close(sock_serv);
    fclose(text_file);
    return 0;
}

void* handler(void* arg)
{
    char buf[MAX_MESSEDGE_LENGHT];
    int sock_client = (int)(intptr_t)arg;
    while(1)
    {
        if(recv(sock_client, buf, MAX_MESSEDGE_LENGHT, 0) <= 0)
        {
            puts("Client disconnected");
            break;
        }
        int pos = strcspn(buf, "\n");
        buf[pos] = '\0';
        printf("Messedge '%s' recieve\n", buf);
        pthread_mutex_lock(&st_mutex);
        fprintf(text_file, "%s\n", buf);
        fflush(text_file);
        pthread_mutex_unlock(&st_mutex);
    }   
    close(sock_client);
    return 0;
}