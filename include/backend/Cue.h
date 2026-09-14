#pragma once

#include <cstdint>
#include <string>

namespace BBackend {

    enum class CueState : uint8_t {
        INVALID=0,
        STANDBY,
        RUNNING,
        ERROR,
        PAUSED,
    };

    class Cue {

    private:
        std::string mName{};
        std::string mDescription{};
        uint16_t mId = 0; // 0 is invalid
        CueState mState = CueState::INVALID;

    public:

        Cue(uint16_t id){};
        ~Cue(){};

        std::string name() const {return mName;}
        std::string description() const {return mDescription;}
        uint16_t id() const {return mId;}

        void setName(std::string& name);
        void setDescription(std::string& desc);
    };
} // namespace BBackend