# Build Instructions

This document provides detailed instructions for building the Xeno Software Suite on different platforms.

## Quick Start

### Automated Setup (Recommended)

#### Linux/macOS
```bash
./scripts/setup.sh
```

#### Windows
```cmd
.\scripts\setup.bat
```

The setup scripts will:
1. Install system dependencies
2. Set up vcpkg package manager
3. Install all required libraries
4. Configure and build the project
5. Run tests to verify the build

## Manual Build Process

If you prefer to build manually or the automated setup doesn't work for your system:

### Prerequisites

- **CMake 3.20+**
- **Qt 6.5+** with Widgets, Multimedia, and WebEngine modules
- **C++20** compatible compiler
- **vcpkg** package manager

### Dependencies

The project uses vcpkg for dependency management. Required packages:

- `qt6[widgets,multimedia,quick,webchannel,webengine]`
- `opencv4`
- `ffmpeg[avcodec,avformat,swscale]`
- `portaudio`
- `libsndfile`
- `cpp-httplib`
- `nlohmann-json`
- `gtest`

### Build Steps

1. **Clone and setup vcpkg:**
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # Linux/macOS
   # or
   .\bootstrap-vcpkg.bat  # Windows
   ```

2. **Install dependencies:**
   ```bash
   ./vcpkg install --triplet=x64-linux    # Linux
   ./vcpkg install --triplet=x64-osx      # macOS
   ./vcpkg install --triplet=x64-windows  # Windows
   ```

3. **Configure project:**
   ```bash
   cmake -S . -B build \
     -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
     -DCMAKE_BUILD_TYPE=Release
   ```

4. **Build:**
   ```bash
   cmake --build build --parallel
   ```

5. **Test:**
   ```bash
   cd build && ctest --output-on-failure
   ```

## Platform-Specific Notes

### Linux (Ubuntu/Debian)

Install system dependencies:
```bash
sudo apt update
sudo apt install -y \
  build-essential cmake git curl zip unzip tar \
  pkg-config ninja-build qt6-base-dev qt6-multimedia-dev \
  qt6-webengine-dev libgl1-mesa-dev
```

### macOS

Install Xcode command line tools:
```bash
xcode-select --install
```

Install dependencies with Homebrew:
```bash
brew install cmake git ninja qt@6
```

### Windows

Requirements:
- Visual Studio 2019/2022 or Build Tools for Visual Studio
- Qt 6.5+ (installer from qt.io)
- Git for Windows

## Troubleshooting

### Common Issues

#### Qt6 Not Found
Ensure Qt6 is installed and CMAKE_PREFIX_PATH includes Qt installation:
```bash
export CMAKE_PREFIX_PATH="/path/to/qt6:$CMAKE_PREFIX_PATH"
```

#### vcpkg Dependencies Fail
Try updating vcpkg and rebuilding:
```bash
cd vcpkg
git pull
./bootstrap-vcpkg.sh  # or .bat on Windows
```

#### Compiler Errors
Ensure you have C++20 support:
- GCC 10+
- Clang 12+
- MSVC 2019 16.11+

#### Missing System Libraries (Linux)
Install development packages:
```bash
# Additional libraries that might be needed
sudo apt install -y \
  libxkbcommon-x11-0 libxcb-icccm4 libxcb-image0 \
  libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 \
  libxcb-xinerama0 libfontconfig1-dev libfreetype6-dev \
  libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev \
  libxi-dev libxrender-dev libxcb1-dev libxcb-glx0-dev \
  libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev \
  libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev \
  libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev \
  libxcb-util-dev libxcb-xinerama0-dev libxcb-xkb-dev \
  libxkbcommon-dev libxkbcommon-x11-dev
```

### Build Configurations

#### Debug Build
```bash
cmake -S . -B build-debug \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Debug
```

#### Release Build
```bash
cmake -S . -B build-release \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

#### With Custom Qt Location
```bash
cmake -S . -B build \
  -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_PREFIX_PATH="/path/to/qt6" \
  -DCMAKE_BUILD_TYPE=Release
```

## Development Workflow

### Building Individual Applications

```bash
# Build only the launcher
cmake --build build --target xeno-launcher

# Build only shared libraries
cmake --build build --target ai-integration utils

# Build and run tests
cmake --build build --target test_shared_libraries
./build/tests/unit/test_shared_libraries
```

### Running Applications

After building, applications are located in:
- `build/apps/launcher/xeno-launcher`
- `build/apps/image-edit/xeno-image-edit`
- `build/apps/video-edit/xeno-video-edit`
- `build/apps/audio-edit/xeno-audio-edit`
- `build/apps/xeno-code/xeno-code`

### Configuration

Create `build/config.json` with your API keys:
```json
{
  "xeno_ai": {
    "endpoint": "https://api.xenolabs.ai",
    "api_key": "your_api_key"
  },
  "open_router": {
    "endpoint": "https://openrouter.ai/api/v1", 
    "api_key": "your_openrouter_key"
  },
  "ollama": {
    "endpoint": "http://localhost:11434"
  }
}
```

## IDE Setup

### Visual Studio Code

Recommended extensions:
- C/C++ Extension Pack
- CMake Tools
- Qt tools

Settings in `.vscode/settings.json`:
```json
{
  "cmake.configureArgs": [
    "-DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake"
  ],
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

### Qt Creator

1. Open CMakeLists.txt as project
2. Set CMake configuration:
   - CMAKE_TOOLCHAIN_FILE: `/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`
   - CMAKE_BUILD_TYPE: `Release` or `Debug`

### Visual Studio (Windows)

1. Use "Open Folder" to open the project root
2. CMake should auto-configure with CMakeSettings.json:
```json
{
  "configurations": [
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "installRoot": "${projectDir}\\out\\install\\${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "cmakeToolchain": "vcpkg/scripts/buildsystems/vcpkg.cmake"
    }
  ]
}
```

## Performance Tips

- Use Ninja generator for faster builds: `-G Ninja`
- Enable parallel builds: `--parallel` or `-j$(nproc)`
- Use ccache for faster rebuilds
- Build in Release mode for performance testing
- Use unity builds for faster compilation: `-DCMAKE_UNITY_BUILD=ON`