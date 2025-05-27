#pragma once
#include <string>
#include <vector>
#include <Eigen/Dense>

class VoiceTrainer {
public:
    static bool trainEncoder(const std::string& melFeaturesPath, 
                           const std::string& f0FeaturesPath,
                           const std::string& outputModelPath);
    
private:
    static Eigen::MatrixXf loadNpyMatrix(const std::string& path);
    static std::vector<float> loadNpyVector(const std::string& path);
    static bool runTrainingLoop(const Eigen::MatrixXf& melFeatures, 
                               const std::vector<float>& f0Features,
                               const std::string& outputPath);
    static std::vector<float> extractSpeakerEmbedding(const Eigen::MatrixXf& melFeatures);
};