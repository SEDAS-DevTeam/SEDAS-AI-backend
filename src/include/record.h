#include <portaudio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>

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

#define SAMPLE_RATE (44100)
#define NUM_CHANNELS (1)
#define SAMPLE_FORMAT paInt16       // 16-bit PCM
#define FRAMES_PER_BUFFER (512)

std::string out_path = "./temp_out/controller.wav";