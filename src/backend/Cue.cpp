#include "backend/Cue.h"

Cue::Cue(const QString& name, const QString& description) :mName(name), mDescription(description) {
}

MediaCue::MediaCue(const QString& name, uint _duration, const QString& description) 
    :Cue(name,description), mDuration(_duration) {
}


TextCue::TextCue(const QString& name, const QString& description) 
    :Cue(name,description) {
}