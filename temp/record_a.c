#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <math.h>
#include <portaudio.h>
#include <math.h>
#include <sndfile.h>

#define MIN_TALKING_BUFFERS 8
#define TALKING_THRESHOLD_WEIGHT 0.99
#define TALKING_TRIGGER_RATIO 4.0
#define SAMPLE_RATE       (44100)  // в 1 секунде записи содержится 44100 семплов.
#define FRAMES_PER_BUFFER   (1024)
#define SAMPLE_SILENCE  (0.0f)
#define NUM_SECONDS     (2)
#define BUFFER_LEN      1024

typedef struct s_audio{
    uint16_t format_type;
    uint8_t number_channels;
    uint32_t sample_rate;
    size_t size;
    float *rec_samples;
}               t_audio;

typedef struct s_a_snippet{
    float *snippet;
    size_t size;
}           t_a_snippet;


//#include "uchat.h"

static t_audio * init_audio_data() {
    t_audio *data = malloc(sizeof(t_audio));
    data->format_type = paFloat32;  //r
    data->number_channels = 0;   //remove
    data->sample_rate = SAMPLE_RATE;
    data->size = 0;
    data->rec_samples = NULL;
    return data;
}

static int process_stream(PaStream *stream, t_audio *data,
                          t_a_snippet *sample_block, int *i) {
    if (!stream || !data || !sample_block)
        return -1;
//    static int i = 0;
    (*i)++;
//    printf("process_stream  %d\n", (*i));
    Pa_ReadStream(stream, sample_block->snippet, FRAMES_PER_BUFFER);
    data->rec_samples = realloc(data->rec_samples, sample_block->size * (*i));
    data->size = sample_block->size * (*i);
    if (data->rec_samples) {
        size_t next_ndex = ((*i)- 1) * sample_block->size;
        char *destination = (char*)data->rec_samples + next_ndex;
        memcpy(destination, sample_block->snippet, sample_block->size);
    }
    else{
        free(data->rec_samples);
        data->rec_samples = NULL;
        data->size = 0;
//            i = 0;
    }
    return 0;
}

static int record(PaStream *stream, t_audio *data, t_a_snippet *sample_block) {
    int err = 0;
    int j = 0;

    printf("Wire on. Will run %d seconds.\n", NUM_SECONDS);
    fflush(stdout);
    for (int i = 0; i < (NUM_SECONDS * SAMPLE_RATE) / FRAMES_PER_BUFFER; ++i) {
        err = process_stream(stream, data, sample_block, &j);
    }

    mx_save_audio(data);
    printf("Wire off.\n"); fflush(stdout);
    err = Pa_StopStream(stream);
    if (err != paNoError)
        mx_exit_stream(data, err);
    return err;
}

char *mx_record_audio(void) {
    t_audio *data = init_audio_data();
    t_a_snippet *sample_block = malloc(sizeof(t_a_snippet));
    PaError err = paNoError;

    sample_block->snippet = NULL;
    sample_block->size = 0;
    PaStream *stream = NULL;
//    bool sample_complete = false;
    printf(" start record\n");
    err = mx_init_stream(&stream, data, sample_block);
    if (err){
        fprintf(stderr, "%s\n", "error");
        return NULL;
    }
    err = record(stream, data, sample_block);
    if (err != 0)
        printf("err =%d\n", err);
    printf(" exit record\n");
    printf("record to file->%s\n", data->file_name);
    return data->file_name;
}

int mx_init_stream(PaStream **stream, t_audio *data, t_a_snippet *sample_block) {
    PaError err;
    PaStreamParameters input_parameters;
    PaStreamParameters output_parameters;
    const PaDeviceInfo* inputInfo;
    const PaDeviceInfo* outputInfo;
    int numChannels;

//    const PaDeviceInfo *info;
    if (!stream || !data || !sample_block)
        return -1;

    printf("patest_read_write_wire.c\n"); fflush(stdout);
    printf("sizeof(int) = %lu\n", sizeof(int)); fflush(stdout);
    printf("sizeof(long) = %lu\n", sizeof(long)); fflush(stdout);

    err = Pa_Initialize();
    if (err != paNoError)
        return mx_exit_stream(data, err);
    err = Pa_GetDeviceCount();
//    info = Pa_GetDeviceInfo(Pa_GetDefaultInputDevice());
//    if (!info) {
//        fprintf(stdout, "%s\n", "Unable to find info on default input device.");
//        return -1;
//    }
    input_parameters.device = Pa_GetDefaultInputDevice();
    printf( "Input device # %d.\n", input_parameters.device);
    inputInfo = Pa_GetDeviceInfo(input_parameters.device);
    printf( "    Name: %s\n", inputInfo->name );
    printf( "      LL: %g s\n", inputInfo->defaultLowInputLatency);
    printf( "      HL: %g s\n", inputInfo->defaultHighInputLatency);

    if (input_parameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        return -1;
//        mx_exit_stream(data);
    }
    output_parameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    printf( "Output device # %d.\n", output_parameters.device);
    outputInfo = Pa_GetDeviceInfo( output_parameters.device);
    printf( "   Name: %s\n", outputInfo->name );
    printf( "     LL: %g s\n", outputInfo->defaultLowOutputLatency);
    printf( "     HL: %g s\n", outputInfo->defaultHighOutputLatency);

    numChannels = inputInfo->maxInputChannels < outputInfo->maxOutputChannels
                  ? inputInfo->maxInputChannels : outputInfo->maxOutputChannels;
    printf( "Num channels = %d.\n", numChannels );

    data->number_channels = numChannels;
    input_parameters.channelCount = numChannels;
    input_parameters.sampleFormat = paFloat32;
    input_parameters.suggestedLatency = inputInfo->defaultLowInputLatency;
//            Pa_GetDeviceInfo(input_parameters.device)->defaultLowInputLatency;
    input_parameters.hostApiSpecificStreamInfo = NULL;
    err = Pa_OpenStream(stream, &input_parameters, NULL, data->sample_rate, FRAMES_PER_BUFFER, paClipOff, NULL, NULL);
    if (err)
        return mx_exit_stream(data, err);
    sample_block->size = FRAMES_PER_BUFFER * sizeof(float) * data->number_channels;  //number bytes
    sample_block->snippet = malloc(sample_block->size);

    if( sample_block->snippet == NULL ) {
        printf("Could not allocate record array.\n");
        return mx_exit_stream(data, err);
    }
    memset(sample_block->snippet, SAMPLE_SILENCE, sample_block->size);

    return Pa_StartStream(*stream);
}

