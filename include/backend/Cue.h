#pragma once

#include <QString>

enum CueAction {
    Default,
    DoNothing,
};
enum CueState {
    Stopped,
    Running,
    PreWait,
    PostWait,
    Error
};

class Cue {
public:
    QString mName;
    QString mDescription;
    uint mPreWait = 0; // msec
    uint mPostWait = 0; //msec
    CueAction mNextAction = Default;
    CueState mState = Stopped;
};

class MediaCue : public Cue {
public:
    MediaCue();
};