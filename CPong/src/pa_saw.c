#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "portaudio.h"

#define NUM_MSECONDS   (50)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

//#ifndef M_PI
//#define M_PI  (3.14159265)
//#endif

typedef struct
{
    float left_phase;
    float right_phase;
}sawData;


static void SawStreamFinished( void* userData )
{
   sawData *data = (sawData *) userData;
}

static int SawFunctionCallback( const void *inputBuffer, void *outputBuffer,
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

int play_saw() {

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
                SawFunctionCallback,
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
