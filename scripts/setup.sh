#!/bin/bash

# Xeno Software Suite - Development Environment Setup Script
# This script sets up the development environment for Linux and macOS

set -e

echo "🚀 Setting up Xeno Software Suite development environment..."

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "vcpkg.json" ]; then
    echo "❌ Error: Please run this script from the root of the xeno-software-suite repository"
    exit 1
fi

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check system requirements
echo "📋 Checking system requirements..."

# Check for CMake
if ! command_exists cmake; then
    echo "❌ CMake is not installed. Please install CMake 3.20 or higher."
    echo "   Ubuntu/Debian: sudo apt install cmake"
    echo "   macOS: brew install cmake"
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | grep -o '[0-9]\+\.[0-9]\+' | head -n1)
echo "✅ CMake $CMAKE_VERSION found"

# Check for Git
if ! command_exists git; then
    echo "❌ Git is not installed. Please install Git first."
    exit 1
fi
echo "✅ Git found"

# Check for C++ compiler
if command_exists g++; then
    COMPILER="g++"
    COMPILER_VERSION=$(g++ --version | head -n1)
elif command_exists clang++; then
    COMPILER="clang++"
    COMPILER_VERSION=$(clang++ --version | head -n1)
else
    echo "❌ No C++ compiler found. Please install g++ or clang++."
    echo "   Ubuntu/Debian: sudo apt install build-essential"
    echo "   macOS: xcode-select --install"
    exit 1
fi
echo "✅ $COMPILER found: $COMPILER_VERSION"

# Platform-specific setup
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="linux"
    echo "🐧 Detected Linux platform"
    
    # Check for required libraries
    echo "📦 Installing system dependencies..."
    
    if command_exists apt; then
        # Ubuntu/Debian
        sudo apt update
        sudo apt install -y \
            build-essential \
            cmake \
            git \
            curl \
            zip \
            unzip \
            tar \
            pkg-config \
            ninja-build \
            qt6-base-dev \
            qt6-multimedia-dev \
            qt6-webengine-dev \
            libgl1-mesa-dev \
            libxkbcommon-x11-0 \
            libxcb-icccm4 \
            libxcb-image0 \
            libxcb-keysyms1 \
            libxcb-randr0 \
            libxcb-render-util0 \
            libxcb-xinerama0
            
    elif command_exists yum; then
        # RHEL/CentOS/Fedora
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y cmake git curl zip unzip tar pkg-config ninja-build
    else
        echo "⚠️  Unknown Linux distribution. Please install development tools manually."
    fi
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="osx"
    echo "🍎 Detected macOS platform"
    
    # Check for Xcode command line tools
    if ! xcode-select -p &> /dev/null; then
        echo "📦 Installing Xcode command line tools..."
        xcode-select --install
        echo "⏳ Please complete the Xcode installation and run this script again."
        exit 1
    fi
    
    # Check for Homebrew
    if ! command_exists brew; then
        echo "📦 Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    # Install dependencies
    echo "📦 Installing dependencies with Homebrew..."
    brew install cmake git ninja qt@6
    
else
    echo "❌ Unsupported platform: $OSTYPE"
    echo "This script supports Linux and macOS only."
    exit 1
fi

# Setup vcpkg
echo "📦 Setting up vcpkg package manager..."

if [ ! -d "vcpkg" ]; then
    echo "⬇️  Cloning vcpkg..."
    git clone https://github.com/Microsoft/vcpkg.git
else
    echo "✅ vcpkg directory already exists"
fi

cd vcpkg

# Bootstrap vcpkg
if [ ! -f "vcpkg" ]; then
    echo "🔨 Bootstrapping vcpkg..."
    ./bootstrap-vcpkg.sh
else
    echo "✅ vcpkg already bootstrapped"
fi

cd ..

# Set environment variables
export VCPKG_ROOT="$(pwd)/vcpkg"
export PATH="$VCPKG_ROOT:$PATH"

echo "🔧 Installing project dependencies..."

# Install dependencies using vcpkg
if [[ "$PLATFORM" == "linux" ]]; then
    TRIPLET="x64-linux"
elif [[ "$PLATFORM" == "osx" ]]; then
    TRIPLET="x64-osx"
fi

echo "📦 Installing vcpkg packages for $TRIPLET..."
./vcpkg/vcpkg install --triplet=$TRIPLET

# Create build directory
echo "📁 Creating build directory..."
mkdir -p build

# Configure the project
echo "🔧 Configuring project with CMake..."
cmake -S . -B build \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -G Ninja

echo "🔨 Building project..."
cmake --build build --parallel

# Create sample configuration
echo "⚙️  Creating sample configuration..."
cat > build/config.json << 'EOF'
{
  "xeno_ai": {
    "endpoint": "https://api.xenolabs.ai",
    "api_key": "your_xeno_api_key_here"
  },
  "open_router": {
    "endpoint": "https://openrouter.ai/api/v1",
    "api_key": "your_openrouter_key_here"
  },
  "ollama": {
    "endpoint": "http://localhost:11434"
  }
}
EOF

# Create environment setup script
cat > build/env.sh << EOF
#!/bin/bash
# Source this file to set up your development environment
export VCPKG_ROOT="$(pwd)/../vcpkg"
export PATH="\$VCPKG_ROOT:\$PATH"
export CMAKE_TOOLCHAIN_FILE="\$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"

echo "Environment configured for Xeno Software Suite development"
echo "VCPKG_ROOT: \$VCPKG_ROOT"
EOF

chmod +x build/env.sh

# Run tests
echo "🧪 Running tests..."
cd build
ctest --output-on-failure --parallel
cd ..

echo ""
echo "🎉 Setup complete!"
echo ""
echo "📋 Next steps:"
echo "   1. Edit build/config.json with your API keys"
echo "   2. Run: source build/env.sh (to set up environment)"
echo "   3. Run: ./build/apps/launcher/xeno-launcher (to start the launcher)"
echo ""
echo "🔧 Development commands:"
echo "   • Build:     cmake --build build --parallel"
echo "   • Test:      cd build && ctest --output-on-failure"
echo "   • Clean:     cmake --build build --target clean"
echo ""
echo "📖 See CONTRIBUTING.md for detailed development guidelines"
echo "🌟 Happy coding with Xeno Software Suite!"