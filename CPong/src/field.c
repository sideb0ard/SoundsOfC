#include <ncurses.h>
#include <stdlib.h>

#include "player.h"
#include "field.h"

Field * field_init() {
    Field *field = malloc(sizeof(Field));
    int max_x = 0, max_y = 0;
    int score_size = 3;

    // ncurses inittttt, mate?
    initscr();
    cbreak();  
    noecho(); // Don't output the pressed key
    keypad(stdscr, true); // enable keypad to get arrow keys
    nodelay(stdscr, TRUE); // Don't block on getch() calls
    curs_set(FALSE); // Don't display the cursor on-screen
    if(has_colors() == FALSE)
    { endwin();
      printf("Your terminal does not support color\n");
      exit(1);
    }
    start_color();      /* Start color      */
    init_pair(1, COLOR_RED,     COLOR_BLACK);
    init_pair(2, COLOR_GREEN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
    init_pair(4, COLOR_BLUE,    COLOR_BLACK);
    init_pair(5, COLOR_CYAN,    COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_WHITE,   COLOR_BLACK);


    getmaxyx(stdscr, max_y, max_x);

    field->container = stdscr;
    field->game      = newwin(max_y - score_size, max_x, 0, 0);
    field->score     = newwin(score_size, max_x, max_y - score_size, 0);
    field->score_size = score_size;

    return field;
}

void get_midpoints(WINDOW *window, int *mid_x, int *mid_y) {
    int max_x = 0, max_y = 0;

    getmaxyx(window, max_y, max_x);

    *mid_x = max_x / 2;
    *mid_y = max_y / 2;
}

void field_get_midpoints(Field *field, int *mid_x, int *mid_y) {
    // TODO: cache this on Field struct?
    get_midpoints(field->game, mid_x, mid_y);
}

void field_wclear(Field *field) {
  wclear(field->game);
  wclear(field->score);
}

void field_redraw(Field *field) {
    int container_x = 0,
        container_y = 0,
        game_x      = 0,
        game_y      = 0;

    getmaxyx(field->container, container_y, container_x);
    getmaxyx(field->game, game_y, game_x);

    // check if outer container size changed
    if (container_y != (game_y + 1) || container_x != game_x) {
        wresize(field->game, container_y - field->score_size, container_x);
        wresize(field->score, field->score_size, container_x);
        mvwin(field->score, container_y - field->score_size, 0);
    }
}

void field_draw_score(Field *field, Player *player) {
    int mid_x, mid_y;

    get_midpoints(field->score, &mid_x, &mid_y);

    mvwprintw(field->score, 0, (mid_x - 2), "%04d", player->score);
    //mvwprintw(field->score, 0, (mid_x - 2), "%s", "jobbiebumfart");
}

void field_destroy(Field *field) {
    delwin(field->game);
    delwin(field->score);

    endwin();

    free(field);
}
