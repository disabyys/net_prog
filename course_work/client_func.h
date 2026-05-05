#ifndef CLIENT_FUNC_H
#define CLIENT_FUNC_H

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <termios.h>
#include <stdbool.h>

#include <pong_shared.h>

static struct termios orig_term;

void configure_client(int* sock_client, struct sockaddr_in* serv_addr, struct hostent** hp, char* host_name, char* port);
void draw_box(int left_pos_field, int upper_pos_field, int right_pos_field,
              int lower_pos_field);
int setcursorvisible(int value);
int read_key(enum keys *key);
void configure_terminal();
void gotoXY(int x, int y);
void draw_ball(int ball_pos_x, int ball_pos_y);
void draw_racket(int racket_pos_x, int racket_pos_y);
void clear_cell(int pos_x, int pos_y);
void clear_racket(int racket_pos_x, int racket_pos_y);

#endif