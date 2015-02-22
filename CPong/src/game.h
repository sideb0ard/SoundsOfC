#ifndef GAME_H
#define GAME_H

#include "structs.h"

#define MICROSECONDS_PER_SECOND 1000000
#define BALL_DELAY 0.03

#define MOVE_LEFT   'n'
#define MOVE_RIGHT 'm'

Game *game_init(struct timeval start_time);
void game_tick(Game *game);
void game_destroy(Game *game);

#endif
