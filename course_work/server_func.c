#include <server_func.h>

void init_field(struct game_state* state, int score1, int score2)
{
    int ball_v_x = 1 + rand() % ROWS;
    int temp = rand() % 3;
    int ball_v_y = (temp == 0) ? -2 : ((temp == 1) ? 0 : 2);
    state->pos_obj.ball_pos_x = COLLS / 2;
    state->pos_obj.ball_pos_y = ball_v_x;
    state->pos_obj.ball_v_x = ball_v_y;
    state->pos_obj.ball_v_y = -1 + rand() % 3;
    state->pos_obj.racketx[0] = INDENT_HOR_WALLS + OFFSET_RACKETS;
    state->pos_obj.racketx[1] = COLLS - INDENT_HOR_WALLS - OFFSET_RACKETS;
    state->pos_obj.rackety[0] = state->pos_obj.rackety[1] = ROWS / 2;
    state->score1 = score1;
    state->score2 = score2;
}

void configure_server(struct sockaddr_in* serv_addr, int* sock_server, int* lenght)
{
    if((*sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Create socket for server failed\n");
        exit(EXIT_FAILURE);
    }
    memset(serv_addr, 0, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(8080);
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    if((bind(*sock_server, (struct sockaddr*)serv_addr, sizeof(*serv_addr))) == -1)
    {
        perror("Binding is failed\n");
        exit(EXIT_FAILURE);
    }
    *lenght = sizeof(*serv_addr);
    if((getsockname(*sock_server, (struct sockaddr*)serv_addr, (socklen_t*)lenght) == -1))
    {
        perror("Getsockname error\n");
        exit(EXIT_FAILURE);
    }
    printf("Server port: %d\n", htons(serv_addr->sin_port));
    listen(*sock_server, AMOUNT_PLAYERS);
}

enum collisions check_wall_collision(struct game_state state)
{
    if(state.pos_obj.ball_pos_x == INDENT_HOR_WALLS + OFFSET_RACKETS - 1){ return PLAYER_2_HIT; }
    else if(state.pos_obj.ball_pos_x == COLLS - INDENT_HOR_WALLS - OFFSET_RACKETS + 1) { return PLAYER_1_HIT; }
    else if((state.pos_obj.ball_pos_x - 1 == state.pos_obj.racketx[0]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[0])) { return RACKET_1_CENTRE; }
    else if((state.pos_obj.ball_pos_x + 1 == state.pos_obj.racketx[1]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[1])) { return RACKET_2_CENTRE; }
    else if((state.pos_obj.ball_pos_x - 1 == state.pos_obj.racketx[0]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[0] + 1)) { return RACKET_1_UPPER; }
    else if((state.pos_obj.ball_pos_x + 1 == state.pos_obj.racketx[1]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[1] + 1)) { return RACKET_2_UPPER; }
    else if((state.pos_obj.ball_pos_x - 1 == state.pos_obj.racketx[0]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[0] - 1)) { return RACKET_1_BOTTOM; }
    else if((state.pos_obj.ball_pos_x + 1 == state.pos_obj.racketx[1]) && (state.pos_obj.ball_pos_y == state.pos_obj.rackety[1] - 1)) { return RACKET_2_BOTTOM; }
    else if(state.pos_obj.ball_pos_y == 1) { return UPPER_WALL; }
    else if(state.pos_obj.ball_pos_y == ROWS - 1) { return BOTTOM_WALL; }
    else { return NO_COLLISION; }
}

void update_ball_position(enum collisions ball_col, struct game_state* state)
{
    if(ball_col == UPPER_WALL || ball_col == BOTTOM_WALL)
    {
        state->pos_obj.ball_v_y = (ball_col == UPPER_WALL) ? -1 : 1;
        return;
    }
    else if(ball_col == RACKET_1_CENTRE || RACKET_2_CENTRE)
    {
        state->pos_obj.ball_v_y = 0;
        state->pos_obj.ball_pos_x *= -1;
        return;
    }
    else if(ball_col == RACKET_1_UPPER || ball_col == RACKET_2_UPPER)
    {
        state->pos_obj.ball_v_y = 1;
        state->pos_obj.ball_v_x *= -1;
        return;
    }
    else if(ball_col == RACKET_1_BOTTOM || ball_col == RACKET_2_BOTTOM)
    {
        state->pos_obj.ball_v_y = -1;
        state->pos_obj.ball_v_x *= -1;
        return;
    }
    else if(ball_col == PLAYER_1_HIT)
    {
        printf("Player 1 hit. Score = %d\n", state->score1);
        init_field(state, state->score1 + 1, state->score2);
        return;
    }
    else if(ball_col == PLAYER_2_HIT)
    {
        printf("Player 2 hit. Score = %d\n", state->score2);
        init_field(state, state->score1, state->score2 + 1);
        return;
    }
    else { return; }
}

