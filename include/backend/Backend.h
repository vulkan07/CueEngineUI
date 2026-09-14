#pragma once

#include "backend/EventQueue.h"
#include "backend/Session.h"

#include <spdlog/spdlog.h>
#include <thread>
#include <chrono>

namespace BBackend {

    enum class BackendState {
        UNINITIALIZED=0,
        INVALID,
        READY,
    };

    class Backend {
    private:

        std::thread mThread;
        Session mSession;
        EventQueue mEventQueue;
        BackendState mState = BackendState::UNINITIALIZED;

        Backend(){
            spdlog::set_level(spdlog::level::debug); //TMP
            spdlog::debug("Backend constructor");

            mThread = std::thread([this](){
                this->init();
                this->loop();
            });
        };

        void init() {
            spdlog::debug("Backend init begin");
            mState=BackendState::INVALID;

            mSession = Session();
            mEventQueue = EventQueue();

            mState=BackendState::READY;
            spdlog::debug("Backend init end");
        }

        void loop(){
            while (true) {
                mEventQueue.process();
            }
        };


    public:
        // Singleton //
        static Backend& getInstance() {
            static Backend backend;
            return backend;
        }
        Backend(Backend const&) = delete;
        void operator=(Backend const&) = delete;


    };

};