#include "audio_recorder.h"
#include <portaudio.h>
#include <sndfile.h>
#include <iostream>
#include <cstring>

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 1024
#define RECORDING_DURATION 30

struct AudioData {
    float* buffer;
    int frameIndex;
    int maxFrames;
};

static int recordCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData) {
    AudioData* data = (AudioData*)userData;
    const float* input = (const float*)inputBuffer;
    
    if (data->frameIndex + framesPerBuffer <= data->maxFrames) {
        memcpy(&data->buffer[data->frameIndex], input, framesPerBuffer * sizeof(float));
        data->frameIndex += framesPerBuffer;
        return paContinue;
    }
    
    return paComplete;
}

bool AudioRecorder::record(const std::string& outputPath) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    int totalFrames = SAMPLE_RATE * RECORDING_DURATION;
    AudioData data;
    data.buffer = new float[totalFrames];
    data.frameIndex = 0;
    data.maxFrames = totalFrames;

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaStream* stream;
    err = Pa_OpenStream(&stream,
                       &inputParameters,
                       nullptr,
                       SAMPLE_RATE,
                       FRAMES_PER_BUFFER,
                       paClipOff,
                       recordCallback,
                       &data);

    if (err != paNoError) {
        std::cerr << "Failed to open stream: " << Pa_GetErrorText(err) << std::endl;
        delete[] data.buffer;
        Pa_Terminate();
        return false;
    }

    std::cout << "Recording for " << RECORDING_DURATION << " seconds..." << std::endl;
    
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
        delete[] data.buffer;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return false;
    }

    while (Pa_IsStreamActive(stream)) {
        Pa_Sleep(100);
    }

    Pa_CloseStream(stream);
    Pa_Terminate();

    SF_INFO sfinfo;
    sfinfo.samplerate = SAMPLE_RATE;
    sfinfo.channels = 1;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    SNDFILE* file = sf_open(outputPath.c_str(), SFM_WRITE, &sfinfo);
    if (!file) {
        std::cerr << "Failed to open output file: " << sf_strerror(nullptr) << std::endl;
        delete[] data.buffer;
        return false;
    }

    sf_write_float(file, data.buffer, data.frameIndex);
    sf_close(file);
    delete[] data.buffer;

    std::cout << "Recording saved to: " << outputPath << std::endl;
    return true;
}