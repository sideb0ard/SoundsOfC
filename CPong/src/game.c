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
  Player *player = player_init(field, 8);
  Ball *ball = ball_init(field);

  game->field       = field;
  game->player      = player;
  game->ball        = ball;
  game->start_time  = start_time;

  return game;
}

void game_tick(Game *game) {
  int ch = 0, move_result = BALL_MOVED;

  long elapsed = elapsed_since(&game->start_time);
  static long last_event = 0;

  int new_x, new_y;

  //if((elapsed - last_event) >= (BALL_DELAY * MICROSECONDS_PER_SECOND)) {
    
  getmaxyx(stdscr, new_y, new_x);
  wclear(game->field->game);  // clear field every time
  wclear(game->field->score);  // clear field every time

  field_redraw(game->field);
  draw_borders(game->field->game);
  draw_borders(game->field->score);

  mvwprintw(game->field->game, 0, 0, "elapsed" );
  mvwprintw(game->field->score, 0, 0, "Score");
  move_result = ball_move(game->ball, game->field, game->player);


  if (move_result == BALL_SCORE) {
    game->player->score += 1;
  } else if (move_result == BALL_MISS) {
    game->player->score = 0;
    //ball_reset(game->ball, game->field);
  }
  //last_event = elapsed;

  if ((ch = getch()) != ERR) {
    if (ch == MOVE_LEFT) {
      player_move(game->field, game->player, -5);
    }
    if (ch == MOVE_RIGHT) {
      player_move(game->field, game->player, 5);
    }
  }

  field_draw_score(game->field, game->player);
  player_draw(game->player, game->field);
  ball_draw(game->ball, game->field);

  //wrefresh(game->field->game);
  //wrefresh(game->field->score);
  wnoutrefresh(game->field->game);
  wnoutrefresh(game->field->score);
  doupdate();
  //}
  usleep(BALL_DELAY * MICROSECONDS_PER_SECOND);
}

void game_destroy(Game *game) {
  field_destroy(game->field);
  free(game->player);
  free(game->ball);
  free(game);
}
