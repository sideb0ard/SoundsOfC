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
  wattron(field->game, COLOR_PAIR(7));
  mvwprintw(field->game, ball->location.y, ball->location.x, "o");
  wattroff(field->game, COLOR_PAIR(7));
}

void ball_reset(Ball *ball, Field *field) {
  int mid_x, mid_y;
  field_get_midpoints(field, &mid_x, &mid_y);
  ball->location.x = 30;
  ball->location.y = 2;
  ball->velocity.x = 1;
  ball->velocity.y = 1;
}

int ball_move(Ball *ball, Field *field, Player *player) {
  int max_x = 0,
      max_y = 0;

  int result = BALL_MOVED;

  int player_y1 = player->location.y,
      player_x1 = player->location.x,
      player_x2 = player->location.x + player->length;

  getmaxyx(field->game, max_y, max_x);

  if (ball->location.x > (max_x - 2) || ball->location.x < 1) {
    ball->velocity.x *= -1;
    //play_sin(ball->location.x + 140);
  } 
  if (ball->location.y > (max_y - 2) || ball->location.y < 1) {
    ball->velocity.y *= -1;
    //play_sin((ball->location.y * ball->location.y % 30) + 130);
  }
  
  int next_x = ball->location.x + ball->velocity.x,
      next_y = ball->location.y + ball->velocity.y;

  //if (ball->location.y >= player_y1) {
  if (next_y >= player_y1) {
    // deliberately increasing reach of bat to account for 
    // bouncing off corners next:
    if (ball->location.x >= (player_x1 -2) && 
          ball->location.x <= (player_x2 + 2)) {
      ball->velocity.y *= -1;
      //play_sin(ball->location.x + 340);
      play_sin(440);
      result = BALL_SCORE;
    } else {
      result = BALL_MISS;
    }
  }


  //ball->location.x += ball->velocity.x;
  //ball->location.y += ball->velocity.y;
  ball->location.x = next_x;
  ball->location.y = next_y;

  return result;
}


