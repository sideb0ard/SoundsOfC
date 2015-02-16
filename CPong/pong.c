// pong.c
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "portaudio.h"

#define DELAY 30000

#define NUM_MSECONDS   (50)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

typedef struct vector_t {
  int x;
  int y;
}vector_t;

typedef struct ball {
  vector_t location;
  vector_t velocity;
  vector_t next;
}ball;

typedef struct
{
    int freq;
    float *sine;
    int left_phase;
    int right_phase;
    char message[20];
}sinData;

typedef struct
{
    float left_phase;
    float right_phase;
}sawData;


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
/* PortAudio Shizzzzle */


/*
 * These routines is called by portaudio when playback is done.
 */
static void SinStreamFinished( void* userData )
{
   sinData *data = (sinData *) userData;
}
static void SawStreamFinished( void* userData )
{
   sawData *data = (sawData *) userData;
}

static int paSaw( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    sawData *data = (sawData*)userData;
    float *out = (float*)outputBuffer;
    unsigned int i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->left_phase;  /* left */
        *out++ = data->right_phase;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
    }

    return 0;
}

static int paSine( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    sinData *data = (sinData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for( i=0; i<framesPerBuffer; i++ )
    {
        *out++ = data->sine[data->left_phase];  /* left */
        *out++ = data->sine[data->right_phase];  /* right */
        data->left_phase += 1;
        if( data->left_phase >= data->freq ) data->left_phase -= data->freq;
        data->right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if( data->right_phase >= data->freq ) data->right_phase -= data->freq;
    }

    return paContinue;
}

//////////////////////////////////////////////////////////////////

int play_saw(PaStreamCallback *funcy) {

  pid_t pID = fork();
  if (pID >= 0)  // successful
  {
    if (pID == 0) // child
    {
      PaStreamParameters outputParameters;
      PaStream *stream;
      PaError err;
      sawData data;

      data.left_phase = data.right_phase = 0.0;
      
      err = Pa_Initialize();
      if( err != paNoError ) goto error;

      outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
      if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
      }
      outputParameters.channelCount = 2;       /* stereo output */
      outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
      outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
      outputParameters.hostApiSpecificStreamInfo = NULL;

      err = Pa_OpenStream(
                &stream,
                NULL, /* no input */
                &outputParameters,
                SAMPLE_RATE,
                FRAMES_PER_BUFFER,
                paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                //portAudioCallback,
                //paSine,
                funcy,
                &data );
      if( err != paNoError ) goto error;

      err = Pa_SetStreamFinishedCallback( stream, &SawStreamFinished );
      if( err != paNoError ) goto error;

      err = Pa_StartStream( stream );
      if( err != paNoError ) goto error;

      Pa_Sleep( NUM_MSECONDS );

      err = Pa_StopStream( stream );
      if( err != paNoError ) goto error;

      err = Pa_CloseStream( stream );
      if( err != paNoError ) goto error;

      Pa_Terminate();
      
      exit(0);
error:
      Pa_Terminate();
      exit(1);
    }
    else // parent
    {
      return 0;
    }
  }
  else // failed to fork
  {
      printf("ooft!\n");
      exit(1);
  }
}

int play_sin(int freq, PaStreamCallback *funcy) {

  pid_t pID = fork();
  if (pID >= 0)  // successful
  {
    if (pID == 0) // child
    {
      PaStreamParameters outputParameters;
      PaStream *stream;
      PaError err;
      sinData data;
      data.freq = freq;
      data.sine = malloc(freq * sizeof(float));

      int i;

      /* initialise sinusoidal wavetable */
      for( i=0; i<freq; i++ )
      {
          data.sine[i] = (float) sin( ((double)i/(double)freq) * M_PI * 2. );
      }
      data.left_phase = data.right_phase = 0.0;
      
      err = Pa_Initialize();
      if( err != paNoError ) goto error;

      outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
      if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto error;
      }
      outputParameters.channelCount = 2;       /* stereo output */
      outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
      outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
      outputParameters.hostApiSpecificStreamInfo = NULL;

      err = Pa_OpenStream(
                &stream,
                NULL, /* no input */
                &outputParameters,
                SAMPLE_RATE,
                FRAMES_PER_BUFFER,
                paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                //portAudioCallback,
                //paSine,
                funcy,
                &data );
      if( err != paNoError ) goto error;

      sprintf( data.message, "No Message" );
      err = Pa_SetStreamFinishedCallback( stream, &SinStreamFinished );
      if( err != paNoError ) goto error;

      err = Pa_StartStream( stream );
      if( err != paNoError ) goto error;

      Pa_Sleep( NUM_MSECONDS );

      err = Pa_StopStream( stream );
      if( err != paNoError ) goto error;

      err = Pa_CloseStream( stream );
      if( err != paNoError ) goto error;

      Pa_Terminate();
      
      exit(0);
error:
      Pa_Terminate();
      exit(1);
    }
    else // parent
    {
      return 0;
    }
  }
  else // failed to fork
  {
      printf("ooft!\n");
      exit(1);
  }
}


//////////////////////////////////////////////////////////////////

void cleanup(int signal) {
  int status;
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}

int main(int argc, char *argv[])
{
  int parent_x, parent_y, new_x, new_y;
  int score_size = 3;

  // clean up forked zombie children
  signal(SIGCHLD, cleanup);

  // setup bouncing ball
  ball b = {{ 0, 0 }, { 1, 1 }, { 0, 0 }};

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

  // sound FUncs
  // PaStreamCallback *mySineFunc = &paSine; 
  PaStreamCallback *mySawFunc = &paSaw; 

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

    // draw to our windows
    //mvwprintw(field, 1, 1, "Field");
    mvwprintw(field, b.location.y, b.location.x, "o");
    mvwprintw(score, 1, 1, "Score");

    // refresh each win
    wrefresh(field);
    wrefresh(score);

    usleep(DELAY);

    getmaxyx(field, b.next.y, b.next.x);
    if (b.location.x > (b.next.x - 2) || b.location.x < 1) {
        //play_sin(b.location.x * b.velocity.y, mySawFunc);
        //play_sin(b.location.x * b.velocity.y, mySawFunc);
        play_saw(mySawFunc);
        b.velocity.x *= -1;
    }
    if (b.location.y > (b.next.y - 1) || b.location.y < 1) {
        play_saw(mySawFunc);
        b.velocity.y *= -1;
    }
  }

  delwin(field);
  delwin(score);
  endwin();

  return 0;
}
