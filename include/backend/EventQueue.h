#pragma once


#include <variant>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>


struct Response {
    std::string ev;
};

struct Command {
    std::string cmd;
};

struct Event {
    std::variant<Command,Response> event;
};

class EventQueue {
    private:
        std::condition_variable mCV;
        std::mutex mMutex;
        std::queue<Event> mEventQueue;
    public:
        EventQueue(): mCV(), mMutex(), mEventQueue() {};

        void process(){
            std::unique_lock<std::mutex> lock(mMutex);
            //mCV.wait(lock, [this] { return mEventQueue.empty(); });
        };

        void pushEvent(Event& event){
            std::unique_lock<std::mutex> lock(mMutex);
            mEventQueue.push(event);
            //mCV.notify_one();
        };
        /*
        Event popEvent(){
            std::lock_guard<std::mutex> lock(mMutex);
            Event data = mEventQueue.back();
            mEventQueue.pop();
            return data;
        };*/
};