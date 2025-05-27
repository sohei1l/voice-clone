#!/bin/bash

echo "EchoTwin Quick Start Demo"
echo "========================"

# Check if echotwin exists
if ! command -v ./echotwin &> /dev/null; then
    echo "Building echotwin..."
    ./scripts/build.sh
    cp release/echotwin-* ./echotwin 2>/dev/null || echo "Using build output"
fi

echo "Step 1: Recording voice sample (30 seconds)..."
echo "Please speak clearly when recording starts."
read -p "Press Enter to start recording..."
./echotwin record demo_voice.wav

echo "Step 2: Extracting features..."
./echotwin featurize demo_voice.wav

echo "Step 3: Training voice model..."  
./echotwin train mel_features.npy f0_features.npy demo_voice.vec

echo "Step 4: Testing synthesis..."
./echotwin say "Hello! This is my cloned voice speaking." demo_voice.vec

echo "Step 5: Exporting sample..."
./echotwin --export demo_voice.vec "Welcome to EchoTwin voice cloning!" welcome.wav

echo ""
echo "Demo complete! Your voice model is saved as 'demo_voice.vec'"
echo "Try: ./echotwin say \"Your text here\" demo_voice.vec"