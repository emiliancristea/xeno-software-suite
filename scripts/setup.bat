@echo off
REM Xeno Software Suite - Development Environment Setup Script (Windows)
REM This script sets up the development environment for Windows

setlocal enabledelayedexpansion

echo 🚀 Setting up Xeno Software Suite development environment...

REM Check if we're in the right directory
if not exist "CMakeLists.txt" (
    echo ❌ Error: Please run this script from the root of the xeno-software-suite repository
    exit /b 1
)

if not exist "vcpkg.json" (
    echo ❌ Error: Please run this script from the root of the xeno-software-suite repository
    exit /b 1
)

echo 📋 Checking system requirements...

REM Check for CMake
cmake --version >nul 2>&1
if errorlevel 1 (
    echo ❌ CMake is not installed. Please install CMake 3.20 or higher.
    echo    Download from: https://cmake.org/download/
    exit /b 1
)

for /f "tokens=3" %%i in ('cmake --version ^| findstr /r "cmake version"') do set CMAKE_VERSION=%%i
echo ✅ CMake %CMAKE_VERSION% found

REM Check for Git
git --version >nul 2>&1
if errorlevel 1 (
    echo ❌ Git is not installed. Please install Git first.
    echo    Download from: https://git-scm.com/download/windows
    exit /b 1
)
echo ✅ Git found

REM Check for Visual Studio Build Tools
where cl >nul 2>&1
if errorlevel 1 (
    echo ❌ Visual Studio C++ compiler not found.
    echo    Please install Visual Studio 2019/2022 or Build Tools for Visual Studio
    echo    Download from: https://visualstudio.microsoft.com/downloads/
    exit /b 1
)
echo ✅ Visual Studio C++ compiler found

REM Check for Qt
echo 📦 Checking for Qt installation...
set QT_FOUND=0

REM Common Qt installation paths
set QT_PATHS=C:\Qt\6.5.0\msvc2019_64;C:\Qt\6.5.0\msvc2022_64;C:\Qt\6.6.0\msvc2019_64;C:\Qt\6.6.0\msvc2022_64

for %%p in (%QT_PATHS%) do (
    if exist "%%p\bin\qmake.exe" (
        set QT_DIR=%%p
        set QT_FOUND=1
        echo ✅ Qt found at %%p
        goto qt_found
    )
)

:qt_found
if %QT_FOUND%==0 (
    echo ⚠️ Qt 6 not found in common locations. You may need to install it manually.
    echo    Download from: https://www.qt.io/download-qt-installer
    echo    Or use vcpkg to install Qt packages
)

REM Setup vcpkg
echo 📦 Setting up vcpkg package manager...

if not exist "vcpkg" (
    echo ⬇️ Cloning vcpkg...
    git clone https://github.com/Microsoft/vcpkg.git
) else (
    echo ✅ vcpkg directory already exists
)

cd vcpkg

REM Bootstrap vcpkg
if not exist "vcpkg.exe" (
    echo 🔨 Bootstrapping vcpkg...
    call bootstrap-vcpkg.bat
) else (
    echo ✅ vcpkg already bootstrapped
)

cd ..

REM Set environment variables
set VCPKG_ROOT=%CD%\vcpkg
set PATH=%VCPKG_ROOT%;%PATH%

echo 🔧 Installing project dependencies...

REM Install dependencies using vcpkg
echo 📦 Installing vcpkg packages for x64-windows...
vcpkg\vcpkg.exe install --triplet=x64-windows

REM Create build directory
echo 📁 Creating build directory...
if not exist "build" mkdir build

REM Configure the project
echo 🔧 Configuring project with CMake...
cmake -S . -B build ^
    -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -A x64

echo 🔨 Building project...
cmake --build build --config Release --parallel

REM Create sample configuration
echo ⚙️ Creating sample configuration...
(
echo {
echo   "xeno_ai": {
echo     "endpoint": "https://api.xenolabs.ai",
echo     "api_key": "your_xeno_api_key_here"
echo   },
echo   "open_router": {
echo     "endpoint": "https://openrouter.ai/api/v1",
echo     "api_key": "your_openrouter_key_here"
echo   },
echo   "ollama": {
echo     "endpoint": "http://localhost:11434"
echo   }
echo }
) > build\config.json

REM Create environment setup batch file
(
echo @echo off
echo REM Source this file to set up your development environment
echo set VCPKG_ROOT=%CD%\vcpkg
echo set PATH=%%VCPKG_ROOT%%;%%PATH%%
echo set CMAKE_TOOLCHAIN_FILE=%%VCPKG_ROOT%%\scripts\buildsystems\vcpkg.cmake
echo.
echo echo Environment configured for Xeno Software Suite development
echo echo VCPKG_ROOT: %%VCPKG_ROOT%%
) > build\env.bat

REM Run tests
echo 🧪 Running tests...
cd build
ctest --output-on-failure --parallel --build-config Release
cd ..

echo.
echo 🎉 Setup complete!
echo.
echo 📋 Next steps:
echo    1. Edit build\config.json with your API keys
echo    2. Run: build\env.bat (to set up environment)
echo    3. Run: build\apps\launcher\Release\xeno-launcher.exe (to start the launcher)
echo.
echo 🔧 Development commands:
echo    • Build:     cmake --build build --config Release --parallel
echo    • Test:      cd build ^&^& ctest --output-on-failure --build-config Release
echo    • Clean:     cmake --build build --target clean --config Release
echo.
echo 📖 See CONTRIBUTING.md for detailed development guidelines
echo 🌟 Happy coding with Xeno Software Suite!

pause