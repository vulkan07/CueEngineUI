#pragma once

#include "backend/CueList.h"

namespace BBackend {

    enum class SessionState : uint8_t {
        INVALID=0,
        MODIFIED,
        SAVED,
    };

    class Session {
        private:
            std::string mTitle {};
            SessionState mState = SessionState::INVALID;

            CueList mCueList {};
            

        public:
            Session(){};
            //Session(std::filesystem::path path);
            ~Session(){};

            void save(){};
    };

} // namespace BBackend