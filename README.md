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

# Extract mel-spectrograms and F0 features
./echotwin featurize [input.wav]

# Train compact voice model from features
./echotwin train [mel.npy] [f0.npy] [voice.vec]

# Generate speech with cloned voice
./echotwin say "Hello world" [voice.vec] [output.wav]
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
- ONNX Runtime