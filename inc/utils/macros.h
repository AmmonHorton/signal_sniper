#ifndef MACROS_H
#define MACROS_H

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

enum LogLevel {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};

class Logger {
    public:
        static void log(LogLevel level, const std::string& message) {
            switch (level) {
                case LOG_LEVEL_DEBUG:
                    std::cout << "[DEBUG] " << message << std::endl;
                    break;
                case LOG_LEVEL_INFO:
                    std::cout << "[INFO] " << message << std::endl;
                    break;
                case LOG_LEVEL_WARN:
                    std::cout << "[WARN] " << message << std::endl;
                    break;
                case LOG_LEVEL_ERROR:
                    std::cerr << "[ERROR] " << message << std::endl;
                    break;
            }
        }
    };
    
// Define logging macros
#define LOG_DEBUG(message) \
    do { \
        std::ostringstream oss; \
        oss << message; \
        Logger::log(LOG_LEVEL_DEBUG, oss.str()); \
    } while (false)

#define LOG_INFO(message) \
    do { \
        std::ostringstream oss; \
        oss << message; \
        Logger::log(LOG_LEVEL_INFO, oss.str()); \
    } while (false)

#define LOG_WARN(message) \
    do { \
        std::ostringstream oss; \
        oss << message; \
        Logger::log(LOG_LEVEL_WARN, oss.str()); \
    } while (false)

#define LOG_ERROR(message) \
    do { \
        std::ostringstream oss; \
        oss << message; \
        Logger::log(LOG_LEVEL_ERROR, oss.str()); \
    } while (false)


// Define check macros
#define EXPECT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            std::ostringstream oss; \
            oss << message; \
            throw std::runtime_error(oss.str()); \
        } \
    } while (false)

#endif // MACROS_H