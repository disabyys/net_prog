#include <client_func.h>

void draw_box(int left_pos_field, int upper_pos_field, int right_pos_field,
              int lower_pos_field)
{
    gotoXY(left_pos_field, upper_pos_field);
    write(STDOUT_FILENO, "l", 1);
    for (int i = left_pos_field + 1; i < right_pos_field; i++)
    {
        gotoXY(i, upper_pos_field);
        write(STDOUT_FILENO, "q", 1);
    }
    gotoXY(right_pos_field, upper_pos_field);
    write(STDOUT_FILENO, "k", 2);
    gotoXY(left_pos_field, upper_pos_field + 1);
    for (int i = upper_pos_field + 1; i < lower_pos_field; i++)
    {
        write(STDOUT_FILENO, "x", 1);
        for (int j = left_pos_field + 1; j < right_pos_field; j++)
        {
            gotoXY(j, i);
            write(STDOUT_FILENO, " ", 1);
        }
        gotoXY(right_pos_field, i);
        write(STDOUT_FILENO, "x", 1);
        gotoXY(left_pos_field, i);
    }
    gotoXY(left_pos_field, lower_pos_field);
    write(STDOUT_FILENO, "m", 1);
    for (int i = left_pos_field + 1; i < right_pos_field; i++)
    {
        gotoXY(i, lower_pos_field);
        write(STDOUT_FILENO, "q", 1);
    }
    gotoXY(right_pos_field, lower_pos_field);
    write(STDOUT_FILENO, "j", 1);
    write(STDOUT_FILENO, "\n", 1);
}

int setcursorvisible(int value)
{
    if (value == 0)
    {
        write(STDOUT_FILENO, "\033[?25l\033[?1c", strlen("\033[?25l\033[?1c"));
    }
    else if (value == 1)
    {
        write(STDOUT_FILENO, "\033[?25h\033[?8c", strlen("\033[?25h\033[?8c"));
    }
    else
    {
        fprintf(stderr, "This cursor mode is not allowed.");
        return -1;
    }
    return 0;
}

int read_key(enum keys *key)
{
    char input;
    ssize_t n = read(STDIN_FILENO, &input, 1);
    if (n == -1)
        return -1;
    if (n == 0) // timeout VTIME
    {
        *key = SILENCE;
        return 0;
    }
    if (input == 'w' || input == 'W')
    {
        *key = KEY_UP;
        return 0;
    }
    else if (input == 's' || input == 'S')
    {
        *key = KEY_DOWN;
        return 0;
    }
    else if (input == 0x1B) // ESC
    {
        *key = KEY_ESC;
        return 0;
    }
    else
    {
        *key = SILENCE;
        return 0;
    }
}

void configure_client(int *sock_client, struct sockaddr_in *serv_addr, struct hostent **hp, char *host_name, char *port)
{
    if ((*sock_client = (socket(AF_INET, SOCK_STREAM, 0))) == -1)
    {
        perror("Create socket for client failed\n");
        exit(EXIT_FAILURE);
    }
    memset(serv_addr, 0, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    *hp = gethostbyname(host_name);
    memcpy(&serv_addr->sin_addr, (*hp)->h_addr_list[0], (*hp)->h_length);
    serv_addr->sin_port = htons(atoi(port));
    if (connect(*sock_client, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0)
    {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }
}

void configure_terminal()
{
    write(STDOUT_FILENO, "\e(0", strlen("\e(0"));
    struct termios new_term;
    tcgetattr(STDIN_FILENO, &orig_term);
    new_term = orig_term;
    new_term.c_lflag &= ~(ECHO | ICANON);
    new_term.c_cc[VMIN] = 0;
    new_term.c_cc[VTIME] = 2;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
}

void gotoXY(int x, int y)
{
    char buf[10];
    sprintf(buf, "\033[%d;%dH", y, x);
    write(STDOUT_FILENO, buf, strlen(buf));
}

void draw_ball(int ball_pos_x, int ball_pos_y)
{
    gotoXY(ball_pos_x, ball_pos_y);
    write(STDOUT_FILENO, "\033[31m", 5);
    write(STDOUT_FILENO, "*", 1);
    write(STDOUT_FILENO, "\033[30m", 5);
}

void draw_racket(int racket_pos_x, int racket_pos_y)
{
    for (int i = 0; i < 3; i++)
    {
        gotoXY(racket_pos_x, racket_pos_y - 1 + i);
        write(STDOUT_FILENO, "#", 1);
    }
}

void clear_cell(int pos_x, int pos_y)
{
    gotoXY(pos_x, pos_y);
    write(STDOUT_FILENO, " ", 1);
}

void clear_racket(int racket_pos_x, int racket_pos_y)
{
    for (int i = 0; i < 3; i++)
    {
        clear_cell(racket_pos_x, racket_pos_y - 1 + i);
    }
}