#!/bin/bash

# Check if CMake is installed
if ! command -v cmake &>/dev/null; then
    echo "CMake is not installed. Installing..."
    sudo apt update
    sudo apt install -y cmake
else
    echo "CMake is already installed."
fi

if ! dpkg -l | grep -q libglfw3-dev; then
    echo "GLFW development libraries are not installed. Installing..."
    sudo apt update
    sudo apt-get install -y libglfw3 libglfw3-dev
else
    echo "GLFW development libraries are already installed."
fi


echo "Installation complete."
