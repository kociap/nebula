#!/bin/bash

# Add LLVM repository.
source /etc/os-release
wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/llvm.asc > /dev/null
echo "deb [arch=amd64] http://apt.llvm.org/${UBUNTU_CODENAME}/ llvm-toolchain-${UBUNTU_CODENAME} main" | sudo tee /etc/apt/sources.list.d/llvm.list > /dev/null

sudo apt-get update

# Install clang and its tools.
sudo apt-get -y install llvm clang clangd clang-format clang-tidy clang-tools lld

# Install cmake just in case.
sudo apt-get -y install cmake

# GLFW dependencies.
sudo apt-get install -y libxinerama-dev libxcursor-dev libxi-dev libxrandr-dev libgl1-mesa-dev

echo "Installation complete."
