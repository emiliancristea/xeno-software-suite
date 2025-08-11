#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
    #include <shlobj.h>
#elif __APPLE__
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#endif

namespace xeno::utils {

// ConfigManager implementation
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& config_path) {
    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                config_data[key] = value;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigManager::saveConfig(const std::string& config_path) {
    try {
        std::ofstream file(config_path);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& [key, value] : config_data) {
            file << key << "=" << value << std::endl;
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string ConfigManager::getString(const std::string& key, const std::string& default_value) {
    auto it = config_data.find(key);
    return it != config_data.end() ? it->second : default_value;
}

int ConfigManager::getInt(const std::string& key, int default_value) {
    auto it = config_data.find(key);
    if (it != config_data.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception&) {
            return default_value;
        }
    }
    return default_value;
}

bool ConfigManager::getBool(const std::string& key, bool default_value) {
    auto it = config_data.find(key);
    if (it != config_data.end()) {
        return it->second == "true" || it->second == "1";
    }
    return default_value;
}

void ConfigManager::setString(const std::string& key, const std::string& value) {
    config_data[key] = value;
}

void ConfigManager::setInt(const std::string& key, int value) {
    config_data[key] = std::to_string(value);
}

void ConfigManager::setBool(const std::string& key, bool value) {
    config_data[key] = value ? "true" : "false";
}

// Logger implementation
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLevel(Level level) {
    current_level = level;
}

void Logger::log(Level level, const std::string& message) {
    if (level >= current_level) {
        const char* level_strings[] = {"DEBUG", "INFO", "WARNING", "ERROR"};
        std::cout << "[" << level_strings[level] << "] " << message << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

// Platform implementation
Platform::OS Platform::getOS() {
#ifdef _WIN32
    return Windows;
#elif __APPLE__
    return macOS;
#elif __linux__
    return Linux;
#else
    return Unknown;
#endif
}

std::string Platform::getOSString() {
    switch (getOS()) {
        case Windows: return "Windows";
        case macOS: return "macOS";
        case Linux: return "Linux";
        default: return "Unknown";
    }
}

std::string Platform::getAppDataPath() {
#ifdef _WIN32
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\XenoSoftwareSuite";
    }
    return "";
#elif __APPLE__
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Application Support/XenoSoftwareSuite";
    }
    return "";
#else
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.config/XenoSoftwareSuite";
    }
    return "";
#endif
}

std::string Platform::getTempPath() {
#ifdef _WIN32
    char temp_path[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_path);
    return std::string(temp_path);
#else
    return "/tmp";
#endif
}

bool Platform::isPathExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool Platform::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace xeno::utils