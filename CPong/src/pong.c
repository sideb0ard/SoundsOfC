#include "pong.h"

void cleanup(int signal) {
  int status;
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

void ninit() {
  // ncurses inittttt, mate?
  initscr();
  noecho();
  curs_set(FALSE);
}

int main(int argc, char *argv[])
{
  int parent_x, parent_y, new_x, new_y;
  int score_size = 3;

  int numballs;
  object balls[MAX_BALLS];
  if (argc == 2) {
    numballs = atoi(argv[1]);
  } else {
    numballs = 1;
    // defaults balls is 1
  }

  for ( int i = 0; i < numballs; i++) {
    balls[i] = (object) {{ i, i}, {i+1,i+1}, {0,0}};
  }

  // Setup signal handler for forked-up zombie children
  signal(SIGCHLD, cleanup);

  // ncurses initialize
  ninit();

  // get max window dimensions
  getmaxyx(stdscr, parent_y, parent_x);
  // set up intial windows
  WINDOW *field = newwin(parent_y - score_size, parent_x, 0, 0);
  WINDOW *score = newwin(score_size, parent_x, parent_y - score_size, 0);
  // draw borders
  draw_borders(field);
  draw_borders(score);

  int batlocation_x;
  int batlocation_y;
  //object bat = {{ parent_y - 7, parent_x / 2}, {1,1}, {0,0}};
  object bat = {{  parent_x / 2, parent_y - 6}, {1,1}, {0,0}};


  while(1) {     /// MAIN LOOP!

    getmaxyx(stdscr, new_y, new_x);
    wclear(field);  // clear field every time

    //TODO:1
    // check if window needs resized
    //resize_if_needed();
    if (new_y != parent_y || new_x != parent_x){
      parent_x = new_x;
      parent_y = new_y;

      wresize(field, new_y - score_size, new_x);
      wresize(score, score_size, new_x);
      mvwin(score, new_y - score_size, 0);

      wclear(stdscr);
      wclear(score);

    }

    //TODO:2 - FOR ALL WINDOWS, REDRAW BORDERS
    // draw borders
    draw_borders(field);
    draw_borders(score);

    // update balls
    for ( int i = 0; i < numballs; i++) {
      balls[i].location.x += balls[i].velocity.x;
      balls[i].location.y += balls[i].velocity.y;
    }

    // FOR ALL BALLS, DRAW POSITION ON FIELD
    for ( int i = 0; i < numballs; i++) {
      mvwprintw(field, balls[i].location.y, balls[i].location.x, "o");
    }
    // DRAW BAT
    //mvwprintw(field, bat.location.y, bat.location.x, "===");
    mvwprintw(field, bat.location.y, bat.location.x, "=");
    // DRAW SCORE
    mvwprintw(score, 1, 1, "Score");

    //TODO:5 - FOR ALL WINDOWS, REDRAW BORDERS
    // refresh each win
    wrefresh(field);
    wrefresh(score);

    usleep(DELAY);

    // FOR ALL BALLS, UPDATE POSITION
    for ( int i = 0; i < numballs; i++) {
      if (balls[i].location.x == bat.location.x && \
              balls[i].location.y == (bat.location.y + 1)) {
          balls[i].velocity.y *= -1;
          }
      update_position(field, &balls[i]);
    }
  }

  delwin(field);
  delwin(score);
  endwin();

  return 0;
}

