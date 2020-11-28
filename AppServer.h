#ifndef AppServer_h
#define AppServer_h

#include <Arduino.h>
#include <FastLED.h>

class AppServer {
public:
    AppServer();

    ~AppServer();

    static void start();

    static void stop();

private:
    static void refreshStartTime();
};

#endif /* AppServer_h */