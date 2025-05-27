#pragma once
#include <string>

class AudioRecorder {
public:
    static bool record(const std::string& outputPath);
};