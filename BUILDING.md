# How to build the project

## Requirements
- CMake

## Clone the repository
```bash
git clone https://github.com/CuriousTama/AudioPlayer.git
```

## Build
### 1. Create the build directory and run cmake

Open the terminal from the repository as root and run:

```bash
mkdir build 
cd build
cmake ..
```
If you need to have examples, you can add `-DBUILD_EXAMPLES=ON` to the cmake command: `cmake .. -DBUILD_EXAMPLES=ON`

### 2. Build
```bash
cmake --build .
```

Or open it in you IDE and build it, it should be located in `build/`.