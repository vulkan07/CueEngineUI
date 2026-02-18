#include "ui/Utils.h"

QString stringFromDuration(uint msec) {
    if (msec >= 60*60*1000) // 1 hour
        return QTime(0,0).addMSecs(msec).toString("hh:mm:ss");
    else
        return QTime(0,0).addMSecs(msec).toString("mm:ss");
}