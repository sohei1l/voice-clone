#pragma once
#include <string>
#include <vector>

class SpeechSynthesizer {
public:
    static bool synthesize(const std::string& text, 
                          const std::string& voiceModelPath,
                          const std::string& outputPath = "");
    
    static bool playAudio(const std::vector<float>& audio, int sampleRate);
    
private:
    static std::vector<float> loadVoiceEmbedding(const std::string& path);
    static std::vector<int> textToTokens(const std::string& text);
    static std::vector<float> generateSpeech(const std::vector<int>& tokens, 
                                           const std::vector<float>& voiceEmbedding);
    static bool saveWav(const std::vector<float>& audio, 
                       const std::string& path, 
                       int sampleRate);
};