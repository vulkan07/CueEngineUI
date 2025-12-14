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

    explicit Cue(const QString& name, const QString& description = "");

    virtual ~Cue() = default;
};

class MediaCue : public Cue {
};

class TextCue : public Cue {
};