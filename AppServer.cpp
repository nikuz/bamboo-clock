#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "def.h"
#include "AppServer.h"
#include "AppTime.h"
#include "screen.h"
#include "vars.h"

AsyncWebServer server(80);

AppServer::AppServer() {}

AppServer::~AppServer() {}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request){
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    }
};

void AppServer::start() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/style.css", "text/css");
    });
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/script.js", "text/javascript");
    });

    // API
    server.on("/sync", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        StaticJsonDocument<500> doc;
        doc["time"] = AppTime::getTimeString();
        doc["ntp"] = Vars::getInt("ntpEnabled");
        doc["color"] = Vars::getString("customColor");
        doc["ssid"] = Vars::getString("customSSID");
        doc["brightness"] = Vars::getInt("brightness");
        doc["brightnessAuto"] = Vars::getInt("brightnessAuto");
        serializeJsonPretty(doc, *response);
        request->send(response);
    });
    server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            AsyncWebParameter* p = request->getParam("value");
            const char *value = p->value().c_str();

            char hourStr[3];
            memcpy(hourStr, &value[0], 2);
            hourStr[3] = '\0';
            int hour = atoi(hourStr);

            char minuteStr[3];
            memcpy(minuteStr, &value[3], 2);
            minuteStr[3] = '\0';
            int minute = atoi(minuteStr);

            AppTime::userTimeUpdate(hour, minute);
            unsigned long &timeRefreshTimer = Vars::getULong("timeRefreshTimer");
            timeRefreshTimer = 0;

            request->send(200, "text/plain", "OK");
        } else {
            request->send(404, "text/plain", "No value sent");
        }

        AppServer::refreshStartTime();
    });
    server.on("/ntp", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            AsyncWebParameter* p = request->getParam("value");
            Vars::set("ntpEnabled", atoi(p->value().c_str()));
            request->send(200, "text/plain", "OK");
        } else {
            request->send(404, "text/plain", "No value sent");
        }

        AppServer::refreshStartTime();
    });
    server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid") && request->hasParam("password")) {
            AsyncWebParameter* ssid = request->getParam("ssid");
            Vars::set("customSSID", ssid->value().c_str());

            AsyncWebParameter* password = request->getParam("password");
            Vars::set("customPassword", password->value().c_str());

            request->send(200, "text/plain", "OK");
        } else {
            request->send(404, "text/plain", "No value sent");
        }

        AppServer::refreshStartTime();
    });
    server.on("/color", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            request->send(200, "text/plain", "OK");

            AsyncWebParameter* p = request->getParam("value");
            Vars::set("customColor", p->value().c_str());
            Screen::generateRainbow();
        } else {
            request->send(404, "text/plain", "No value sent");
        }

        AppServer::refreshStartTime();
    });
    server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("auto") || request->hasParam("value")) {
            request->send(200, "text/plain", "OK");

            if (request->hasParam("auto")) {
                AsyncWebParameter *brightnessAuto = request->getParam("auto");
                Vars::set("brightnessAuto", atoi(brightnessAuto->value().c_str()));
            }

            if (request->hasParam("value")) {
                AsyncWebParameter* brightness = request->getParam("value");
                Vars::set("brightness", atoi(brightness->value().c_str()));
            }
        } else {
            request->send(404, "text/plain", "No value sent");
        }

        AppServer::refreshStartTime();
    });
    server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request) {
        unsigned long &serverStartTime = Vars::getULong("serverStartTime");
        request->send(200, "text/plain", "OK");
        serverStartTime = millis() + SERVER_LIFETIME + 1000L;
    });

    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

    server.begin();
}

void AppServer::stop() {
    SPIFFS.end();
    server.reset();
    server.end();
    Serial.println("Server stopped");
}

void AppServer::refreshStartTime() {
    unsigned long &serverStartTime = Vars::getULong("serverStartTime");
    serverStartTime = millis();
}