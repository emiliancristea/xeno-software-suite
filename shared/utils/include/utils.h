#pragma once

#include <string>
#include <vector>
#include <map>

namespace xeno::utils {

/**
 * @brief Configuration manager for application settings
 */
class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    bool loadConfig(const std::string& config_path);
    bool saveConfig(const std::string& config_path);
    
    std::string getString(const std::string& key, const std::string& default_value = "");
    int getInt(const std::string& key, int default_value = 0);
    bool getBool(const std::string& key, bool default_value = false);
    
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setBool(const std::string& key, bool value);

private:
    ConfigManager() = default;
    std::map<std::string, std::string> config_data;
};

/**
 * @brief Logging utility
 */
class Logger {
public:
    enum Level {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3
    };
    
    static Logger& getInstance();
    
    void setLevel(Level level);
    void log(Level level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    Logger() = default;
    Level current_level = INFO;
};

/**
 * @brief Platform utilities
 */
class Platform {
public:
    enum OS {
        Windows,
        macOS,
        Linux,
        Unknown
    };
    
    static OS getOS();
    static std::string getOSString();
    static std::string getAppDataPath();
    static std::string getTempPath();
    static bool isPathExists(const std::string& path);
    static bool createDirectory(const std::string& path);
};

} // namespace xeno::utils