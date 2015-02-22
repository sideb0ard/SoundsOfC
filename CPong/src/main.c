#include <sys/time.h>
#include <stdlib.h>
#include <ncurses.h>

#include "field.h"
#include "player.h"
#include "ball.h"
#include "game.h"

void cleanup(int signal) {
  int status;
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

int main(int argc, char *argv[]) {

  struct timeval start_time;
  gettimeofday(&start_time, NULL);
  Game *game = game_init(start_time);

  while(1) {
    game_tick(game);
  }

  game_destroy(game);
  
  return 0;
}

