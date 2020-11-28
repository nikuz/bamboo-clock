#include <Arduino.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <Time.h>

RtcDS3231 <TwoWire> Rtc(Wire);

#include "def.h"
#include "AppTime.h"
#include "AppWiFi.h"

const char *ntpServer = "1.rs.pool.ntp.org";
const char *ntpServer2 = "0.europe.pool.ntp.org";
const char *ntpServer3 = "1.europe.pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
static int blankVariable = -1;

struct tm NTPCurrentTime = {
        tm_sec: -1,
        tm_min: -1,
        tm_hour: -1,
        tm_mday: -1,
        tm_mon: -1,
        tm_year: -1,
        tm_wday: -1,
        tm_yday: -1,
        tm_isdst: -1
};
struct tm RTCCurrentTime = {
        tm_sec: -1,
        tm_min: -1,
        tm_hour: -1,
        tm_mday: -1,
        tm_mon: -1,
        tm_year: -1,
        tm_wday: -1,
        tm_yday: -1,
        tm_isdst: -1
};

static inline unsigned long elapsed() { return millis(); }
int overFlowCounter;

AppTime::AppTime() {}

AppTime::~AppTime() {}

void AppTime::RTCBegin() {
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    if (!Rtc.IsDateTimeValid()) {
        Rtc.SetDateTime(compiled);
        Serial.println("RTC lost confidence in the DateTime!");
    }
    if (!Rtc.GetIsRunning()) {
        Rtc.SetIsRunning(true);
        Serial.println("RTC was not actively running, starting now");
    }
    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) {
        Rtc.SetDateTime(compiled);
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
}

void AppTime::obtainSNTP() {
    if (AppWiFi::isConnected()) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, ntpServer2, ntpServer3);
    }
}

bool AppTime::localTime(struct tm *timeinfo) {
    return getLocalTime(timeinfo);
}

int AppTime::RTCGetTemperature() {
    RtcTemperature rtcTemp = Rtc.GetTemperature();
    return rtcTemp.AsFloatDegC();
}

bool AppTime::RTCIsDateTimeValid() {
    return Rtc.IsDateTimeValid();
}

struct tm AppTime::RTCGetCurrentTime() {
    RtcDateTime rtcTime = Rtc.GetDateTime();

    struct tm dateTime = {
        tm_sec: rtcTime.Second(),
        tm_min: rtcTime.Minute(),
        tm_hour: rtcTime.Hour(),
        tm_mday: rtcTime.Day(),
        tm_mon: rtcTime.Month() - 1, // 0 based
        tm_year: rtcTime.Year() - 1900,
        tm_wday: 0,
        tm_yday: 0,
        tm_isdst: -1
    };

    return dateTime;
}

struct tm AppTime::getCurrentTime() {
    struct tm timeinfo = NTPCurrentTime;
    if (!AppWiFi::isConnected() || !AppTime::localTime(&timeinfo)) {
        if (AppTime::RTCIsDateTimeValid()) {
            timeinfo = AppTime::RTCGetCurrentTime();
        } else {
            // AppTime::RTCBegin();
            // AppTime::RTCUpdateByNtp();
        }
    }

    return timeinfo;
}

int AppTime::getCurrentHour() {
    struct tm currentTime = AppTime::getCurrentTime();
    return currentTime.tm_hour;
}

int AppTime::getCurrentMinute() {
    struct tm currentTime = AppTime::getCurrentTime();
    return currentTime.tm_min;
}

char *AppTime::getTimeString(char format[]) {
    struct tm currentTime = AppTime::getCurrentTime();
    static char timeString[6];
    snprintf_P(
        timeString,
        sizeof timeString,
        PSTR(format),
        currentTime.tm_hour,
        currentTime.tm_min
    );
    return timeString;
}

struct tm AppTime::getTmFromString(const char *value) {
    // 10/10/2018 00:09:21
    int month = -1;
    int day = -1;
    int year = -1;
    int hours = -1;
    int minutes = -1;
    int seconds = -1;

    char dateString[20];
    strncpy(dateString, value, 20);

    char *pch;
    const char *delimiter = " /:";
    pch = strtok(dateString, delimiter);
    int i = 0;
    while (pch != NULL) {
        switch (i) {
            case 0:
                month = atoi(pch);
                break;
            case 1:
                day = atoi(pch);
                break;
            case 2:
                year = atoi(pch);
                break;
            case 3:
                hours = atoi(pch);
                break;
            case 4:
                minutes = atoi(pch);
                break;
            case 5:
                seconds = atoi(pch);
                break;
        }
        pch = strtok(NULL, delimiter);
        i++;
    }

    struct tm dateTime = {
            tm_sec: seconds,
            tm_min: minutes,
            tm_hour: hours,
            tm_mday: day,
            tm_mon: month - 1, // 0 based
            tm_year: year - 1900,
            tm_wday: 0,
            tm_yday: 0,
            tm_isdst: -1
    };

    return dateTime;
}

void AppTime::RTCDateTimeUpdate() {
    const char months[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    struct tm ntpTime = {0};

    if (getLocalTime(&ntpTime)) {
        char date[15];
        char time[10];

        sprintf(
            date,
            "%.3s%3d %4d",
            months[ntpTime.tm_mon],
            ntpTime.tm_mday,
            ntpTime.tm_year + 1900
        );

        sprintf(
            time,
            "%02d:%02d:%02d",
            ntpTime.tm_hour,
            ntpTime.tm_min,
            ntpTime.tm_sec
        );

        // 10/10/2018 00:09:21
        RtcDateTime ntpDateTime = RtcDateTime(date, time);
        Rtc.SetDateTime(ntpDateTime);
    }
}

void AppTime::userTimeUpdate(int hour, int minute) {
    RtcDateTime curTime = Rtc.GetDateTime();
    char timeString[9];
    snprintf_P(
        timeString,
        sizeof timeString,
        PSTR("%02u:%02u:%02u"),
        hour,
        minute,
        curTime.Second()
    );
    Serial.print("Set: ");
    Serial.println(timeString);
    Rtc.SetDateTime(RtcDateTime(__DATE__, timeString));
}