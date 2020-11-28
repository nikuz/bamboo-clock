#ifndef Screen_h
#define Screen_h

#include <Arduino.h>
#include <FastLED.h>

class Screen {
public:
    Screen();

    ~Screen();

    static void initiate();

    static void refresh(int curHours, int curMinutes, bool dotsShown);

    static void setBrightness();

    static void generateRainbow();

private:
    static void getSplitTime(int value, int result[]);

    static bool customColorIsSet();

    static unsigned long getCustomColorHex();

    static void showNumber(int number, CRGB *leds);

    static void showNumber0(CRGB *leds);

    static void showNumber1(CRGB *leds);

    static void showNumber2(CRGB *leds);

    static void showNumber3(CRGB *leds);

    static void showNumber4(CRGB *leds);

    static void showNumber5(CRGB *leds);

    static void showNumber6(CRGB *leds);

    static void showNumber7(CRGB *leds);

    static void showNumber8(CRGB *leds);

    static void showNumber9(CRGB *leds);
};

#endif /* Screen_h */