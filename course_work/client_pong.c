#include <client_func.h>

struct position_objects_t pos;

void* draw_handler(void* arg);
void* network_handler(void* arg);

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
    configure_client(&sock_client, &serv_addr, &hp, argv[1], argv[2]);
    if(getenv("IN_XTERM") == NULL)
    {
        printf("Launching xterm...\n");  
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "xterm -ti vt100 -font fixed -geometry %dx%d -e env IN_XTERM=1 %s %s %s", COLLS, ROWS, argv[0], argv[1], argv[2]);
        system(cmd);
        return 0;
    }
    configure_terminal();
    setcursorvisible(0);
    pthread_t draw_thread, network_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&network_thread, &attr, (void*)network_handler, (void*)(intptr_t)sock_client);
    pthread_create(&draw_thread, &attr, (void*)draw_handler,  (void*)(intptr_t)sock_client);
    pthread_join(network_thread, NULL);
    pthread_join(draw_thread, NULL);
    write(STDOUT_FILENO, "\e(B", strlen("\e(B"));
    setcursorvisible(1);
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
    pthread_attr_destroy(&attr);
    return 0;
}

void* draw_handler(void* arg)
{
    int sock_client = (int)(intptr_t)arg;
     struct winsize s;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &s);
    int left_pos_field = INDENT_HOR_WALLS;
    int right_pos_field = s.ws_col - INDENT_HOR_WALLS;
    int lower_pos_field = s.ws_row;
    int upper_pos_field = 0;
    struct position_objects_t pos;
    while(1)
    {
        if(recv(sock_client, &pos, sizeof(pos), (intptr_t)NULL) == -1)
        {
            close(sock_client);
            pthread_exit(NULL);
        }
        char buf[20];
        sprintf(buf, "\033[2J\033[H");
        write(STDOUT_FILENO, buf, strlen(buf));
        draw_box(left_pos_field, upper_pos_field, right_pos_field, lower_pos_field);
        draw_ball(pos.ball_pos_x, pos.ball_pos_y);
        draw_racket(pos.racketx[0], pos.rackety[0]);
        draw_racket(pos.racketx[1], pos.rackety[1]);
        fflush(stdout);
    }
}

void* network_handler(void* arg)
{
    int sock_client = (int)(intptr_t)arg;
    while(1)
    {
        enum keys key;
        if(read_key(&key) == -1)
        {
            close(sock_client);
            pthread_exit(NULL);
        }
        if(send(sock_client, &key, sizeof(key), (intptr_t)NULL) == -1)
        {
            close(sock_client);
            pthread_exit(NULL);
        }
        if(key == KEY_ESC)
        {
            close(sock_client);
            pthread_exit(NULL);
        }
    }
}

