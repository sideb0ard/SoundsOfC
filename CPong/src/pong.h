#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "portaudio.h"

#define DELAY 30000
#define FRAMES_PER_BUFFER 64
#define MAX_BALLS 10
#define NUM_MSECONDS 50
#define SAMPLE_RATE 44100

#ifndef M_PI
#define M_PI  3.14159265
#endif

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
}vector_t;

typedef struct ball {
  vector_t location;
  vector_t velocity;
  vector_t next;
}ball;


int play_sin(int freq);
void draw_borders(WINDOW *screen);
void update_position(WINDOW *w, ball *b);
