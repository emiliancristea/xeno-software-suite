# Contributing to Xeno Software Suite

Thank you for your interest in contributing to the Xeno Software Suite! This document provides guidelines for contributing to our AI-enhanced creative tools platform.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Project Structure](#project-structure)
- [Coding Standards](#coding-standards)
- [Platform Integration Guidelines](#platform-integration-guidelines)
- [Testing Guidelines](#testing-guidelines)
- [Pull Request Process](#pull-request-process)
- [Security Considerations](#security-considerations)

## Code of Conduct

By participating in this project, you agree to maintain a respectful and inclusive environment for all contributors.

## Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/your-username/xeno-software-suite.git
   cd xeno-software-suite
   ```
3. **Set up the development environment** (see below)
4. **Create a feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Prerequisites

- **CMake** 3.20 or higher
- **Qt 6.5** or higher
- **vcpkg** package manager
- **C++20** compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)

### Initial Setup

#### Linux/macOS
```bash
# Run the setup script
./scripts/setup.sh

# Build the project
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --parallel
```

#### Windows
```cmd
# Run the setup script
.\scripts\setup.bat

# Build the project
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --parallel --config Release
```

### Configuration

Create a `config.json` file in your build directory for API testing:

```json
{
  "xeno_ai": {
    "endpoint": "https://api.xenolabs.ai",
    "api_key": "your_test_key"
  },
  "open_router": {
    "endpoint": "https://openrouter.ai/api/v1",
    "api_key": "your_test_key"
  },
  "ollama": {
    "endpoint": "http://localhost:11434"
  }
}
```

## Project Structure

```
xeno-software-suite/
├── apps/                    # Individual applications
│   ├── launcher/           # Central hub application
│   ├── image-edit/         # AI-powered image editor
│   ├── video-edit/         # Video editor with AI features
│   ├── audio-edit/         # Audio editor with AI tools
│   └── xeno-code/          # AI-assisted IDE
├── shared/                 # Shared libraries
│   ├── ai-integration/     # AI service integration
│   └── utils/              # Common utilities
├── tests/                  # Test suites
├── docs/                   # Documentation
└── scripts/               # Development scripts
```

### Application Structure

Each application in `apps/` follows this structure:
```
app-name/
├── src/                   # Source files
├── include/               # Header files (if needed)
├── resources/             # Qt resources, icons, etc.
├── tests/                 # Application-specific tests
└── CMakeLists.txt         # Build configuration
```

## Coding Standards

### C++ Guidelines

- **C++20 Standard**: Use modern C++ features appropriately
- **Qt Conventions**: Follow Qt naming conventions for Qt-related code
- **RAII**: Use Resource Acquisition Is Initialization principles
- **Smart Pointers**: Prefer `std::unique_ptr` and `std::shared_ptr` over raw pointers
- **Const Correctness**: Use `const` wherever appropriate

### Code Style

- **Indentation**: 4 spaces (no tabs)
- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `AIIntegration`)
  - Functions/Methods: `camelCase` (e.g., `processImage`)
  - Variables: `snake_case` (e.g., `current_image`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_CREDITS`)
  - Private members: trailing underscore (e.g., `private_member_`)

### Documentation

- **Header Comments**: Include file purpose and copyright
- **Function Documentation**: Use Doxygen-style comments for public APIs
- **Inline Comments**: Explain complex logic and platform-specific code

Example:
```cpp
/**
 * @brief Processes an image using AI enhancement
 * @param image Input image to process
 * @param enhancement_type Type of AI enhancement to apply
 * @return Enhanced image data or error status
 */
AIResponse enhanceImage(const cv::Mat& image, const std::string& enhancement_type);
```

## Platform Integration Guidelines

### Credit System Accuracy

- **Always validate credits** before AI operations
- **Accurately track credit usage** for billing transparency
- **Handle insufficient credits gracefully** with clear user messaging
- **Test credit deduction logic** thoroughly

### API Integration

- **Error Handling**: Implement robust error handling for all API calls
- **Timeout Management**: Set appropriate timeouts for network requests
- **Rate Limiting**: Respect API rate limits and implement backoff strategies
- **Security**: Never log or expose API keys in debug output

### Cross-Platform Compatibility

- **Qt Framework**: Use Qt abstractions for platform-specific functionality
- **File Paths**: Use Qt's path handling for cross-platform compatibility
- **Networking**: Use Qt's network classes for HTTP requests
- **Threading**: Use Qt's threading mechanisms for background operations

## Testing Guidelines

### Unit Tests

- **Test Coverage**: Aim for >80% coverage of shared libraries
- **Mock Dependencies**: Use mocking for external dependencies (AI APIs)
- **Credit Logic**: Thoroughly test all credit-related functionality
- **Edge Cases**: Test boundary conditions and error scenarios

### Integration Tests

- **End-to-End Workflows**: Test complete user workflows
- **API Integration**: Test with mock and staging environments
- **Platform Compatibility**: Test on all supported platforms

### Running Tests

```bash
# Build and run all tests
cmake --build build
cd build && ctest --output-on-failure

# Run specific test suite
./tests/unit/test_shared_libraries
```

## Pull Request Process

### Before Submitting

1. **Run all tests** and ensure they pass
2. **Build on all platforms** (or test with CI)
3. **Update documentation** for any API changes
4. **Follow commit message conventions**:
   ```
   type(scope): description
   
   Examples:
   feat(ai): add voice cloning functionality
   fix(credits): correct credit deduction calculation
   docs(readme): update installation instructions
   ```

### PR Requirements

- **Clear Description**: Explain what changes were made and why
- **Platform Integration**: Note any changes to credit system or API integration
- **Security Impact**: Highlight any security-related changes
- **Breaking Changes**: Clearly document any breaking changes
- **Testing**: Describe testing performed

### Review Criteria

- **Code Quality**: Follows coding standards and best practices
- **Platform Security**: Maintains security of credit system and API keys
- **Credit Accuracy**: Credit calculations are correct and well-tested
- **Cross-Platform**: Works correctly on Windows, macOS, and Linux
- **Performance**: No significant performance regressions

## Security Considerations

### API Key Management

- **Never commit API keys** to the repository
- **Use environment variables** or secure configuration files
- **Implement key rotation** capabilities
- **Log security events** appropriately

### Credit System Security

- **Validate all credit operations** on both client and server
- **Implement audit trails** for credit transactions
- **Secure communication** with Xeno Labs API
- **Handle edge cases** securely (network failures, etc.)

### Data Protection

- **Encrypt sensitive data** in transit and at rest
- **Implement GDPR compliance** measures
- **Minimize data collection** to necessary information only
- **Secure temporary files** and clean up properly

## Getting Help

- **Issues**: Use GitHub Issues for bug reports and feature requests
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Documentation**: Check the `docs/` directory for detailed guides
- **Community**: Join the Xeno Labs forums for community support

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License.