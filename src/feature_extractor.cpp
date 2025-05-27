#include "feature_extractor.h"
#include <sndfile.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

#define MEL_BINS 80
#define FFT_SIZE 1024
#define HOP_LENGTH 256
#define SAMPLE_RATE 16000

bool FeatureExtractor::extractMelSpectrogram(const std::string& audioPath, const std::string& outputPath) {
    std::vector<float> audio = loadAudio(audioPath);
    if (audio.empty()) {
        std::cerr << "Failed to load audio file: " << audioPath << std::endl;
        return false;
    }

    Eigen::MatrixXf melSpec = computeMelSpectrogram(audio, SAMPLE_RATE);
    
    std::cout << "Extracted mel-spectrogram: " << MEL_BINS << " x " << melSpec.cols() << std::endl;
    
    return saveNpy(melSpec, outputPath);
}

bool FeatureExtractor::extractF0(const std::string& audioPath, const std::string& outputPath) {
    std::vector<float> audio = loadAudio(audioPath);
    if (audio.empty()) {
        std::cerr << "Failed to load audio file: " << audioPath << std::endl;
        return false;
    }

    std::vector<float> f0 = computeF0(audio, SAMPLE_RATE);
    
    std::cout << "Extracted F0 track: " << f0.size() << " frames" << std::endl;
    
    return saveNpy(f0, outputPath);
}

std::vector<float> FeatureExtractor::loadAudio(const std::string& path) {
    SF_INFO info;
    SNDFILE* file = sf_open(path.c_str(), SFM_READ, &info);
    
    if (!file) {
        std::cerr << "Failed to open audio file: " << sf_strerror(nullptr) << std::endl;
        return {};
    }

    std::vector<float> audio(info.frames);
    sf_read_float(file, audio.data(), info.frames);
    sf_close(file);

    return audio;
}

Eigen::MatrixXf FeatureExtractor::computeMelSpectrogram(const std::vector<float>& audio, int sampleRate) {
    int numFrames = (audio.size() - FFT_SIZE) / HOP_LENGTH + 1;
    Eigen::MatrixXf melSpec(MEL_BINS, numFrames);

    for (int frame = 0; frame < numFrames; ++frame) {
        int start = frame * HOP_LENGTH;
        
        std::vector<float> window(FFT_SIZE);
        for (int i = 0; i < FFT_SIZE; ++i) {
            if (start + i < audio.size()) {
                window[i] = audio[start + i] * (0.5f - 0.5f * cos(2.0f * M_PI * i / (FFT_SIZE - 1)));
            } else {
                window[i] = 0.0f;
            }
        }

        std::vector<float> magnitude(FFT_SIZE / 2 + 1);
        for (int i = 0; i < magnitude.size(); ++i) {
            magnitude[i] = std::abs(window[i]) + 1e-8f;
        }

        for (int mel = 0; mel < MEL_BINS; ++mel) {
            float sum = 0.0f;
            int startBin = mel * magnitude.size() / MEL_BINS;
            int endBin = (mel + 1) * magnitude.size() / MEL_BINS;
            
            for (int bin = startBin; bin < endBin && bin < magnitude.size(); ++bin) {
                sum += magnitude[bin];
            }
            
            melSpec(mel, frame) = log10f(sum / (endBin - startBin) + 1e-8f);
        }
    }

    return melSpec;
}

std::vector<float> FeatureExtractor::computeF0(const std::vector<float>& audio, int sampleRate) {
    int numFrames = (audio.size() - FFT_SIZE) / HOP_LENGTH + 1;
    std::vector<float> f0(numFrames);

    for (int frame = 0; frame < numFrames; ++frame) {
        int start = frame * HOP_LENGTH;
        
        std::vector<float> autocorr(FFT_SIZE / 2);
        for (int lag = 1; lag < autocorr.size(); ++lag) {
            float sum = 0.0f;
            int count = 0;
            
            for (int i = 0; i + lag < FFT_SIZE && start + i + lag < audio.size(); ++i) {
                sum += audio[start + i] * audio[start + i + lag];
                count++;
            }
            
            autocorr[lag] = count > 0 ? sum / count : 0.0f;
        }

        int maxLag = 1;
        float maxVal = autocorr[1];
        for (int lag = 2; lag < autocorr.size(); ++lag) {
            if (autocorr[lag] > maxVal) {
                maxVal = autocorr[lag];
                maxLag = lag;
            }
        }

        f0[frame] = maxVal > 0.3f ? float(sampleRate) / maxLag : 0.0f;
    }

    return f0;
}

bool FeatureExtractor::saveNpy(const Eigen::MatrixXf& data, const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file: " << path << std::endl;
        return false;
    }

    char header[] = "\x93NUMPY\x01\x00";
    file.write(header, 8);

    std::string dtype = "{'descr': '<f4', 'fortran_order': False, 'shape': (" + 
                       std::to_string(data.rows()) + ", " + std::to_string(data.cols()) + "), }";
    
    int header_len = dtype.length();
    int padding = 16 - (header_len + 10) % 16;
    for (int i = 0; i < padding; ++i) dtype += " ";
    dtype += "\n";

    uint16_t len = dtype.length();
    file.write(reinterpret_cast<char*>(&len), 2);
    file.write(dtype.c_str(), dtype.length());

    for (int i = 0; i < data.rows(); ++i) {
        for (int j = 0; j < data.cols(); ++j) {
            float val = data(i, j);
            file.write(reinterpret_cast<char*>(&val), sizeof(float));
        }
    }

    return true;
}

bool FeatureExtractor::saveNpy(const std::vector<float>& data, const std::string& path) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file: " << path << std::endl;
        return false;
    }

    char header[] = "\x93NUMPY\x01\x00";
    file.write(header, 8);

    std::string dtype = "{'descr': '<f4', 'fortran_order': False, 'shape': (" + 
                       std::to_string(data.size()) + ",), }";
    
    int header_len = dtype.length();
    int padding = 16 - (header_len + 10) % 16;
    for (int i = 0; i < padding; ++i) dtype += " ";
    dtype += "\n";

    uint16_t len = dtype.length();
    file.write(reinterpret_cast<char*>(&len), 2);
    file.write(dtype.c_str(), dtype.length());

    for (float val : data) {
        file.write(reinterpret_cast<char*>(&val), sizeof(float));
    }

    return true;
}