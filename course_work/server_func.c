#include <server_func.h>

void init_field(struct game_state *state, int score1, int score2)
{
    int ball_v_x = (rand() % 2 == 0) ? -2 : 2;
    int ball_v_y = (rand() % 2 == 0) ? -1 : 1;
    state->pos_obj.ball_pos_x = COLLS / 2;
    state->pos_obj.ball_pos_y = INDENT_VERT_WALLS + 2 + (rand() % (ROWS - 4));
    state->pos_obj.ball_v_x = ball_v_x;
    state->pos_obj.ball_v_y = ball_v_y;
    state->pos_obj.racketx[0] = INDENT_HOR_WALLS + OFFSET_RACKETS;
    state->pos_obj.racketx[1] = COLLS - INDENT_HOR_WALLS - OFFSET_RACKETS;
    state->pos_obj.rackety[0] = state->pos_obj.rackety[1] = ROWS / 2;
    state->score1 = score1;
    state->score2 = score2;
}

void configure_server(struct sockaddr_in *serv_addr, int *sock_server, int *lenght)
{
    if ((*sock_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Create socket for server failed\n");
        exit(EXIT_FAILURE);
    }
    memset(serv_addr, 0, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(8080);
    serv_addr->sin_addr.s_addr = INADDR_ANY;
    int opt = 1;
    if (setsockopt(*sock_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    if ((bind(*sock_server, (struct sockaddr *)serv_addr, sizeof(*serv_addr))) == -1)
    {
        perror("Binding is failed\n");
        exit(EXIT_FAILURE);
    }
    *lenght = sizeof(*serv_addr);
    if ((getsockname(*sock_server, (struct sockaddr *)serv_addr, (socklen_t *)lenght) == -1))
    {
        perror("Getsockname error\n");
        exit(EXIT_FAILURE);
    }
    printf("Server port: %d\n", htons(serv_addr->sin_port));
    listen(*sock_server, AMOUNT_PLAYERS);
}

enum collisions check_wall_collision(struct game_state state)
{
    int ball_x = state.pos_obj.ball_pos_x;
    int ball_y = state.pos_obj.ball_pos_y;
    int racket1_x = state.pos_obj.racketx[0];
    int racket1_y = state.pos_obj.rackety[0];
    int racket2_x = state.pos_obj.racketx[1];
    int racket2_y = state.pos_obj.rackety[1];
    if (ball_x <= INDENT_HOR_WALLS + OFFSET_RACKETS - 4)
    {
        return PLAYER_2_HIT;
    }
    else if (ball_x >= COLLS - INDENT_HOR_WALLS - OFFSET_RACKETS + 4)
    {
        return PLAYER_1_HIT;
    }
    if (ball_x <= racket1_x + 2 && ball_x >= racket1_x)
    {
        if (ball_y == racket1_y)
            return RACKET_1_CENTRE;
        else if (ball_y == racket1_y + 1)
            return RACKET_1_UPPER;
        else if (ball_y == racket1_y - 1)
            return RACKET_1_BOTTOM;
    }
    if (ball_x >= racket2_x - 2 && ball_x <= racket2_x)
    {
        if (ball_y == racket2_y)
            return RACKET_2_CENTRE;
        else if (ball_y == racket2_y + 1)
            return RACKET_2_UPPER;
        else if (ball_y == racket2_y - 1)
            return RACKET_2_BOTTOM;
    }
    if (ball_y <= INDENT_VERT_WALLS)
        return UPPER_WALL;
    else if (ball_y >= ROWS - 2)
        return BOTTOM_WALL;

    return NO_COLLISION;
}

void update_ball_position(enum collisions ball_col, struct game_state *state)
{
    if (ball_col == UPPER_WALL || ball_col == BOTTOM_WALL)
    {
        state->pos_obj.ball_v_y = -state->pos_obj.ball_v_y;
        return;
    }
    else if (ball_col == RACKET_1_CENTRE || ball_col == RACKET_2_CENTRE)
    {
        state->pos_obj.ball_v_y = 0;
        state->pos_obj.ball_v_x = -state->pos_obj.ball_v_x;
        return;
    }
    else if (ball_col == RACKET_1_UPPER || ball_col == RACKET_2_UPPER)
    {
        state->pos_obj.ball_v_y = 1;
        state->pos_obj.ball_v_x = -state->pos_obj.ball_v_x;
        return;
    }
    else if (ball_col == RACKET_1_BOTTOM || ball_col == RACKET_2_BOTTOM)
    {
        state->pos_obj.ball_v_y = -1;
        state->pos_obj.ball_v_x = -state->pos_obj.ball_v_x;
        return;
    }
    else if (ball_col == PLAYER_1_HIT)
    {
        printf("Player 1 hit. Score = %d\n", state->score1);
        init_field(state, state->score1 + 1, state->score2);
        return;
    }
    else if (ball_col == PLAYER_2_HIT)
    {
        printf("Player 2 hit. Score = %d\n", state->score2);
        init_field(state, state->score1, state->score2 + 1);
        return;
    }
    else
    {
        return;
    }
}
