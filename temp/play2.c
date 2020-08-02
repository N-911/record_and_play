#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <math.h>
//#include <portaudio.h>
#include <math.h>
//#include <sndfile.h>

#include "../libportaudio/include/portaudio.h"
#include "../libsndfile/src/sndfile.h"
#include <math.h>

#define NUM_CHANNELS    (2)
#define MIN_TALKING_BUFFERS 8
#define TALKING_THRESHOLD_WEIGHT 0.99
#define TALKING_TRIGGER_RATIO 4.0
#define SAMPLE_RATE       (44100)  // в 1 секунде записи содержится 44100 семплов.
#define FRAMES_PER_BUFFER   (512)
#define SAMPLE_SILENCE  (0.0f)
#define NUM_SECONDS     (4)
#define BUFFER_LEN    512

typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    SAMPLE      recordedSamples[FRAMES_PER_BUFFER * 2];
}
        paTestData;


static int playCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
    paTestData *data = (paTestData*)userData;
    SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer ) {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete; }
    else {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}


static void print_s_info(SF_INFO s_info) {
    printf ("frames = %lld\n", s_info.frames); fflush(stdout);
    printf ("samplerate = %d\n", s_info.samplerate); fflush(stdout);
    printf ("channels = %d\n", s_info.channels); fflush(stdout);
    printf ("format = %d\n", s_info.format); fflush(stdout);
    printf ("sections = %d\n", s_info.sections); fflush(stdout);
    printf ("seekable = %d\n", s_info.seekable); fflush(stdout);

}


static int init_stream(PaStream** stream, SF_INFO s_info) {
    PaStreamParameters output_parameters;
    PaError err;

    output_parameters.device = Pa_GetDefaultOutputDevice();
    output_parameters.channelCount = s_info.channels;
    output_parameters.sampleFormat = paFloat32;
    output_parameters.suggestedLatency = Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
    output_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(stream, NULL, &output_parameters, s_info.samplerate,
                        paFramesPerBufferUnspecified,
                        paClipOff,
                        NULL,
                        NULL);
    if (err != paNoError || ! stream) {
        printf("error init_stream =%s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return -1;
    }
    return Pa_StartStream(*stream);
}

int mx_play_sound_file(char *file_name, char *start_time, char *duration_t) {
    PaStream* stream = NULL;
    SNDFILE* a_file = NULL;
    SF_INFO s_info;
    PaError err;
    sf_count_t length;	// length of file in frames
    sf_count_t start_point; // start_point of frames read
    sf_count_t end_point; // end point of frames playing

    double starttime = 0;
    double duration = 0;
    paTestData          data2;

    int                 i;
//    int                 totalFrames;
//    int                 numSamples;
    int                 numBytes;
    SAMPLE              max, val;
    double              average;


    data2.maxFrameIndex = NUM_SECONDS * SAMPLE_RATE; // Record for a few seconds.
    data2.frameIndex = 0;
//    numSamples = totalFrames * NUM_CHANNELS;
//    numBytes = numSamples * sizeof(SAMPLE);
//    data.recordedSamples = (SAMPLE *) malloc( numBytes ); // From now on, recordedSamples is initialised.
//    if( data.recordedSamples == NULL )

    err = Pa_Initialize();
    if (err != paNoError) {
        printf("error Pa_Initialize =%s\n", Pa_GetErrorText(err));
        return -1;

    }
    printf("play 1\n");
    memset(&s_info, 0, sizeof(s_info));
    a_file = sf_open(file_name, SFM_READ, &s_info);
    if (!a_file) {
        printf("error open =%d\n", sf_error(a_file));
//        printf("error str open =%s\n", sf_error_number(a_file));
        print_s_info(s_info);
        Pa_Terminate();
        return -1;
    }

    print_s_info(s_info);

    printf("play 2\n");
    length = s_info.frames;
    if (start_time) {
        starttime = atof(start_time);
        start_point = (sf_count_t) starttime * s_info.samplerate;
    }
    else {
        start_point = 0;
    }  // start play with the beginning

    if (duration_t) {
        duration = atof(duration_t);
        end_point = (sf_count_t) (duration * s_info.samplerate + start_point);
        end_point = (end_point < length) ? end_point : length;
    }
    else {
        end_point = length;
        duration = (double) (end_point - start_point) / (double) s_info.samplerate;
    }

    printf("length frames =%lld\n", length);
    printf("starttime  =%f\n", starttime);
    printf("duration  =%f\n", duration);
    printf("start_point frames =%lld\n", start_point);
    printf("end_point frames =%lld\n\n", end_point);

    err = init_stream(&stream, s_info);
    if (err){
        fprintf(stderr, "%s\n", "error");
        return err;
    }
    printf("play 3\n");
    sf_count_t read_count = 0;
    sf_count_t read_sum = 0;

//    if (s_info.channels > 1)

//    float data[BUFFER_LEN * s_info.channels];
//    memset(data, 0, sizeof(data));
    memset(data2.recordedSamples, 0, sizeof(data2.recordedSamples));


    int subFormat = s_info.format & SF_FORMAT_SUBMASK;
    double scale = 1.0;
    int m = 0;
    sf_seek(a_file, start_point, SEEK_SET);

    printf("play 4\n");
/////===========
    printf("\n=== Now playing back. ===\n"); fflush(stdout);
    err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            SAMPLE_RATE,
            FRAMES_PER_BUFFER,
            paClipOff,      /* we won't output out of range samples so don't bother clipping them */
            playCallback,
            &data );
    if( err != paNoError ) goto done;

    if( stream )
    {
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto done;

        printf("Waiting for playback to finish.\n"); fflush(stdout);

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err < 0 ) goto done;

        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto done;

        printf("Done.\n"); fflush(stdout);
    }

    ///////======
    while ((read_count = sf_readf_float(a_file, data, BUFFER_LEN))) {
        if (subFormat == SF_FORMAT_FLOAT || subFormat == SF_FORMAT_DOUBLE) {
            for (m = 0 ; m < read_count ; ++m) {
                data[m] *= scale;
            }
        }
        read_sum += read_count;
        if (read_sum > end_point - start_point) {
            printf("read_sum frames =%lld\n", read_sum);
            break;
        }
        err = Pa_WriteStream(stream, data, BUFFER_LEN);
        if (err != paNoError) {
            printf("error Pa_WriteStream =%s\n", Pa_GetErrorText(err));
            break;
        }
        memset(data, 0, sizeof(data));
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
        printf("error Pa_CloseStream =%s\n", Pa_GetErrorText(err));

    err = Pa_Terminate();
    if( err != paNoError )
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    sf_close(a_file);
    return 0;
}

int main (int argc, char * argv []) {
    mx_play_sound_file(argv[1], argv[2], argv[3]);
    return 0;

}

