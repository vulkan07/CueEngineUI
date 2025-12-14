#pragma once

#include "backend/Cue.h"

#include <vector>
#include <memory>
#include <QObject>

class Backend : public QObject {
    Q_OBJECT

private:
    std::vector<std::unique_ptr<Cue>> mCues;

    Backend() : QObject() {}

public:

    /* singleton */
    static Backend& getInstance() {
        static Backend backend;
        return backend;
    }
    Backend(Backend const&) = delete;
    void operator=(Backend const&) = delete;
    /* singleton */


    //
    void addCue(std::unique_ptr<Cue> cue);
    // returns nullptr if index is invalid
    Cue* getCue(int index);
    // returns the internal list of cues
    const std::vector<std::unique_ptr<Cue>>& getCues() const;
    // returns the number of cues
    size_t getLength();
};

#define backend Backend::getInstance()