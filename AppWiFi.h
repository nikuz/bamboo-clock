#ifndef AppWiFi_h
#define AppWiFi_h

#include <Arduino.h>

class AppWiFi {
public:
    AppWiFi();

    ~AppWiFi();

    static void handleDNSLoop();

    static void connect(const char* ssid, const char* password);

    static void reconnect();

    static void startAP();

    static void stopAP();

    static bool isConnected();

};

#endif /* AppWiFi_h */