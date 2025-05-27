# EchoTwin

Lightweight C++ voice cloning CLI that creates personalized text-to-speech from a 30-second recording.

## Features

- **Record**: Capture high-quality voice samples
- **Train**: Generate compact voice models
- **Synthesize**: Convert text to speech in your voice

## Usage

```bash
# Record 30-second voice sample
./echotwin record [output.wav]

# Extract features (coming soon)
./echotwin featurize

# Train voice model (coming soon)
./echotwin train

# Generate speech (coming soon)
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