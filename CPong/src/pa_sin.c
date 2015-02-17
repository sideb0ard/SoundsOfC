#include "pong.h"

static void SinStreamFinished( void* userData )
{
   sin_data *data = (sin_data *) userData;
}

static int SinFunctionCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    sin_data *data = (sin_data*)userData;
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

int play_sin(int freq) {

  pid_t pID = fork();
  if (pID >= 0)  // successful
  {
    if (pID == 0) // child
    {
      PaStreamParameters outputParameters;
      PaStream *stream;
      PaError err;
      sin_data data;
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
                SinFunctionCallback,
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
