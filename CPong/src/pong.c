//#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "pong.h"

void cleanup(int signal) {
  int status;
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

int main(int argc, char *argv[])
{
  int parent_x, parent_y, new_x, new_y;
  int score_size = 3;

  int numballs = 1;
  ball balls[10];
  if (argc == 2) {
    numballs = atoi(argv[1]);
  } else {
    // defaults balls is 1
  }

  // TODO: fix this - make array of balls wurk!
  //for ( int i = 0; i < numballs; i++) {
  //  balls[i] = {{ i, i}, {i+1,i+1}, {0,0}};
  //}


  // clean up forked zombie children
  signal(SIGCHLD, cleanup);

  // setup bouncing ball
  ball b = {{ 0, 0 }, { 1, 1 }, { 0, 0 }};
  ball btoo = {{ 1, 1 }, { 2, 1 }, { 0, 0 }};
  ball btho = {{ 2, 2 }, { 3, 3 }, { 0, 0 }};

  // ncurses inittttt, mate?
  initscr();
  noecho();
  curs_set(FALSE);

  // get max window dimensions
  getmaxyx(stdscr, parent_y, parent_x);

  // set up intial windows
  WINDOW *field = newwin(parent_y - score_size, parent_x, 0, 0);
  WINDOW *score = newwin(score_size, parent_x, parent_y - score_size, 0);

  // draw borders
  draw_borders(field);
  draw_borders(score);

  while(1) {
    getmaxyx(stdscr, new_y, new_x);
    // clear field every time
    wclear(field);
    if (new_y != parent_y || new_x != parent_x){
      parent_x = new_x;
      parent_y = new_y;

      wresize(field, new_y - score_size, new_x);
      wresize(score, score_size, new_x);
      mvwin(score, new_y - score_size, 0);

      wclear(stdscr);
      wclear(score);

    }
    // draw borders
    draw_borders(field);
    draw_borders(score);

    // update ball
    b.location.x += b.velocity.x;
    b.location.y += b.velocity.y;

    btoo.location.x += btoo.velocity.x;
    btoo.location.y += btoo.velocity.y;

    btho.location.x += btho.velocity.x;
    btho.location.y += btho.velocity.y;
    
    // draw to our windows
    //mvwprintw(field, 1, 1, "Field");
    mvwprintw(field, b.location.y, b.location.x, "o");
    mvwprintw(field, btoo.location.y, btoo.location.x, "*");
    mvwprintw(field, btho.location.y, btho.location.x, "$$");
    mvwprintw(score, 1, 1, "Score");

    // refresh each win
    wrefresh(field);
    wrefresh(score);

    usleep(DELAY);

    update_position(field, &b);
    update_position(field, &btoo);
    update_position(field, &btho);

  }

  delwin(field);
  delwin(score);
  endwin();

  return 0;
}

