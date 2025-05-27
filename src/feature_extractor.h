#pragma once
#include <string>
#include <vector>
#include <Eigen/Dense>

class FeatureExtractor {
public:
    static bool extractMelSpectrogram(const std::string& audioPath, const std::string& outputPath);
    static bool extractF0(const std::string& audioPath, const std::string& outputPath);
    
private:
    static std::vector<float> loadAudio(const std::string& path);
    static Eigen::MatrixXf computeMelSpectrogram(const std::vector<float>& audio, int sampleRate);
    static std::vector<float> computeF0(const std::vector<float>& audio, int sampleRate);
    static bool saveNpy(const Eigen::MatrixXf& data, const std::string& path);
    static bool saveNpy(const std::vector<float>& data, const std::string& path);
};