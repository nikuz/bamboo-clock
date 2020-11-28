#define VERSION_ID "1"
#define PRODUCTION 0
#define DEBUG 1
#define RAINBOW 1

#if DEBUG
#define DEBUG_PRINT(x)    Serial.print(x)
#define DEBUG_PRINTLN(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

// LED
#define LED_TYPE           WS2812
#define LED_COLOR_ORDER    GRB
#define LED_NUM            16
#define LED_MIN_BRIGHTNESS 10
#define LED_MAX_BRIGHTNESS 255

// pins
#define DIGIT_1_PIN        33
#define DIGIT_2_PIN        32
#define DIGIT_3_PIN        15
#define DIGIT_4_PIN        13
#define DOTS_PIN           17
#define BUTTON_PIN         27
#define LIGHT_SENSOR_PIN   36

// clock
#define DOTS_BLINK_DELAY         1000L
#define DOTS_BLINK_SHORT_DELAY   300L

// server
#define AP_SSID            "Bamboo Clock"
#define AP_PASSWORD        "clock-ap-"
#define SERVER_LIFETIME    1000L * 120