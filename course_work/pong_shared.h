#ifndef PONG_SHARED_H
#define PONG_SHARED_H

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define COLLS 150
#define ROWS 50
#define INDENT_VERT_WALLS 3
#define INDENT_HOR_WALLS 5
#define OFFSET_RACKETS 20

#define MAX_SCORE 10

enum keys
{
  KEY_UP,
  KEY_DOWN,
  KEY_ESC,
  SILENCE
};

struct position_objects_t
{

  int ball_pos_x, ball_pos_y;
  int ball_v_x, ball_v_y;
  int racketx[2];
  int rackety[2];
};

struct game_state
{
  struct position_objects_t pos_obj;
  unsigned score1, score2;
};

#endif