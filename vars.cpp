#include <Arduino.h>

#include "def.h"
#include "vars.h"
#include "AppStorage.h"

static String customColor = "";
static int ntpEnabled = 0;
static String customSSID = "";
static String customPassword = "";
static unsigned long serverStartTime = 0;
static unsigned long timeRefreshTimer = 0;
static int brightness = 50;
static int brightnessAuto = 1;

static IntVariable intVariables[] = {
        IntVariable(&ntpEnabled, "ntpEnabled", true),
        IntVariable(&brightness, "brightness", true),
        IntVariable(&brightnessAuto, "brightnessAuto", true),
};
static ULongVariable uLongVariables[] = {
        ULongVariable(&serverStartTime, "serverStartTime", false),
        ULongVariable(&timeRefreshTimer, "timeRefreshTimer", false),
};
static StringVariable stringVariables[] = {
        StringVariable(&customColor, "customColor", true),
        StringVariable(&customSSID, "customSSID", true),
        StringVariable(&customPassword, "customPassword", true),
};
static int intFish = -32000;
static unsigned long uLongFish = 0;
static String stringFish = "";

Vars::Vars() {}

Vars::~Vars() {}

void Vars::initiate() {
    int intVarsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < intVarsLen; i++) {
        if (intVariables[i].key && intVariables[i].store) {
            *intVariables[i].var = AppStorage::getUInt(intVariables[i].key, *intVariables[i].var);
        }
    }
    int uLongVarsLen = *(&uLongVariables + 1) - uLongVariables;
    for (int i = 0; i < uLongVarsLen; i++) {
        if (uLongVariables[i].key && uLongVariables[i].store) {
            *uLongVariables[i].var = AppStorage::getULong(uLongVariables[i].key, *uLongVariables[i].var);
        }
    }
    int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].key && stringVariables[i].store) {
            *stringVariables[i].var = AppStorage::getString(stringVariables[i].key, *stringVariables[i].var);
        }
    }
}

void Vars::set(const char *key, int value) {
    int intVarsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < intVarsLen; i++) {
        if (intVariables[i].key == key) {
            IntVariable variable = intVariables[i];
            *variable.var = value;

            if (variable.store) {
                AppStorage::putUInt(key, value);
            }
            break;
        }
    }
}

void Vars::set(const char *key, unsigned long value) {
    int uLongVarsLen = *(&uLongVariables + 1) - uLongVariables;
    for (int i = 0; i < uLongVarsLen; i++) {
        if (uLongVariables[i].key == key) {
            ULongVariable variable = uLongVariables[i];
            *variable.var = value;

            if (variable.store) {
                AppStorage::putULong(key, value);
            }
            break;
        }
    }
}

void Vars::set(const char *key, String value) {
    int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].key == key) {
            StringVariable variable = stringVariables[i];
            *variable.var = value;

            if (variable.store) {
                AppStorage::putString(key, value);
            }
            break;
        }
    }
}

int &Vars::getInt(const char *key) {
    const int intVarsLen = *(&intVariables + 1) - intVariables;
    for (int i = 0; i < intVarsLen; i++) {
        if (intVariables[i].key == key) {
            return *intVariables[i].var;
        }
    }

    return intFish;
}

unsigned long &Vars::getULong(const char *key) {
    const int uLongVarsLen = *(&uLongVariables + 1) - uLongVariables;
    for (int i = 0; i < uLongVarsLen; i++) {
        if (uLongVariables[i].key == key) {
            return *uLongVariables[i].var;
        }
    }

    return uLongFish;
}

String &Vars::getString(const char *key) {
    const int stringVarsLen = *(&stringVariables + 1) - stringVariables;
    for (int i = 0; i < stringVarsLen; i++) {
        if (stringVariables[i].key == key) {
            return *stringVariables[i].var;
        }
    }

    return stringFish;
}