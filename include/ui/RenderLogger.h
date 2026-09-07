// Purpose of this class is to avoid spamming 
// render error messages that occur every paint
#pragma once

#include <unordered_map>
#include <string>

enum class RenderLogLevel {
    Info = 0,
    Warning,
    Error,
};
struct RenderLogMessage {
    RenderLogLevel level=RenderLogLevel::Info;
    std::string persistentMessage=nullptr;
    std::string data=nullptr;
};

class RenderLogger {
public:
    /* singleton */
    static RenderLogger& getInstance() {
        static RenderLogger rl;
        return rl;
    }
    RenderLogger(RenderLogger const&) = delete;
    void operator=(RenderLogger const&) = delete;
    /* singleton */

    RenderLogger();

    void log(RenderLogMessage msg);

private:
    std::unordered_map<std::string, std::string> messages;
};