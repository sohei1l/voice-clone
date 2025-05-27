# EchoTwin

Lightweight C++ voice cloning CLI that creates personalized text-to-speech from a 30-second recording.

## Features

- **Record**: Capture high-quality voice samples
- **Train**: Generate compact voice models
- **Synthesize**: Convert text to speech in your voice

## Usage

```bash
# Record voice sample
./echotwin record

# Extract features  
./echotwin featurize

# Train voice model
./echotwin train

# Generate speech
./echotwin say "Hello world"
```

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Requirements

- CMake 3.16+
- PortAudio
- libsndfile
- Eigen3