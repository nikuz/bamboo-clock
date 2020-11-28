#ifndef AppTime_h
#define AppTime_h

#include <Arduino.h>
#include <Time.h>

class AppTime {
public:
    AppTime();

    ~AppTime();

    static void obtainSNTP();

    static bool localTime(struct tm *timeinfo);

    static void RTCBegin();

    static void RTCUpdateByNtp();

    static int RTCGetTemperature();

    static struct tm RTCGetCurrentTime();

    static struct tm getCurrentTime();

    static int getCurrentHour();

    static int getCurrentMinute();

    static char *getTimeString(char format[] = "%02u:%02u");

    static struct tm getTmFromString(const char *value);

    static void RTCDateTimeUpdate();

    static void userTimeUpdate(int hour, int minute);

    static bool RTCIsDateTimeValid();
};

#endif /* AppTime_h */
