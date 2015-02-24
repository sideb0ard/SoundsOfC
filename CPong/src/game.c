#include <ncurses.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "ball.h"
#include "drawing_library.h"
#include "field.h"
#include "game.h"
#include "player.h"


long elapsed_since(struct timeval *time) {
  struct timeval now;
  gettimeofday(&now, NULL);
  return ((now.tv_sec   * MICROSECONDS_PER_SECOND) + now.tv_usec) -
         ((time->tv_sec * MICROSECONDS_PER_SECOND) + time->tv_usec);
}

Game * game_init(struct timeval start_time) {

  Game *game = malloc(sizeof(Game));

  Field *field = field_init();
  Player *player = player_init(field, 11);
  Ball *ball = ball_init(field);

  game->field       = field;
  game->player      = player;
  game->balls[0]    = ball;
  game->num_balls   = 1;
  game->start_time  = start_time;

  return game;
}

void game_tick(Game *game) {
  int ch = 0;
  int move_results[game->num_balls];
  for (int i = 0; i <game->num_balls; i++) {
    move_results[i] = BALL_MOVED;
  }

  long elapsed = elapsed_since(&game->start_time);

  field_wclear(game->field);
  field_redraw(game->field);

  draw_borders(game->field->game);
  draw_borders(game->field->score);

  for (int i = 0; i<game->num_balls; i++) {
    move_results[i] = ball_move(game->balls[i], \
        game->field, game->player);
  }


  if ((ch = getch()) != ERR) {
    //if (ch == MOVE_LEFT) {
    if (ch == KEY_LEFT) {
      player_move(game->field, game->player, -5);
    }
    if (ch == KEY_RIGHT) {
      player_move(game->field, game->player, 5);
    }
  }

  for (int i = 0; i<game->num_balls; i++) {
    if (move_results[i] == BALL_SCORE) {
      game->player->score += 1;
    } else if (move_results[i] == BALL_MISS) {
      //game->player->score = 0;
      ball_reset(game->balls[i], game->field);
      //player_reset(game->player, game->field);
    }
  }

  //if (game->player->score % 3 == 0) {
  //  Ball *newball = ball_init(game->field);
  //  game->balls[game->num_balls] = newball;
  //  game->num_balls++;
  //}


  field_draw_score(game->field, game->player);
  player_draw(game->player, game->field);
  for (int i = 0; i<game->num_balls; i++) {
    ball_draw(game->balls[i], game->field);
  }

  wnoutrefresh(game->field->game);
  wnoutrefresh(game->field->score);
  doupdate();
  //}
  usleep(BALL_DELAY * MICROSECONDS_PER_SECOND);
}

void game_destroy(Game *game) {
  field_destroy(game->field);
  free(game->player);
  for (int i = 0; i<game->num_balls; i++) {
    free(game->balls[i]);
  }
  free(game);
}
