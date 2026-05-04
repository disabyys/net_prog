#include <sys/time.h>
#include <stdbool.h>
#include <server_func.h>

void configure_server(struct sockaddr_in* serv_addr, int* sock_server, int* lenght);

int main()
{
    srand(time(NULL));
    int sock_server, sock_client;
    int lenght;
    struct sockaddr_in serv_addr;
    configure_server(&serv_addr, &sock_server, &lenght);
    struct game_state state;
    init_field(&state, 0, 0);
    struct client clients[AMOUNT_PLAYERS];
    for(int i=0; i<AMOUNT_PLAYERS; i++)
    {
        while(1)
        {
            if((sock_client = accept(sock_server, (struct sockaddr*)&serv_addr, (socklen_t*)&lenght)) == -1)
            {
                if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    continue;
                }
                else
                {
                    perror("Accept failed\n");
                    exit(EXIT_FAILURE);
                }
            }
            clients[i].is_alive = true;
            clients[i].player_id = i+1;
            clients[i].socket = sock_client;
            break;
        }
    }
    while(1)
    {
        if(!clients[0].is_alive || !clients[1].is_alive)
        {
            perror("One or more clients is disconnected");
            close(clients[0].socket);
            close(clients[1].socket);
            close(sock_server);
            exit(EXIT_FAILURE);
        }
        for(int i=0; i<AMOUNT_PLAYERS; i++)
        {
            if(send(clients[i].socket, &state.pos_obj, sizeof(state.pos_obj), (intptr_t)NULL) == -1)
            {
                clients[i].is_alive = false;
                continue;
            }
        }
        for(int i=0; i<AMOUNT_PLAYERS; i++)
        {
            enum keys key;
            if(recv(clients[i].socket, &key, sizeof(key), (intptr_t)NULL) == -1)
            {  
                perror("Select error\n");
                clients[i].is_alive = false;
                continue;
            }
            switch (key)
            {
            case KEY_UP:
                if(state.pos_obj.rackety[i] > 1 && state.pos_obj.rackety[i] <= ROWS - 1)
                {
                    state.pos_obj.rackety[i]++;
                }
                break;
            case KEY_DOWN:
                if(state.pos_obj.rackety[i] >= 1 && state.pos_obj.rackety[i] < ROWS - 1)
                {
                    state.pos_obj.rackety[i]--;
                }
                break;
            case KEY_ESC:
                clients[i].is_alive = false;
                continue;
            case SILENCE:
                break;
            }
        }
        enum collisions collision = check_wall_collision(state);
        update_ball_position(collision, &state);
        if(state.score1 == MAX_SCORE || state.score2 == MAX_SCORE)
        {
            printf("Player %d won!\n", (state.score1 == MAX_SCORE) ? 1 : 2);
            close(clients[0].socket);
            close(clients[1].socket);
            close(sock_server);
            exit(0);
        }
    }
    return 0;
}
