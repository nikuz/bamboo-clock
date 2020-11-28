#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <DNSServer.h>

#include "def.h"
#include "AppWiFi.h"
#include "AppTime.h"

AppWiFi::AppWiFi() {}

AppWiFi::~AppWiFi() {}

SemaphoreHandle_t _connected;
DNSServer dnsServer;

void AppWiFi::handleDNSLoop() {
    dnsServer.processNextRequest();
}

void onEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
        case SYSTEM_EVENT_STA_DISCONNECTED:
            xSemaphoreTake(_connected, 0);
            switch (info.disconnected.reason) {
                case WIFI_REASON_NOT_AUTHED:
                case WIFI_REASON_AUTH_FAIL:
                    WiFi.reconnect();
                    break;
            }
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
        case SYSTEM_EVENT_AP_STACONNECTED:
            xSemaphoreGive(_connected);
            break;
    }
}

void AppWiFi::connect(const char* ssid, const char* password) {
    _connected = xSemaphoreCreateBinary();
    auto on_event_id = WiFi.onEvent(onEvent);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    if (xSemaphoreTake(_connected, 10000)) {
        Serial.printf("Connected: %s\n", WiFi.localIP().toString().c_str());
    }  else {
        Serial.printf("Timed out waiting for connection\n");
    }
}

bool AppWiFi::isConnected() {
    return WiFi.isConnected();
}

void AppWiFi::reconnect() {
    if (!AppWiFi::isConnected()) {
        WiFi.reconnect();
        for (int loops = 10; loops > 0; loops--) {
            if (WiFi.isConnected()) {
                //get internet time
                AppTime::obtainSNTP();
                AppTime::RTCDateTimeUpdate();
                break;
            } else {
                DEBUG_PRINTLN(loops);
                delay(1000);
            }
        }
    }
}

void AppWiFi::startAP() {
    WiFi.disconnect(true);

    WiFi.softAP(AP_SSID, AP_PASSWORD);
    dnsServer.start(53, "*", WiFi.softAPIP());

    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void AppWiFi::stopAP() {
    WiFi.softAPdisconnect(true);
    Serial.println("AP disconnected");
}