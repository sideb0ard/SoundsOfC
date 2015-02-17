//#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "pong.h"

//////////////////////////////////////////////////////////////////

void draw_borders(WINDOW *screen) {
  int x, y, i;

  getmaxyx(screen, y, x);

  // 4cornz
  mvwprintw(screen, 0, 0, "+");
  mvwprintw(screen, y - 1, 0, "+");
  mvwprintw(screen, 0, x - 1, "+");
  mvwprintw(screen, y - 1, x - 1, "+");

  // sides
  for (i = 1; i < (y - 1); i++) {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }

  // top and bottom
  for (i = 1; i < (x - 1); i++) {
    mvwprintw(screen, 0, i, "-");
    mvwprintw(screen, y - 1, i, "-");
  }
}



//////////////////////////////////////////////////////////////////
//
void update_position(WINDOW *w, ball *b) {
    getmaxyx(w, b->next.y, b->next.x);
    if (b->location.x > (b->next.x - 2) || b->location.x < 1) {
        //play_sin(b.location.x * b.velocity.y, mySawFunc);
        play_sin(b->location.x + 140);
        //play_saw(mySawFunc);
        b->velocity.x *= -1;
    }
    if (b->location.y > (b->next.y - 1) || b->location.y < 1) {
        //play_saw(mySawFunc);
        play_sin((b->location.y * b->location.y % 30) + 130);
        b->velocity.y *= -1;
    }
}