int mx_exit_stream(t_audio *data, PaError err) {
//    PaError             err = paNoError;
    Pa_Terminate();
    if (data->rec_samples)
        free (data->rec_samples);
    if (err != paNoError) {
        fprintf(stderr, "An error occured while using the portaudio stream\n" );
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error message: %s\n", Pa_GetErrorText( err ));
        err = 1; // Always return 0 or 1, but no other return codes.
    }
    return err;
}

static long mx_save_audio(t_audio *data) {
    uint8_t err = SF_ERR_NO_ERROR;
    SF_INFO sfinfo ={
            .channels = data->number_channels,
            .samplerate = data->sample_rate,
            .format = SF_FORMAT_AIFF | SF_FORMAT_FLOAT
    };
    char file_name[100];
    snprintf(file_name, 100, "./Uchat_downloads/rec_massage:%d.aif", rand());  //rand -> replace by message id
    printf("start save audio\n");
    SNDFILE *outfile = sf_open(file_name, SFM_WRITE, &sfinfo);
    if (!outfile) {
        printf("error outfile =%d\n", sf_error(outfile));
        return -1;
    }
    long wr = sf_writef_float(outfile, data->rec_samples, data->size / sizeof(float));
    err = data->size - wr;
    printf("data to write to file =%zu\n", data->size);
    printf("write to file =%lu\n", wr);
    sf_write_sync(outfile);
    sf_close(outfile);
    data->file_name = strdup(file_name);
    return err;
}



float mx_change_threshold(float talking_threshold, float talking_ntensity) {
    return TALKING_THRESHOLD_WEIGHT * talking_threshold +
           (1 - TALKING_THRESHOLD_WEIGHT) * talking_ntensity /
           TALKING_TRIGGER_RATIO;
}

float mx_rms(float *data, size_t len) {
    // RMS (Root Mean Square) – это среднеквадратическое значение
    // громкостей всех семплов дорожки.
    double sum = 0.0;
    for (size_t i = 0; i < len; ++i) {
        sum += pow(data[i], 2);
    }
    return sqrt(sum / len);
}

// now not useds
int mx_process_stream_ext(PaStream *stream, t_audio *data,
                          t_a_snippet *sample_block, const char *fileName,
                          bool *sample_complete) {

    if (!stream || !data || !sample_block || !sample_complete)
        return -1;
    (void) fileName;
    static int i = 0;
    time_t talking = 0;
    time_t silence = 0;
    float talking_threshold = 0.000750;
    bool was_talking = false;
    PaError err = 0;
    Pa_ReadStream(stream, sample_block->snippet, FRAMES_PER_BUFFER);
    float talking_intensity = mx_rms(sample_block->snippet, FRAMES_PER_BUFFER);
    if (err) {
        return err;
    }
    if (talking_intensity > talking_threshold) {
        if (!was_talking) {
            was_talking = true;
            talking_threshold /= TALKING_TRIGGER_RATIO;  // уменьшаем порог чуствительность записи
        }

        talking_threshold = mx_change_threshold(talking_threshold, talking_intensity);
        printf("Listening: %d\n", i);
        i++;
        time(&talking);
        data->rec_samples = realloc(data->rec_samples, sample_block->size * i);
        data->size = sample_block->size * i;
        if (data->rec_samples){
            size_t next_ndex = (i - 1) * sample_block->size;
            char *destination = (char*)data->rec_samples + next_ndex;
            memcpy(destination, sample_block->snippet, sample_block->size);
        }
        else{
            free(data->rec_samples);
            data->rec_samples = NULL;
            data->size = 0;
        }
    } else {
        if (was_talking) {
            was_talking = false;
            talking_threshold *= TALKING_TRIGGER_RATIO;
        }
        talking_threshold = mx_change_threshold(talking_threshold,talking_intensity);
        double test = difftime(time(&silence), talking);
        if (test >= 1.5 && test <= 10 && data->rec_samples && i >= MIN_TALKING_BUFFERS){
            if (sample_complete) *sample_complete = true;
            printf("ssssssss\n");
            // char buffer[100];
            //snprintf(buffer, 100, "file:%d.flac", i);
            mx_save_audio(data);
            talking = 0;
            free(data->rec_samples);
            data->rec_samples = NULL;
            data->size = 0;
            i = 0;
        }
    }
    return err;
//    free (sample_block->snippet);
//    Pa_Terminate();
}




////

int main (int argc, char * argv []) {
    mx_record_audio();
//    mx_play_sound_file(argv[1]);
    return 0;

}



