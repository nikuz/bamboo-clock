#include "def.h"
#include "AppWiFi.h"
#include "AppTime.h"
#include "Screen.h"
#include "AppServer.h"
#include "vars.h"

int curHours = 0;
int curMinutes = 0;
unsigned long dotsBlinkTimer = 1001;
bool dotsShown = false;
bool backButtonIsPressed = false;
bool serverStarted = false;
int secondsToShoutDownServer = 0;

static unsigned long &serverStartTime = Vars::getULong("serverStartTime");
static unsigned long &timeRefreshTimer = Vars::getULong("timeRefreshTimer");

void setup() {
    pinMode(BUTTON_PIN, INPUT);
    pinMode(LIGHT_SENSOR_PIN, INPUT);

    Serial.begin(115200);

    Vars::initiate();

    Screen::initiate();
    AppTime::RTCBegin();

    startWifi();
}

void loop() {
    Screen::setBrightness();

    if (timeRefreshTimer == 0 || timeRefreshTimer < millis()) {
        updateTime();
    }

    int dotsBlinkDelay = serverStartTime != 0 ? DOTS_BLINK_SHORT_DELAY : DOTS_BLINK_DELAY;
    if (millis() - dotsBlinkTimer > dotsBlinkDelay) {
        dotsBlinkTimer = millis();
        if (dotsShown) {
            dotsShown = false;
        } else {
            dotsShown = true;
        }

        Screen::refresh(curHours, curMinutes, dotsShown);
    }

    bool backButtonPressedNow = digitalRead(BUTTON_PIN) == HIGH;
    if (backButtonPressedNow && !backButtonIsPressed) {
        backButtonIsPressed = true;
    } else if (!backButtonPressedNow && backButtonIsPressed) {
        backButtonIsPressed = false;
        serverStartTime = millis();
        AppWiFi::startAP();
        AppServer::start();

        secondsToShoutDownServer = SERVER_LIFETIME;
    }

    if (serverStartTime != 0) {
        AppWiFi::handleDNSLoop();
    }

    if (serverStartTime != 0 && millis() - serverStartTime > SERVER_LIFETIME) {
        serverStartTime = 0;
        secondsToShoutDownServer = 0;
        AppServer::stop();
        AppWiFi::stopAP();

        startWifi();
    }

    // Server lifetime countdown
    if (secondsToShoutDownServer > 0) {
        const int serverCountDown = (SERVER_LIFETIME - (millis() - serverStartTime)) / 1000;
        if (serverCountDown > 0 && serverCountDown != secondsToShoutDownServer) {
            secondsToShoutDownServer = serverCountDown;
            Serial.print("Server shutdown after: ");
            Serial.println(serverCountDown);
        }
    }
}

void startWifi() {
    int &ntpEnabled = Vars::getInt("ntpEnabled");
    String &customSSID = Vars::getString("customSSID");
    String &customPassword = Vars::getString("customPassword");

    if (ntpEnabled == 1 && customSSID != "") {
        AppWiFi::connect(customSSID.c_str(), customPassword.c_str());
        AppTime::obtainSNTP();
        AppTime::RTCDateTimeUpdate();
        updateTime();
    }
};

void updateTime() {
    if (AppTime::RTCIsDateTimeValid()) {
        struct tm curTime = AppTime::getCurrentTime();
        // Hours
        int hours = curTime.tm_hour;
        if (curHours != hours) {
            curHours = hours;
        }

        // Minutes
        int minutes = curTime.tm_min;
        if (curMinutes != minutes) {
            curMinutes = minutes;
        }

        long millisecondsToRefresh = 1000L * (60L - curTime.tm_sec);

        timeRefreshTimer = millis() + millisecondsToRefresh;
    }
}
