#include "speech_synthesizer.h"
#include <portaudio.h>
#include <sndfile.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <random>

#define SAMPLE_RATE 16000

bool SpeechSynthesizer::synthesize(const std::string& text, 
                                  const std::string& voiceModelPath,
                                  const std::string& outputPath) {
    
    std::cout << "Loading voice model: " << voiceModelPath << std::endl;
    std::vector<float> voiceEmbedding = loadVoiceEmbedding(voiceModelPath);
    
    if (voiceEmbedding.empty()) {
        std::cerr << "Failed to load voice model" << std::endl;
        return false;
    }
    
    std::cout << "Converting text to tokens..." << std::endl;
    std::vector<int> tokens = textToTokens(text);
    
    std::cout << "Generating speech for: \"" << text << "\"" << std::endl;
    std::vector<float> audio = generateSpeech(tokens, voiceEmbedding);
    
    if (audio.empty()) {
        std::cerr << "Failed to generate speech" << std::endl;
        return false;
    }
    
    if (!outputPath.empty()) {
        std::cout << "Saving to: " << outputPath << std::endl;
        if (!saveWav(audio, outputPath, SAMPLE_RATE)) {
            std::cerr << "Failed to save audio file" << std::endl;
            return false;
        }
    }
    
    std::cout << "Playing synthesized speech..." << std::endl;
    return playAudio(audio, SAMPLE_RATE);
}

std::vector<float> SpeechSynthesizer::loadVoiceEmbedding(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open voice model: " << path << std::endl;
        return {};
    }
    
    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    
    std::vector<float> embedding(size);
    for (uint32_t i = 0; i < size; ++i) {
        file.read(reinterpret_cast<char*>(&embedding[i]), sizeof(float));
    }
    
    std::cout << "Loaded voice embedding: " << size << " dimensions" << std::endl;
    return embedding;
}

std::vector<int> SpeechSynthesizer::textToTokens(const std::string& text) {
    std::vector<int> tokens;
    
    for (char c : text) {
        if (c >= 'a' && c <= 'z') {
            tokens.push_back(c - 'a' + 1);
        } else if (c >= 'A' && c <= 'Z') {
            tokens.push_back(c - 'A' + 1);
        } else if (c == ' ') {
            tokens.push_back(27);
        } else if (c == '.') {
            tokens.push_back(28);
        } else if (c == ',') {
            tokens.push_back(29);
        } else if (c == '!') {
            tokens.push_back(30);
        } else if (c == '?') {
            tokens.push_back(31);
        } else {
            tokens.push_back(0);
        }
    }
    
    return tokens;
}

std::vector<float> SpeechSynthesizer::generateSpeech(const std::vector<int>& tokens, 
                                                   const std::vector<float>& voiceEmbedding) {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noise(0.0f, 0.1f);
    
    const int samplesPerToken = SAMPLE_RATE / 10;
    const int totalSamples = tokens.size() * samplesPerToken;
    
    std::vector<float> audio(totalSamples);
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        int token = tokens[i];
        int startSample = i * samplesPerToken;
        
        float baseFreq = 100.0f + token * 10.0f;
        
        if (!voiceEmbedding.empty()) {
            int embIdx = i % voiceEmbedding.size();
            baseFreq *= (1.0f + voiceEmbedding[embIdx] * 0.5f);
        }
        
        baseFreq = std::max(50.0f, std::min(500.0f, baseFreq));
        
        for (int j = 0; j < samplesPerToken; ++j) {
            if (startSample + j < audio.size()) {
                float t = float(j) / SAMPLE_RATE;
                
                float amp = 0.3f;
                if (token == 27) {
                    amp = 0.05f;
                } else if (token == 28 || token == 29) {
                    amp = 0.1f;
                }
                
                float fadeIn = std::min(1.0f, float(j) / (samplesPerToken * 0.1f));
                float fadeOut = std::min(1.0f, float(samplesPerToken - j) / (samplesPerToken * 0.1f));
                amp *= fadeIn * fadeOut;
                
                float sample = amp * std::sin(2.0f * M_PI * baseFreq * t);
                sample += amp * 0.3f * std::sin(2.0f * M_PI * baseFreq * 2.0f * t);
                sample += amp * 0.1f * std::sin(2.0f * M_PI * baseFreq * 3.0f * t);
                
                sample += noise(gen) * 0.02f;
                
                audio[startSample + j] = std::max(-1.0f, std::min(1.0f, sample));
            }
        }
    }
    
    for (int i = 1; i < audio.size() - 1; ++i) {
        audio[i] = 0.25f * audio[i-1] + 0.5f * audio[i] + 0.25f * audio[i+1];
    }
    
    return audio;
}

bool SpeechSynthesizer::saveWav(const std::vector<float>& audio, 
                               const std::string& path, 
                               int sampleRate) {
    SF_INFO info;
    info.samplerate = sampleRate;
    info.channels = 1;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    
    SNDFILE* file = sf_open(path.c_str(), SFM_WRITE, &info);
    if (!file) {
        std::cerr << "Failed to create output file: " << sf_strerror(nullptr) << std::endl;
        return false;
    }
    
    sf_write_float(file, audio.data(), audio.size());
    sf_close(file);
    
    return true;
}

bool SpeechSynthesizer::playAudio(const std::vector<float>& audio, int sampleRate) {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    
    PaStreamParameters outputParameters;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = 1;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;
    
    PaStream* stream;
    err = Pa_OpenStream(&stream,
                       nullptr,
                       &outputParameters,
                       sampleRate,
                       1024,
                       paClipOff,
                       nullptr,
                       nullptr);
    
    if (err != paNoError) {
        std::cerr << "Failed to open output stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return false;
    }
    
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return false;
    }
    
    const int chunkSize = 1024;
    for (size_t i = 0; i < audio.size(); i += chunkSize) {
        size_t samplesToWrite = std::min(chunkSize, (int)(audio.size() - i));
        
        err = Pa_WriteStream(stream, &audio[i], samplesToWrite);
        if (err != paNoError) {
            std::cerr << "Error writing to stream: " << Pa_GetErrorText(err) << std::endl;
            break;
        }
    }
    
    Pa_CloseStream(stream);
    Pa_Terminate();
    
    return err == paNoError;
}