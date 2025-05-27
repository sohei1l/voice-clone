# EchoTwin

Lightweight C++ voice cloning CLI that creates personalized text-to-speech from a 30-second recording.

## Features

- **Record**: Capture high-quality voice samples from microphone
- **Extract**: Generate mel-spectrograms and F0 features  
- **Train**: Create compact speaker-specific voice models
- **Synthesize**: Convert any text to speech in cloned voice
- **Export**: Save synthesized speech as WAV files for sharing

## Quick Start

```bash
# Run the complete demo
./examples/quickstart.sh
```

## Usage

```bash
# Record 30-second voice sample
./echotwin record [output.wav]

# Extract mel-spectrograms and F0 features
./echotwin featurize [input.wav]

# Train compact voice model from features
./echotwin train [mel.npy] [f0.npy] [voice.vec]

# Generate speech with cloned voice
./echotwin say "Hello world" [voice.vec] [output.wav]

# Export WAV file for sharing
./echotwin --export [voice.vec] "Your message" [output.wav]
```

## Build

### Quick Build
```bash
./scripts/build.sh
```

### Manual Build
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Cross-Platform Release
```bash
# Use CMake presets for optimized builds
cmake --preset macos-release    # macOS universal binary
cmake --preset linux-release    # Linux static binary  
cmake --preset windows-release  # Windows executable
```

## Requirements

- CMake 3.16+
- PortAudio
- libsndfile
- Eigen3
- ONNX Runtime