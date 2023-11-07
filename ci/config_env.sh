#!/bin/bash

sudo apt-get update

# Check if CMake is installed.
if ! command -v cmake &>/dev/null; then
    echo "CMake is not installed. Installing..."
    sudo apt-get update
    sudo apt-get install -y cmake
else
    echo "CMake is already installed."
fi

# GLFW dependencies.
sudo apt-get install -y libxinerama-dev libxcursor-dev libxi-dev libxrandr-dev libgl1-mesa-dev

echo "Installation complete."
