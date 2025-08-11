cmake_minimum_required(VERSION 3.20)

project(XenoSoftwareSuite 
    VERSION 1.0.0
    DESCRIPTION "AI-enhanced creative and coding tools integrated with Xeno Labs platform"
    LANGUAGES CXX
)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia Quick WebChannel)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(unofficial-ffmpeg REQUIRED)
find_package(portaudio REQUIRED)
find_package(SndFile REQUIRED)
find_package(httplib REQUIRED)
find_package(nlohmann_json REQUIRED)

# Enable testing
enable_testing()
find_package(GTest REQUIRED)

# Set global compile features
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add subdirectories
add_subdirectory(shared)
add_subdirectory(apps)
add_subdirectory(tests)

# Install configuration
include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")

# CPack configuration for packaging
include(CPack)
set(CPACK_PACKAGE_NAME "XenoSoftwareSuite")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "Xeno AI")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")