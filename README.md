# Xeno Software Suite

Monorepo for Xeno Software Suite: AI-enhanced creative and coding tools integrated with Xeno Labs platform for credit-based AI usage.

## Overview

The Xeno Software Suite is a comprehensive collection of AI-enhanced applications similar to Adobe Creative Cloud, designed to work seamlessly with the Xeno Labs platform. Users can access powerful creative tools that leverage cloud-based AI models through a credit-based system.

## Applications

- **Image Edit**: AI-powered image editor with generative fill and object removal
- **Video Edit**: Advanced video editor with auto-editing and stabilization features  
- **Audio Edit**: Professional audio editing with voice cloning and noise reduction
- **Xeno Code**: AI-assisted IDE with code suggestions and refactoring
- **Launcher**: Central hub for managing and launching applications

## Platform Integration

All applications integrate with:
- **Xeno AI Cloud**: Credit-based AI model access
- **Open Router API**: Third-party model integration
- **Ollama**: Local LLM support for offline mode
- **Xeno Labs**: User authentication and credit wallet management

## Tech Stack

- **Language**: C++20
- **GUI Framework**: Qt 6
- **Build System**: CMake
- **Package Manager**: vcpkg
- **Testing**: Google Test
- **CI/CD**: GitHub Actions

## Building

```bash
# Clone with vcpkg
git clone https://github.com/emiliancristea/xeno-software-suite.git
cd xeno-software-suite

# Setup vcpkg (first time only)
./scripts/setup.sh  # Linux/macOS
# or
./scripts/setup.bat  # Windows

# Build
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Configuration

Create a `config.json` file in your build directory with your API keys:

```json
{
  "xeno_ai": {
    "endpoint": "https://api.xenolabs.ai",
    "api_key": "your_xeno_api_key"
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

## License

MIT License - see [LICENSE](LICENSE) file for details.
