//#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
//
//#include "portaudio.h"
//
#define DELAY 30000
//
//#define NUM_MSECONDS   (50)
//#define SAMPLE_RATE   (44100)
//#define FRAMES_PER_BUFFER  (64)
//
//#ifndef M_PI
//#define M_PI  (3.14159265)
//#endif

typedef struct vector_t {
  int x;
  int y;
}vector_t;

typedef struct ball {
  vector_t location;
  vector_t velocity;
  vector_t next;
}ball;


int play_sin(int freq);
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

