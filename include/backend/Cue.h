#pragma once

#include <QString>

enum class CueType {
    TextCue,
    MediaCue
};

class Cue {
public:
    QString mName;
    QString mDescription;

    // in msec
    uint mPreWait = 0;
    uint mPostWait = 0;

    Cue(const QString& name, const QString& description = "");

    virtual CueType getType() const = 0;
    virtual ~Cue() = default;
};

class MediaCue : public Cue {
public:
    CueType getType() const override {return CueType::MediaCue;}

    uint mDuration = 0;

    MediaCue(const QString& name, uint _duration, const QString& description = "");
};

class TextCue : public Cue {
public:
    CueType getType() const override {return CueType::TextCue;}

    TextCue(const QString& name, const QString& description = "");
};