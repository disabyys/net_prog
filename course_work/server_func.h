#ifndef SERVER_FUNC_H
#define SERVER_FUNC_H

#include <pong_shared.h>
#include <stdbool.h>

#define AMOUNT_PLAYERS 2

struct client {
  int socket;
  int player_id;
  bool is_alive;
};

enum collisions {
  PLAYER_1_HIT,
  PLAYER_2_HIT,
  UPPER_WALL,
  BOTTOM_WALL,
  RACKET_1_CENTRE,
  RACKET_1_UPPER,
  RACKET_1_BOTTOM,
  RACKET_2_CENTRE,
  RACKET_2_UPPER,
  RACKET_2_BOTTOM,
  NO_COLLISION
};

void init_field(struct game_state *state, int score1, int score2);
enum collisions check_wall_collision(struct game_state state);
void update_ball_position(enum collisions ball_col, struct game_state *state);

#endif