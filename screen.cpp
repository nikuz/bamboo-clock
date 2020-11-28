#include <Arduino.h>
#include <FastLED.h>

#include "def.h"
#include "screen.h"
#include "vars.h"

FASTLED_USING_NAMESPACE

CRGB digit_1_leds[LED_NUM];
CRGB digit_2_leds[LED_NUM];
CRGB digit_3_leds[LED_NUM];
CRGB digit_4_leds[LED_NUM];
CRGB dots_leds[2];
unsigned long defaultColor = 0xFF0000;
int prevLightSensorValue = 0;
int &brightness = Vars::getInt("brightness");
int &brightnessAuto = Vars::getInt("brightnessAuto");

unsigned long rainbowOriginal[LED_NUM] = {
    0xFF0000,
    0xFB4901,
    0xE98200,
    0xE5B601,
    0xE5DB01,
    0xB6F201,
    0x3EFE02,
    0x01FE29,
    0x02F16D,
    0x01CDB5,
    0x016CF2,
    0x1502FF,
    0x5B02F6,
    0x9502DE,
    0xC300C0,
    0xE1028C,
};

unsigned long rainbow[LED_NUM];

Screen::Screen() {}

Screen::~Screen() {}

void Screen::initiate() {
    Screen::generateRainbow();
    FastLED.addLeds<LED_TYPE, DIGIT_1_PIN, LED_COLOR_ORDER>(digit_1_leds, LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DIGIT_2_PIN, LED_COLOR_ORDER>(digit_2_leds, LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DIGIT_3_PIN, LED_COLOR_ORDER>(digit_3_leds, LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DIGIT_4_PIN, LED_COLOR_ORDER>(digit_4_leds, LED_NUM).setCorrection(TypicalLEDStrip);
    FastLED.addLeds<LED_TYPE, DOTS_PIN, LED_COLOR_ORDER>(dots_leds, 2).setCorrection(TypicalLEDStrip);

    FastLED.setBrightness(LED_MIN_BRIGHTNESS);

    Screen::showNumber(0, digit_1_leds);
    Screen::showNumber(0, digit_2_leds);
    Screen::showNumber(0, digit_3_leds);
    Screen::showNumber(0, digit_4_leds);
}

void Screen::refresh(int curHours, int curMinutes, bool dotsShown) {
    unsigned long &serverStartTime = Vars::getULong("serverStartTime");

    int hoursSeparated[2];
    Screen::getSplitTime(curHours, hoursSeparated);
    Screen::showNumber(hoursSeparated[0], digit_1_leds);
    Screen::showNumber(hoursSeparated[1], digit_2_leds);

    int minutesSeparated[2];
    Screen::getSplitTime(curMinutes, minutesSeparated);
    Screen::showNumber(minutesSeparated[0], digit_3_leds);
    Screen::showNumber(minutesSeparated[1], digit_4_leds);

    unsigned long dotsColor = Screen::customColorIsSet() ? Screen::getCustomColorHex() : defaultColor;
    dotsColor = dotsShown ? dotsColor : 0x000000;
    dots_leds[0] = dotsColor;
    dots_leds[1] = dotsColor;
    FastLED.show();
}

void Screen::setBrightness() {
    int mapBrightness;

    if (brightnessAuto == 1) {
        mapBrightness = map(analogRead(LIGHT_SENSOR_PIN), 0, 3000, LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS);
    } else {
        mapBrightness = map(brightness, 1, 100, LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS);
    }

    if (mapBrightness > LED_MAX_BRIGHTNESS) {
        mapBrightness = LED_MAX_BRIGHTNESS;
    }
    if (prevLightSensorValue != mapBrightness) {
        prevLightSensorValue = mapBrightness;
        FastLED.setBrightness(mapBrightness);
        FastLED.show();
    }
}

void Screen::getSplitTime(int value, int result[]) {
    if (value >= 10) {
        int firstDigit = value;
        while (firstDigit >= 10) {
            firstDigit /= 10;
        }
        result[0] = firstDigit;
        result[1] = value - firstDigit * 10;
    } else {
        result[0] = 0;
        result[1] = value;
    }
}

bool Screen::customColorIsSet() {
    String &customColor = Vars::getString("customColor");
    return customColor != "" && customColor != "rainbow";
}

unsigned long Screen::getCustomColorHex() {
    if (Screen::customColorIsSet()) {
        String &customColor = Vars::getString("customColor");
        return strtol(String("0x" + customColor).c_str(), NULL, 16);
    }

    return 0;
}

void Screen::generateRainbow() {
    unsigned long customColorHex = Screen::getCustomColorHex();
    int rainbowLength = *(&rainbow + 1) - rainbow;
    for (int i = 0; i < rainbowLength; i++) {
        if (Screen::customColorIsSet()) {
            rainbow[i] = customColorHex;
        } else {
            rainbow[i] = rainbowOriginal[i];
        }
    }
}

void Screen::showNumber(int number, CRGB *leds) {
    switch (number) {
        case 0:
            Screen::showNumber0(leds);
            break;
        case 1:
            Screen::showNumber1(leds);
            break;
        case 2:
            Screen::showNumber2(leds);
            break;
        case 3:
            Screen::showNumber3(leds);
            break;
        case 4:
            Screen::showNumber4(leds);
            break;
        case 5:
            Screen::showNumber5(leds);
            break;
        case 6:
            Screen::showNumber6(leds);
            break;
        case 7:
            Screen::showNumber7(leds);
            break;
        case 8:
            Screen::showNumber8(leds);
            break;
        case 9:
            Screen::showNumber9(leds);
            break;
    }

    FastLED.show();
}

void Screen::showNumber0(CRGB *leds) {
    for (int i = 0; i <= 1; i++) {
        leds[i] = CRGB::Black;
    }
    for (int i = 2; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}

void Screen::showNumber1(CRGB *leds) {
    for (int i = 0; i <= 3; i++) {
        leds[i] = CRGB::Black;
    }
    for (int i = 4; i <= 8; i++) {
        leds[i] = rainbow[i];
    }
    for (int i = 9; i < LED_NUM; i++) {
        leds[i] = CRGB::Black;
    }
}

void Screen::showNumber2(CRGB *leds) {
    for (int i = 0; i <= 6; i++) {
        leds[i] = rainbow[i];
    }
    leds[7] = CRGB::Black;
    for (int i = 8; i <= 13; i++) {
        leds[i] = rainbow[i];
    }
    leds[14] = CRGB::Black;
    leds[15] = rainbow[15];
}

void Screen::showNumber3(CRGB *leds) {
    for (int i = 0; i <= 11; i++) {
        leds[i] = rainbow[i];
    }
    leds[12] = CRGB::Black;
    leds[13] = rainbow[13];
    leds[14] = CRGB::Black;
    leds[15] = rainbow[15];
}

void Screen::showNumber4(CRGB *leds) {
    for (int i = 0; i <= 1; i++) {
        leds[i] = rainbow[i];
    }
    for (int i = 2; i <= 3; i++) {
        leds[i] = CRGB::Black;
    }
    for (int i = 4; i <= 8; i++) {
        leds[i] = rainbow[i];
    }
    for (int i = 9; i <= 12; i++) {
        leds[i] = CRGB::Black;
    }
    for (int i = 13; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}

void Screen::showNumber5(CRGB *leds) {
    for (int i = 0; i <= 4; i++) {
        leds[i] = rainbow[i];
    }
    leds[5] = CRGB::Black;
    for (int i = 6; i <= 11; i++) {
        leds[i] = rainbow[i];
    }
    leds[12] = CRGB::Black;
    for (int i = 13; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}

void Screen::showNumber6(CRGB *leds) {
    for (int i = 0; i <= 4; i++) {
        leds[i] = rainbow[i];
    }
    leds[5] = CRGB::Black;
    for (int i = 6; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}

void Screen::showNumber7(CRGB *leds) {
    for (int i = 0; i <= 1; i++) {
        leds[i] = CRGB::Black;
    }
    for (int i = 2; i < 8; i++) {
        leds[i] = rainbow[i];
    }
    for (int i = 9; i < LED_NUM - 1; i++) {
        leds[i] = CRGB::Black;
    }
    leds[15] = rainbow[15];
}

void Screen::showNumber8(CRGB *leds) {
    for (int i = 0; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}

void Screen::showNumber9(CRGB *leds) {
    for (int i = 0; i <= 11; i++) {
        leds[i] = rainbow[i];
    }
    leds[12] = CRGB::Black;
    for (int i = 13; i < LED_NUM; i++) {
        leds[i] = rainbow[i];
    }
}