#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <sndfile.h>
#include <cstring>

struct WAVHeader {
    char riff[4];                    // "RIFF"
    uint32_t overall_size;           // File size - 8 bytes
    char wave[4];                    // "WAVE"
    char fmt_chunk_marker[4];        // "fmt "
    uint32_t length_of_fmt;          // Length of format data (16 for PCM)
    uint16_t format_type;            // Format type (1 for PCM)
    uint16_t channels;               // Number of channels
    uint32_t sample_rate;            // Sampling rate
    uint32_t byterate;               // Bytes per second
    uint16_t block_align;            // Block alignment
    uint16_t bits_per_sample;        // Bits per sample
    char data_chunk_header[4];       // "data"
    uint32_t data_size;              // Data size
};

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_FORMAT paInt16       // 16-bit PCM
#define FRAMES_PER_BUFFER 512
#define NUM_SECONDS 60 // maximum number of seconds allocated
#define VOLUME_MOD 2.0f

typedef struct {
    int frameIndex;
    int maxFrameIndex;
    float *recordedSamples;
} AudioData;

static int record_callback(const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo *timeInfo,
                           PaStreamCallbackFlags statusFlags, void *userData) {
    AudioData *data = (AudioData *)userData;

    const float *rptr = (const float *)inputBuffer; // Input is float because of paFloat32
    float *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    unsigned long framesToProcess = (data->frameIndex + framesPerBuffer > data->maxFrameIndex)
                                        ? data->maxFrameIndex - data->frameIndex
                                        : framesPerBuffer;

    if (inputBuffer == nullptr) {
        memset(wptr, 0, framesToProcess * NUM_CHANNELS * sizeof(float)); // Silence if no input
    } else {
        memcpy(wptr, rptr, framesToProcess * NUM_CHANNELS * sizeof(float)); // Copy input
    }

    data->frameIndex += framesToProcess;
    return (data->frameIndex >= data->maxFrameIndex) ? paComplete : paContinue;
}

void save_to_wav(const char *filename, const AudioData &data){
    SF_INFO sfinfo;
    sfinfo.channels = NUM_CHANNELS;
    sfinfo.samplerate = SAMPLE_RATE;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* out_file = sf_open(filename,  SFM_WRITE, &sfinfo);
    if (!out_file) {
        std::cerr << "Failed to open WAV file for writing: " << sf_strerror(out_file) << std::endl;
        return;
    }

    float *adjustedSamples = new float[data.frameIndex * NUM_CHANNELS];
    for (int i = 0; i < data.frameIndex * NUM_CHANNELS; ++i){
        adjustedSamples[i] = data.recordedSamples[i] * VOLUME_MOD;
    }

    sf_writef_float(out_file, data.recordedSamples, data.frameIndex * NUM_CHANNELS);
    sf_close(out_file);

    printw("Recording saved to %s\n", filename);
}

AudioData initialize_data(){
    Pa_Initialize();

    // Allocate memory for audio data
    AudioData audioData;
    audioData.maxFrameIndex = SAMPLE_RATE * NUM_SECONDS;
    audioData.frameIndex = 0;
    audioData.recordedSamples = (float *)malloc(audioData.maxFrameIndex * NUM_CHANNELS * sizeof(float));

    return audioData;
}

PaStream* start_stream(AudioData &data){
    PaStream* stream;
    Pa_OpenDefaultStream(&stream, NUM_CHANNELS, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, record_callback, &data);

    return stream;
}

class Recorder{
    public:
        AudioData data;
        PaStream* stream;

        void initialize(){
            data = initialize_data();
            stream = start_stream(data);
        }

        void start(){
            Pa_StartStream(stream);
        }

        void stop(){
            Pa_StopStream(stream);
            Pa_CloseStream(stream);

            save_to_wav(wav_out_path.c_str(), data);
        }

        void terminate(){
            Pa_Terminate();
        }
};