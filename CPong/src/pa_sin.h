#ifndef SIN_H
#define SIN_H

#include "structs.h"
#include "portaudio.h"

int play_sin(int freq);

#define FRAMES_PER_BUFFER 64
#define NUM_MSECONDS 50
#define SAMPLE_RATE 44100

#ifndef M_PI
#define M_PI  3.14159265
#endif

#endif
