#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void draw_box(int left_pos_field, int upper_pos_field, int right_pos_field, int lower_pos_field);
int gotoXY(int x, int y);

#define COLLS   200
#define ROWS    50

int main(int argc, char* argv[])
{
    if(getenv("IN_XTERM") == NULL)
    {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "xterm -ti vt100 -font fixed -geometry %dx%d -e env IN_XTERM=1 %s", COLLS, ROWS, argv[0]);
        system(cmd);
        return 0;
    }
    struct winsize s;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &s);
    int left_pos_field = 15;
    int right_pos_field = s.ws_col - 15;
    int lower_pos_field = s.ws_row;
    int upper_pos_field = 3;
    while(1)
    {
        draw_box(left_pos_field, upper_pos_field, right_pos_field, lower_pos_field);
    }
    
    return 0;
}

int gotoXY(int x, int y)
{
    if (x < 1 || y < 1)
    {
      fprintf (stderr, "This position for printig is not exist.\n");
      return -1;
    }
    char buffer[30];
    int length = sprintf (buffer, "\033[%d;%dH", y, x);

    write(STDOUT_FILENO, buffer, length);
    return 0;
}

void draw_box(int left_pos_field, int upper_pos_field, int right_pos_field, int lower_pos_field)
{
    write(STDOUT_FILENO, "\e(0", strlen("\e(0"));
    write(STDOUT_FILENO, "\e[2J\e[H", 6);
    gotoXY(left_pos_field, upper_pos_field);
    write(STDOUT_FILENO, "l", 1);
    for(int i=left_pos_field+1; i<right_pos_field; i++)
    {
        gotoXY(i, upper_pos_field);
        write(STDOUT_FILENO, "q", 1);
    }
    gotoXY(right_pos_field, upper_pos_field);
    write(STDOUT_FILENO, "k", 2);
    gotoXY(left_pos_field, upper_pos_field+1);
    for(int i=upper_pos_field+1; i<lower_pos_field; i++)
    {
        write(STDOUT_FILENO, "x", 1);
        for(int j=left_pos_field+1; j<right_pos_field; j++)
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
    for(int i=left_pos_field+1; i<right_pos_field; i++)
    {
        gotoXY(i, lower_pos_field);
        write(STDOUT_FILENO, "q", 1);
    }
    gotoXY(right_pos_field, lower_pos_field);
    write(STDOUT_FILENO, "j", 1);
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "\e(B", strlen ("\e(B"));
}