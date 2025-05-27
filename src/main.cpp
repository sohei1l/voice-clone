#include <iostream>
#include <string>
#include <vector>
#include "audio_recorder.h"
#include "feature_extractor.h"
#include "voice_trainer.h"
#include "speech_synthesizer.h"

void showUsage() {
    std::cout << "echotwin - Lightweight Voice Cloning CLI\n\n";
    std::cout << "Usage:\n";
    std::cout << "  echotwin record [output.wav]        - Record 30 seconds from microphone\n";
    std::cout << "  echotwin featurize [input.wav]      - Extract features from audio\n";
    std::cout << "  echotwin train [mel] [f0] [voice]   - Train voice model\n";
    std::cout << "  echotwin say <text> [voice] [out]   - Synthesize speech\n";
    std::cout << "  echotwin --export [voice] [text]    - Export WAV file\n";
    std::cout << "  echotwin --help                     - Show this help\n";
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

    if (command == "--export") {
        std::string voiceModel = "voice.vec";
        std::string text = "Hello world";
        std::string outputFile = "export.wav";
        
        if (argc >= 3) voiceModel = argv[2];
        if (argc >= 4) text = argv[3];
        if (argc >= 5) outputFile = argv[4];
        
        std::cout << "Exporting speech to WAV file..." << std::endl;
        
        if (SpeechSynthesizer::synthesize(text, voiceModel, outputFile)) {
            std::cout << "Export completed: " << outputFile << std::endl;
            return 0;
        } else {
            std::cout << "Export failed\n";
            return 1;
        }
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
        std::string audioFile = "voice_sample.wav";
        if (argc >= 3) {
            audioFile = argv[2];
        }
        
        std::cout << "Extracting features from: " << audioFile << std::endl;
        
        bool success = true;
        success &= FeatureExtractor::extractMelSpectrogram(audioFile, "mel_features.npy");
        success &= FeatureExtractor::extractF0(audioFile, "f0_features.npy");
        
        if (success) {
            std::cout << "Feature extraction completed successfully\n";
            return 0;
        } else {
            std::cout << "Feature extraction failed\n";
            return 1;
        }
    } else if (command == "train") {
        std::string melFile = "mel_features.npy";
        std::string f0File = "f0_features.npy";
        std::string outputFile = "voice.vec";
        
        if (argc >= 3) melFile = argv[2];
        if (argc >= 4) f0File = argv[3];
        if (argc >= 5) outputFile = argv[4];
        
        std::cout << "Training voice encoder..." << std::endl;
        
        if (VoiceTrainer::trainEncoder(melFile, f0File, outputFile)) {
            std::cout << "Training completed successfully\n";
            return 0;
        } else {
            std::cout << "Training failed\n";
            return 1;
        }
    } else if (command == "say") {
        if (argc < 3) {
            std::cout << "Error: Please provide text to synthesize\n";
            return 1;
        }
        
        std::string text = argv[2];
        std::string voiceModel = "voice.vec";
        std::string outputFile = "";
        
        if (argc >= 4) voiceModel = argv[3];
        if (argc >= 5) outputFile = argv[4];
        
        if (SpeechSynthesizer::synthesize(text, voiceModel, outputFile)) {
            std::cout << "Speech synthesis completed successfully\n";
            return 0;
        } else {
            std::cout << "Speech synthesis failed\n";
            return 1;
        }
    } else {
        std::cout << "Unknown command: " << command << "\n";
        showUsage();
        return 1;
    }

    return 0;
}