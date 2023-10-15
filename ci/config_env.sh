#!/bin/bash

sudo apt-get update

# Check if CMake is installed.
if ! command -v cmake &>/dev/null; then
    echo "CMake is not installed. Installing..."
    sudo apt-get install -y cmake
else
    echo "CMake is already installed."
fi

# Check if Clang is installed.
if ! command -v clang &>/dev/null; then
    echo "Clang is not installed. Installing..."
    sudo apt-get install -y clang
else
    echo "Clang is already installed."
fi

# GLFW dependencies.
sudo apt-get install -y libxinerama-dev libxcursor-dev libxi-dev libxrandr-dev

echo "Installation complete."
