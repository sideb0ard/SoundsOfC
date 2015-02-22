#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct saw_data {
    float left_phase;
    float right_phase;
} saw_data;

typedef struct sin_data {
    int freq;
    float *sine;
    int left_phase;
    int right_phase;
    char message[20];
} sin_data;

typedef struct vector_t {
  int x;
  int y;
} vector_t;

typedef struct Ball {
  vector_t location;
  vector_t velocity;
  vector_t next;
} Ball;

typedef struct Field {
  WINDOW *container;
  WINDOW *game;
  WINDOW *score;
  int score_size;
} Field;

typedef struct Player {
  vector_t location;
  int length;
  int score;
} Player;

typedef struct Game {
  Player *player;
  //Ball *balls[10];
  Ball *ball;
  Field *field;
  struct timeval start_time;
} Game;

#endif
