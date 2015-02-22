#include <ncurses.h>
#include <stdlib.h>

#include "field.h"
#include "player.h"

Player * player_init(Field *field, int length) {
    int mid_x, mid_y;
    Player *player = malloc(sizeof(Player));

    field_get_midpoints(field, &mid_x, &mid_y);

    player->location.x      = 0;
    player->location.y      = mid_y - (length / 2);
    player->score  = 0;
    player->length = length;

    return player;
}

void player_draw(Player *player, Field *field) {
    int i = 0;
    int max_y = 0, max_x = 0;

    getmaxyx(field->game, max_y, max_x);

    // Make sure paddle doesn't go off screen
    if ((player->location.y + player->length) > max_y) {
        player->location.y = max_y - player->length;
    }

    // Draw player's paddle
    for (i = 0; i < player->length; i++) {
        mvwprintw(field->game, player->location.y + i, player->location.x, "|");
    }
}

void player_move(Field *field, Player *player, int direction) {
    int max_y = 0, max_x = 0;
    int next_y = player->location.y + direction;

    getmaxyx(field->game, max_y, max_x);

    if (next_y >= 0 && (next_y + player->length) <= max_y) {
        player->location.y = next_y;
    }
}
