#ifndef AppStorage_h
#define AppStorage_h

#include <Arduino.h>

class AppStorage {
public:
    AppStorage();

    ~AppStorage();

    static unsigned int getUInt(const char *key, int value);

    static unsigned long getULong(const char *key, unsigned long value);

    static String getString(const char *key, String value);

    static void putUInt(const char *key, int value);

    static void putULong(const char *key, unsigned long value);

    static void putString(const char *key, String value);
};

#endif /* AppStorage_h */