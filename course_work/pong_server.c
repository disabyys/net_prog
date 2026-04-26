#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>


struct game_field
{
    int width;
    int height;
    int ball_pos_x, ball_pos_y;
    int ball_v_x, ball_v_y;
    // int racket1_y, racket2_y;
    // unsigned score1, score2;
}field;

#define AMOUNT_PLAYERS  2

int main()
{
    srand(time(NULL));
    int sock_server, sock_client;
    int lenght;
    struct sockaddr_in serv_addr;
    if((sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Create socket for server failed\n");
        exit(EXIT_FAILURE);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = 0;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    if((bind(sock_server, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) == -1)
    {
        perror("Binding is failed\n");
        exit(EXIT_FAILURE);
    }
    lenght = sizeof(serv_addr);
    if((getsockname(sock_server, (struct sockaddr*)&serv_addr, (socklen_t*)&lenght) == -1))
    {
        perror("Getsockname error\n");
        exit(EXIT_FAILURE);
    }
    printf("Server port: %d\n", htons(serv_addr.sin_port));
    listen(sock_server, AMOUNT_PLAYERS);
    init_field();
    pthread_t th;
    pthread_attr_t ta;
    pthread_attr_init(&ta);
    while(1)
    {
        if((sock_client = accept(sock_server, (struct sockaddr*)&serv_addr, (socklen_t*)&lenght)) == -1)
        {
            perror("Accept failed\n");
        }
        if((pthread_create(&th, &ta, (void*)handler, (void*)(intptr_t)sock_client)) != 0)
        {
            perror("pthread_create error\n");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void handler(void* arg)
{
    size_t len_field = sizeof(field);
    int sock_client = (int)(intptr_t)arg;
    //сервер оправляет поле и получает обновлённое поле от клиентов?
}

void init_field()
{
    field.width = 140;
    field.height = 14;
    field.ball_pos_x = field.width / 2;
    field.ball_pos_y = 1 + rand() % 14;
    field.ball_v_x = 1;
    field.ball_v_y = 1;
}