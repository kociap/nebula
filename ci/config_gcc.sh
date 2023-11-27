#!/bin/bash

sudo apt-get update

sudo apt-get install -y cmake

# GLFW dependencies.
sudo apt-get install -y libxinerama-dev libxcursor-dev libxi-dev libxrandr-dev libgl1-mesa-dev

echo "Installation complete."
