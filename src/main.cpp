#include <iostream>
#include <string>
#include <vector>
#include "audio_recorder.h"

void showUsage() {
    std::cout << "echotwin - Lightweight Voice Cloning CLI\n\n";
    std::cout << "Usage:\n";
    std::cout << "  echotwin record     - Record 30 seconds from microphone\n";
    std::cout << "  echotwin featurize  - Extract features from audio\n";
    std::cout << "  echotwin train      - Train voice model\n";
    std::cout << "  echotwin say <text> - Synthesize speech\n";
    std::cout << "  echotwin --help     - Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        showUsage();
        return 1;
    }

    std::string command = argv[1];

    if (command == "--help" || command == "-h") {
        showUsage();
        return 0;
    }

    if (command == "record") {
        std::string outputFile = "voice_sample.wav";
        if (argc >= 3) {
            outputFile = argv[2];
        }
        
        if (AudioRecorder::record(outputFile)) {
            std::cout << "Recording completed successfully\n";
            return 0;
        } else {
            std::cout << "Recording failed\n";
            return 1;
        }
    } else if (command == "featurize") {
        std::cout << "Feature extraction not yet implemented\n";
        return 1;
    } else if (command == "train") {
        std::cout << "Training functionality not yet implemented\n";
        return 1;
    } else if (command == "say") {
        if (argc < 3) {
            std::cout << "Error: Please provide text to synthesize\n";
            return 1;
        }
        std::cout << "Speech synthesis not yet implemented\n";
        return 1;
    } else {
        std::cout << "Unknown command: " << command << "\n";
        showUsage();
        return 1;
    }

    return 0;
}