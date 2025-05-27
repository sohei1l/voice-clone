#!/bin/bash

set -e

echo "Building EchoTwin for multiple platforms..."

# Create release directory
mkdir -p release

# Detect platform and build
case "$(uname)" in
    Darwin)
        echo "Building for macOS..."
        cmake --preset macos-release
        cmake --build build/macos --config Release
        cp build/macos/echotwin release/echotwin-macos
        ;;
    Linux)
        echo "Building for Linux..."
        cmake --preset linux-release  
        cmake --build build/linux --config Release
        cp build/linux/echotwin release/echotwin-linux
        ;;
    MINGW*|MSYS*|CYGWIN*)
        echo "Building for Windows..."
        cmake --preset windows-release
        cmake --build build/windows --config Release
        cp build/windows/Release/echotwin.exe release/echotwin-windows.exe
        ;;
    *)
        echo "Unsupported platform: $(uname)"
        exit 1
        ;;
esac

echo "Build completed! Binary available in release/"
ls -la release/