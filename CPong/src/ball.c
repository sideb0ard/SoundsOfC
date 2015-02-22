#include <ncurses.h>
#include <stdlib.h>

#include "field.h"
#include "ball.h"
#include "pa_sin.h"

Ball *ball_init(Field *field) {
  Ball *ball = malloc(sizeof(Ball));
  ball_reset(ball, field);
  return ball;
}

void ball_draw(Ball *ball, Field *field) {
  mvwprintw(field->game, ball->location.y, ball->location.x, "o");
}

void ball_reset(Ball *ball, Field *field) {
  int mid_x, mid_y;

  field_get_midpoints(field, &mid_x, &mid_y);

  ball->location.x = mid_x;
  ball->location.y = mid_y;

  ball->velocity.x = 2;
  ball->velocity.y = 1;
}

int ball_move(Ball *ball, Field *field, Player *player) {
  int max_x = 0,
      max_y = 0;
  //    next_x = ball->location.x + ball->velocity.x,
  //    next_y = ball->location.y + ball->velocity.y;

  int result = BALL_MOVED;

  //int player_y1 = player->location.y,
  //    player_y2 = player->location.y + player->length;

  getmaxyx(field->game, max_y, max_x);

  if (ball->location.x > (max_x - 2) || ball->location.x < 1) {
    ball->velocity.x *= -1;
    play_sin(ball->location.x + 140);
  } 
  if (ball->location.y > (max_y - 2) || ball->location.y < 1) {
    ball->velocity.y *= -1;
    play_sin((ball->location.y * ball->location.y % 30) + 130);
  }
  //if (next_y <= 0 || next_y >= max_y) {
  //  ball->velocity.y *= -1;
  //} else if (next_x >= max_x) {
  //  ball->velocity.x *= -1;
  //} else if (next_x <= 0) {

  //  if (next_y >= player_y1 && next_y <= player_y2) {
  //    ball->velocity.x *= -1;
  //    result = BALL_SCORE;
  //  } else {
  //    result = BALL_MISS;
  //  }
  //}

  ball->location.x += ball->velocity.x;
  ball->location.y += ball->velocity.y;

  return result;
}


