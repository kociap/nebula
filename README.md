[![g++](https://github.com/kociap/nebula/actions/workflows/g++.yaml/badge.svg?branch=main)](https://github.com/kociap/nebula/actions/workflows/g++.yaml)
[![clang](https://github.com/kociap/nebula/actions/workflows/clang.yaml/badge.svg?branch=main)](https://github.com/kociap/nebula/actions/workflows/clang.yaml)

# Nebula - OpenGL C++ CAD Tool for Logic Circuits

## Overview

Nebula is an open-source C++ CAD tool designed for working with logic circuits using OpenGL.

## Features

- Intuitive user interface
- OpenGL rendering for efficient graphics
- Logic circuit design and simulation

## Getting Started

### Prerequisites

- g++/clang++
- OpenGL library
- GLFW3
- imGUI

### Building and Running

```bash
git clone https://github.com/kociap/nebula.git
cd nebula
mkdir builD
cmake -B build -DCMAKE_CXX_COMPILER="g++"
./build/make
./build/nebula
