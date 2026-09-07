#include "ui/RenderLogger.h"
#include <spdlog/spdlog.h>


RenderLogger::RenderLogger(): messages() {

}

void RenderLogger::log(RenderLogMessage msg) {
    if (msg.persistentMessage=="")
        return;

    // already has message
    if (messages.find(msg.persistentMessage) != messages.end()
        &&
        messages[msg.persistentMessage] == msg.data
    ) return;
    
    messages.emplace(msg.persistentMessage, msg.data);
    std::string fullmsg = msg.persistentMessage + " " + msg.data;

    switch (msg.level) {
        case RenderLogLevel::Info:
            spdlog::info(fullmsg);
            break;
        case RenderLogLevel::Warning:
            spdlog::warn(fullmsg);
            break;
        case RenderLogLevel::Error:
            spdlog::error(fullmsg);
            break;
    }

    
}